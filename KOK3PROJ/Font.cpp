#include "Font.h"
#include "Engine.h"
#include "debug.h"
#include "Mesh.h"

Font::Font() {
	m_device = nullptr;
	m_mutex1 = nullptr;
	m_mutex2 = nullptr;
	m_renderTarget = nullptr;
	m_brush = nullptr;
	m_backBuffer = nullptr;
	m_sharedTexture = nullptr;
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_shaderView = nullptr;
	m_factory = nullptr;
	m_textFormat = nullptr;
}

HRESULT Font::InitFontDevice(IDXGIAdapter* adapter, ID3D11Device* device11) {
    HRESULT hresult{};
    hresult = D3D10CreateDevice1(adapter,
        D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_DEBUG | D3D10_CREATE_DEVICE_BGRA_SUPPORT,
        D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &m_device);
    if (FAILED(hresult)) {
        ERROR_MSG("Error creating device adapter. %d error code.", hresult);
        return hresult;
    }

    D3D11_TEXTURE2D_DESC sharedTexDesc;
    ZeroMemory(&sharedTexDesc, sizeof(sharedTexDesc));
    sharedTexDesc.Width = ::engine.getWindowDesc()->width;
    sharedTexDesc.Height = ::engine.getWindowDesc()->height;
    sharedTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sharedTexDesc.MipLevels = 1;
    sharedTexDesc.ArraySize = 1;
    sharedTexDesc.SampleDesc.Count = 1;
    sharedTexDesc.Usage = D3D11_USAGE_DEFAULT;
    sharedTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    sharedTexDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
    hresult = device11->CreateTexture2D(&sharedTexDesc, NULL, &m_sharedTexture);
    if (FAILED(hresult)) {
        ERROR_MSG("Error creating shared texture. %d error code.", hresult);
        return hresult;
    }

    hresult = m_sharedTexture->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&m_mutex1);
    if (FAILED(hresult)) {
        ERROR_MSG("Error querying IDXGIKeyedMutex for shared texture. %d error code.", hresult);
        return hresult;
    }

    IDXGIResource* sharedResource10 = nullptr;
    HANDLE sharedHandle10;
    hresult = m_sharedTexture->QueryInterface(__uuidof(IDXGIResource), (void**)&sharedResource10);
    if (FAILED(hresult)) {
        ERROR_MSG("Error querying IDXGIResource for shared texture. %d error code.", hresult);
        return hresult;
    }

    hresult = sharedResource10->GetSharedHandle(&sharedHandle10);
    sharedResource10->Release();
    if (FAILED(hresult)) {
        ERROR_MSG("Error getting shared handle for shared texture. %d error code.", hresult);
        return hresult;
    }

    IDXGISurface1* sharedSurface10 = nullptr;
    hresult = m_device->OpenSharedResource(sharedHandle10,
        __uuidof(IDXGISurface1), (void**)(&sharedSurface10));
    if (FAILED(hresult)) {
        ERROR_MSG("Error opening shared resource. %d error code.", hresult);
        return hresult;
    }

    hresult = sharedSurface10->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&m_mutex2);
    if (FAILED(hresult)) {
        ERROR_MSG("Error querying IDXGIKeyedMutex for shared surface. %d error code.", hresult);
        sharedSurface10->Release();
        return hresult;
    }

    ID2D1Factory* D2DFactory = nullptr;
    hresult = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), (void**)&D2DFactory);
    if (FAILED(hresult)) {
        ERROR_MSG("Error creating Direct2D factory. %d error code.", hresult);
        sharedSurface10->Release();
        return hresult;
    }

    D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties;
    ZeroMemory(&renderTargetProperties, sizeof(renderTargetProperties));
    renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
    renderTargetProperties.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);
    hresult = D2DFactory->CreateDxgiSurfaceRenderTarget(sharedSurface10, &renderTargetProperties, &m_renderTarget);
    if (FAILED(hresult)) {
        ERROR_MSG("Error creating Direct2D render target. %d error code.", hresult);
        sharedSurface10->Release();
        D2DFactory->Release();
        return hresult;
    }
    sharedSurface10->Release();
    D2DFactory->Release();

    hresult = m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 0.0f, 1.0f), &m_brush);
    if (FAILED(hresult)) {
        ERROR_MSG("Error creating Direct2D solid color brush. %d error code.", hresult);
        return hresult;
    }

    hresult = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&m_factory));
    if (FAILED(hresult)) {
        ERROR_MSG("Error creating DirectWrite factory. %d error code.", hresult);
        return hresult;
    }

    hresult = m_factory->CreateTextFormat(
        L"Script", NULL,
        DWRITE_FONT_WEIGHT_REGULAR,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL, 24.0f,
        L"en-us", &m_textFormat
    );
    if (FAILED(hresult)) {
        ERROR_MSG("Error creating DirectWrite text format. %d error code.", hresult);
        return hresult;
    }

    hresult = m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    if (FAILED(hresult)) {
        ERROR_MSG("Error setting text alignment. %d error code.", hresult);
        return hresult;
    }

    hresult = m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    if (FAILED(hresult)) {
        ERROR_MSG("Error setting paragraph alignment. %d error code.", hresult);
        return hresult;
    }
    m_device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
    return hresult;
}

HRESULT Font::InitScreen(ID3D11Device* device) {
    HRESULT hresult{};
    Vertex vertex[] = {
        Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
        Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
        Vertex(1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
        Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
    };

    DWORD indices[] = {
        0,  1,  2,
        0,  2,  3,
    };

    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(DWORD) * 2 * 3;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA iinitData;
    ZeroMemory(&iinitData, sizeof(D3D11_SUBRESOURCE_DATA));
    iinitData.pSysMem = indices;
    hresult = device->CreateBuffer(&indexBufferDesc, &iinitData, &m_indexBuffer);
    if (FAILED(hresult)) {
        ERROR_MSG("Error creating index buffer. %d error code.", hresult);
        return hresult;
    }

    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * 4;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexBufferData;
    ZeroMemory(&vertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
    vertexBufferData.pSysMem = vertex;
    hresult = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_vertexBuffer);
    if (FAILED(hresult)) {
        ERROR_MSG("Error creating vertex buffer. %d error code.", hresult);
        return hresult;
    }

    hresult = device->CreateShaderResourceView(m_sharedTexture, NULL, &m_shaderView);
    if (FAILED(hresult)) {
        ERROR_MSG("Error creating shader resource view. %d error code.", hresult);
        return hresult;
    }

    return hresult;
}

void Font::Render() {

}

void Font::Release() {
	if (m_device) m_device->Release();
	if (m_mutex1) m_mutex1->Release();
	if (m_mutex2) m_mutex2->Release();
	if (m_renderTarget) m_renderTarget->Release();
	if (m_brush) m_brush->Release();
	if (m_backBuffer) m_backBuffer->Release();
	if (m_sharedTexture) m_sharedTexture->Release();
	if (m_vertexBuffer) m_vertexBuffer->Release();
	if (m_indexBuffer) m_indexBuffer->Release();
	if (m_shaderView) m_shaderView->Release();
	if (m_factory) m_factory->Release();
	if (m_textFormat) m_textFormat->Release();
}
