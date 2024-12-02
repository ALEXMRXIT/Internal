#include "Font.h"
#include "Engine.h"
#include "debug.h"
#include "Shader.h"

Font::Font() {
	m_device = nullptr;
	m_d2dFactory = nullptr;
	m_renderTarget = nullptr;
	m_brush = nullptr;
	m_factory = nullptr;
	m_textFormat = nullptr;
	m_textureDesc = nullptr;
	m_sharedResource = nullptr;
	m_cWcullMode = nullptr;
    m_fontShader = nullptr;
}

HRESULT Font::Init(ID3D11Device* device, ID3D11DeviceContext* context, IDXGIAdapter1* adapter) {
    HRESULT hr = D3D10CreateDevice1(adapter, D3D10_DRIVER_TYPE_HARDWARE,
        NULL, D3D10_CREATE_DEVICE_DEBUG | D3D10_CREATE_DEVICE_BGRA_SUPPORT,
        D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &m_device);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create D3D10 Device1.", hr);
        return hr;
    }

    D3D11_TEXTURE2D_DESC sharedTexDesc;
    ZeroMemory(&sharedTexDesc, sizeof(D3D11_TEXTURE2D_DESC));
    sharedTexDesc.Width = engine.getSupportedResolutin().Width;
    sharedTexDesc.Height = engine.getSupportedResolutin().Height;
    sharedTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sharedTexDesc.MipLevels = 1;
    sharedTexDesc.ArraySize = 1;
    sharedTexDesc.SampleDesc.Count = 1;
    sharedTexDesc.Usage = D3D11_USAGE_DEFAULT;
    sharedTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    hr = device->CreateTexture2D(&sharedTexDesc, NULL, &m_textureDesc);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create texture2D.", hr);
        return hr;
    }

    ID2D1Factory* D2DFactory = nullptr;
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), (void**)&D2DFactory);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create D2D1 factory.", hr);
        return hr;
    }

    D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties;
    ZeroMemory(&renderTargetProperties, sizeof(D2D1_RENDER_TARGET_PROPERTIES));
    renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
    renderTargetProperties.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);

    IDXGISurface* d2dSurface = nullptr;
    hr = m_textureDesc->QueryInterface(__uuidof(IDXGISurface), (void**)&d2dSurface);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to query IDXGISurface.", hr);
        D2DFactory->Release();
        return hr;
    }

    hr = D2DFactory->CreateDxgiSurfaceRenderTarget(d2dSurface, &renderTargetProperties, &m_renderTarget);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create DXGI surface render target.", hr);
        D2DFactory->Release();
        return hr;
    }
    D2DFactory->Release();

    hr = m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &m_brush);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create solid color brush.", hr);
        return hr;
    }

    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(&m_factory));
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create DWrite factory.", hr);
        return hr;
    }

    hr = m_factory->CreateTextFormat(
        L"Microsoft Sans Serif",
        NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        18.0f,
        L"en-US",
        &m_textFormat
    );
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create text format.", hr);
        return hr;
    }

    hr = m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to set text alignment.", hr);
        return hr;
    }

    hr = m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to set paragraph alignment.", hr);
        return hr;
    }

    m_device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST); // ???

    D3D11_RASTERIZER_DESC cmdesc;
    ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
    cmdesc.FillMode = D3D11_FILL_SOLID;
    cmdesc.CullMode = D3D11_CULL_BACK;
    cmdesc.DepthClipEnable = false;
    hr = device->CreateRasterizerState(&cmdesc, &m_cWcullMode);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create rasterizer state.", hr);
        return hr;
    }

    hr = device->CreateShaderResourceView(m_textureDesc, NULL, &m_sharedResource);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create shader resource view.", hr);
        return hr;
    }

    m_fontShader = new Shader();
    if (FAILED(m_fontShader->LoadVertexShader(device, context, "shaders\\label.fx")))
        return hr;
    if (FAILED(m_fontShader->LoadPixelShader(device, context, "shaders\\label.fx")))
        return hr;

    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = device->CreateSamplerState(&sampDesc, &m_textureSamplerState);
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Failed to create Sampler state.", hr);

    return hr;
}

void Font::Render(ID3D11DeviceContext* deviceContext, const std::wstring text) {
	m_renderTarget->BeginDraw();
	m_renderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
	D2D1_COLOR_F FontColor = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
	m_brush->SetColor(FontColor);
	D2D1_RECT_F layoutRect = D2D1::RectF(0, 0,
		engine.getSupportedResolutin().Width,
		engine.getSupportedResolutin().Height);

	m_renderTarget->DrawText(
		text.c_str(), wcslen(text.c_str()),
		m_textFormat, layoutRect, m_brush
	);
	m_renderTarget->EndDraw();

    m_bufferWVP->WVP = XMMatrixIdentity();
    m_bufferWVP->WVP = XMMatrixTranspose(m_bufferWVP->WVP);
    m_fontShader->setVertexShader(deviceContext);
    m_fontShader->setPiexlShader(deviceContext);
	deviceContext->UpdateSubresource(engine.m_preObjectBuffer, 0, NULL, &m_bufferWVP, 0, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &engine.m_preObjectBuffer);
	deviceContext->PSSetShaderResources(0, 1, &m_sharedResource);
	deviceContext->PSSetSamplers(0, 1, &m_textureSamplerState);
	deviceContext->RSSetState(m_cWcullMode);
	deviceContext->DrawIndexed(6, 0, 0);
	deviceContext->RSSetState(nullptr);
}

void Font::Release() {
	if (m_device) m_device->Release();
	if (m_d2dFactory) m_d2dFactory->Release();
	if (m_renderTarget) m_renderTarget->Release();
	if (m_brush) m_brush->Release();
	if (m_factory) m_factory->Release();
	if (m_textFormat) m_textFormat->Release();
	if (m_textureDesc) m_textureDesc->Release();
	if (m_sharedResource) m_sharedResource->Release();
	if (m_cWcullMode) m_cWcullMode->Release();
    if (m_fontShader) {
        m_fontShader->Release();
        delete m_fontShader;
    }
}