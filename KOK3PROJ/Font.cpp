#include "Font.h"
#include "Engine.h"
#include "debug.h"
#include "Mesh.h"

Font::Font() {
	m_device = nullptr;
	m_d2dFactory = nullptr;
	m_renderTarget = nullptr;
	m_brush = nullptr;
	m_factory = nullptr;
	m_textFormat = nullptr;
	m_textureDesc = nullptr;
	m_sharedResource = nullptr;
	m_keyedmutex1 = nullptr;
	m_keyedmutex2 = nullptr;
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
}

HRESULT Font::InitScreen(ID3D11Device* device, IDXGIAdapter1* adapter) {
	HRESULT hr = D3D10CreateDevice1(adapter, D3D10_DRIVER_TYPE_HARDWARE, 
		NULL, D3D10_CREATE_DEVICE_DEBUG | D3D10_CREATE_DEVICE_BGRA_SUPPORT,
		D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &m_device);

	D3D11_TEXTURE2D_DESC sharedTexDesc;
	ZeroMemory(&sharedTexDesc, sizeof(D3D11_TEXTURE2D_DESC));
	sharedTexDesc.Width = ::engine.getWindowDesc()->width;
	sharedTexDesc.Height = ::engine.getWindowDesc()->height;
	sharedTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sharedTexDesc.MipLevels = 1;
	sharedTexDesc.ArraySize = 1;
	sharedTexDesc.SampleDesc.Count = 1;
	sharedTexDesc.Usage = D3D11_USAGE_DEFAULT;
	sharedTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	sharedTexDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
	hr = device->CreateTexture2D(&sharedTexDesc, NULL, &m_textureDesc);
	hr = m_textureDesc->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&m_keyedmutex2);

	IDXGIResource* sharedResource10 = nullptr;
	HANDLE sharedHandle10;

	hr = m_textureDesc->QueryInterface(__uuidof(IDXGIResource), (void**)&sharedResource10);
	hr = sharedResource10->GetSharedHandle(&sharedHandle10);
	sharedResource10->Release();

	IDXGISurface1* sharedSurface10 = nullptr;
	hr = device->OpenSharedResource(sharedHandle10, __uuidof(IDXGISurface1), (void**)(&sharedSurface10));
	hr = sharedSurface10->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&m_keyedmutex1);

	ID2D1Factory* D2DFactory = nullptr;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), (void**)&D2DFactory);

	D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties;
	ZeroMemory(&renderTargetProperties, sizeof(renderTargetProperties));

	renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
	renderTargetProperties.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);

	hr = D2DFactory->CreateDxgiSurfaceRenderTarget(sharedSurface10, &renderTargetProperties, &m_renderTarget);
	sharedSurface10->Release();
	D2DFactory->Release();
	
	hr = m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 0.0f, 1.0f), &m_brush);

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&m_factory));

	hr = m_factory->CreateTextFormat(
		L"Microsoft Sans Serif",
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		18.0f,
		L"en-US",
		&m_textFormat
	);

	hr = m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	hr = m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	m_device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);

	Vertex v[] = {
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
	device->CreateBuffer(&indexBufferDesc, &iinitData, &m_indexBuffer);

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexBufferData.pSysMem = v;
	hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_vertexBuffer);

	device->CreateShaderResourceView(m_textureDesc, NULL, &m_sharedResource);
	return hr;
}

void Font::Render(ID3D11DeviceContext* deviceContext, const std::wstring text) {
	m_keyedmutex2->ReleaseSync(0);
	m_keyedmutex1->AcquireSync(0, 5);

	m_renderTarget->BeginDraw();
	m_renderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
	D2D1_COLOR_F FontColor = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
	m_brush->SetColor(FontColor);
	D2D1_RECT_F layoutRect = D2D1::RectF(0, 0,
		::engine.getWindowDesc()->width,
		::engine.getWindowDesc()->height);

	m_renderTarget->DrawText(
		text.c_str(), wcslen(text.c_str()),
		m_textFormat, layoutRect, m_brush
	);
	m_renderTarget->EndDraw();

	m_keyedmutex1->ReleaseSync(1);
	m_keyedmutex2->AcquireSync(1, 5);

	deviceContext->OMSetBlendState(::engine.m_transparency, NULL, 0xffffffff);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	::engine.cbPerObj.WVP = XMMatrixIdentity();
	::engine.cbPerObj.WVP = XMMatrixTranspose(::engine.cbPerObj.WVP);
	deviceContext->UpdateSubresource(::engine.m_preObjectBuffer, 0, NULL, &::engine.cbPerObj, 0, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &::engine.m_preObjectBuffer);
	deviceContext->PSSetShaderResources(0, 1, &m_sharedResource);
	deviceContext->PSSetSamplers(0, 1, &::engine.m_textureSamplerState);
	deviceContext->DrawIndexed(6, 0, 0);
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
	if (m_keyedmutex1) m_keyedmutex1->Release();
	if (m_keyedmutex2) m_keyedmutex2->Release();
	if (m_vertexBuffer) m_vertexBuffer->Release();
	if (m_indexBuffer) m_indexBuffer->Release();
}