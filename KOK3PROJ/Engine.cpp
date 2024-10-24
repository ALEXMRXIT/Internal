#include "Engine.h"
#include "debug.h"
#include "Shader.h"

Engine::Engine() {
	m_windowDesc = nullptr;
}

Engine::~Engine() {

}

bool Engine::InitWindowDevice(const WindowDescription* desc) {
	m_windowDesc = const_cast<WindowDescription*>(desc);

	WNDCLASSEX wndClassEx;
	ZeroMemory(&wndClassEx, sizeof(WNDCLASSEX));
    wndClassEx.cbSize = sizeof(WNDCLASSEX);
    wndClassEx.style = CS_HREDRAW | CS_VREDRAW;
    wndClassEx.lpfnWndProc = WindowProcessor;
    wndClassEx.cbClsExtra = NULL;
    wndClassEx.cbWndExtra = NULL;
    wndClassEx.hInstance = m_windowDesc->hInstance;
    wndClassEx.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClassEx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
    wndClassEx.lpszMenuName = NULL;
    wndClassEx.lpszClassName = m_windowDesc->title;
    wndClassEx.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

    CHECK_ASSERT(RegisterClassEx(&wndClassEx), "Error register window class");
    m_windowDesc->hWnd = CreateWindowEx(NULL, m_windowDesc->title, m_windowDesc->title,
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, m_windowDesc->width,
        m_windowDesc->height, NULL, NULL, m_windowDesc->hInstance, NULL);
    CHECK_ASSERT(m_windowDesc->hWnd, "Error create window");

    ShowWindow(m_windowDesc->hWnd, m_windowDesc->nCmdShow);
    UpdateWindow(m_windowDesc->hWnd);

	return true;
}

bool Engine::InitRenderDevice() {
    HRESULT handleResult{};

    DXGI_MODE_DESC backBufferDesc;
    ZeroMemory(&backBufferDesc, sizeof(DXGI_MODE_DESC));
    backBufferDesc.Width = m_windowDesc->width;
    backBufferDesc.Height = m_windowDesc->height;
    backBufferDesc.RefreshRate.Numerator = 60;
    backBufferDesc.RefreshRate.Denominator = 1;
    backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    backBufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    backBufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
    swapChainDesc.BufferDesc = backBufferDesc;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = m_windowDesc->hWnd;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    handleResult = D3D11CreateDeviceAndSwapChain(
        NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, 
        &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
    if (FAILED(handleResult)) {
        ERROR_MSG("Error create swap chain device. %d error code.", handleResult);
        return false;
    }

    ID3D11Texture2D* backBuffer;
    ZeroMemory(&backBuffer, sizeof(ID3D11Texture2D));
    handleResult = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    if (FAILED(handleResult)) {
        ERROR_MSG("Error getting back buffer. %d error code.", handleResult);
        return false;
    }

    handleResult = m_device->CreateRenderTargetView(backBuffer, NULL, &m_renderTargetView);
    backBuffer->Release();
    if (FAILED(handleResult)) {
        ERROR_MSG("Error create render target view. %d error code.", handleResult);
        return false;
    }

    D3D11_TEXTURE2D_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(D3D11_TEXTURE2D_DESC));
    depthStencilDesc.Width = m_windowDesc->width;
    depthStencilDesc.Height = m_windowDesc->height;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    m_device->CreateTexture2D(&depthStencilDesc, NULL, &m_depthTexture);
    if (FAILED(handleResult)) {
        ERROR_MSG("Error create texture 2D for buffer depth stencil. %d error code.", handleResult);
        return false;
    }

    m_device->CreateDepthStencilView(m_depthTexture, NULL, &m_depthStencilView);
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

    m_shader = new Shader();

    if (!InitScene())
        return false;

    return true;
}

bool Engine::InitScene() {
    HRESULT handleResult{};
    
    if (FAILED(m_shader->LoadVertexShader(m_device, m_deviceContext, "shader.fx")))
        return E_FAIL;
    if (FAILED(m_shader->LoadPixelShader(m_device, m_deviceContext, "shader.fx")))
        return E_FAIL;

    Vertex vertex[] = {
        Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
            Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
            Vertex(1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
            Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

            // Back Face
            Vertex(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f),
            Vertex(1.0f, -1.0f, 1.0f, 0.0f, 1.0f),
            Vertex(1.0f,  1.0f, 1.0f, 0.0f, 0.0f),
            Vertex(-1.0f,  1.0f, 1.0f, 1.0f, 0.0f),

            // Top Face
            Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f),
            Vertex(-1.0f, 1.0f,  1.0f, 0.0f, 0.0f),
            Vertex(1.0f, 1.0f,  1.0f, 1.0f, 0.0f),
            Vertex(1.0f, 1.0f, -1.0f, 1.0f, 1.0f),

            // Bottom Face
            Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
            Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
            Vertex(1.0f, -1.0f,  1.0f, 0.0f, 0.0f),
            Vertex(-1.0f, -1.0f,  1.0f, 1.0f, 0.0f),

            // Left Face
            Vertex(-1.0f, -1.0f,  1.0f, 0.0f, 1.0f),
            Vertex(-1.0f,  1.0f,  1.0f, 0.0f, 0.0f),
            Vertex(-1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
            Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

            // Right Face
            Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
            Vertex(1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
            Vertex(1.0f,  1.0f,  1.0f, 1.0f, 0.0f),
            Vertex(1.0f, -1.0f,  1.0f, 1.0f, 1.0f),
    };

    DWORD indices[] = {
        // front face
        0,  1,  2,
        0,  2,  3,

        // Back Face
        4,  5,  6,
        4,  6,  7,

        // Top Face
        8,  9, 10,
        8, 10, 11,

        // Bottom Face
        12, 13, 14,
        12, 14, 15,

        // Left Face
        16, 17, 18,
        16, 18, 19,

        // Right Face
        20, 21, 22,
        20, 22, 23
    };

    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(DWORD) * 12 * 3;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA indexBufferData;
    ZeroMemory(&indexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
    indexBufferData.pSysMem = indices;
    handleResult = m_device->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_indexBuffer);
    if (FAILED(handleResult)) {
        ERROR_MSG("Failed to create index buffer. %d error code.", handleResult);
        return false;
    }
    m_deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * 24;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vertexBufferData;
    ZeroMemory(&vertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
    vertexBufferData.pSysMem = vertex;
    handleResult = m_device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_vertexBuffer);
    if (FAILED(handleResult)) {
        ERROR_MSG("Failed to create vertex buffer. %d error code.", handleResult);
        return false;
    }

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    UINT numElements = ARRAYSIZE(layout);

    LPVOID buffPtr = m_shader->getVertexBlob()->GetBufferPointer();
    SIZE_T size = m_shader->getVertexBlob()->GetBufferSize();
    handleResult = m_device->CreateInputLayout(layout, numElements, buffPtr, size, &m_layout);
    if (FAILED(handleResult)) {
        ERROR_MSG("Failed to create input layout. %d error code.", handleResult);
        return false;
    }

    m_deviceContext->IASetInputLayout(m_layout);
    m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = m_windowDesc->width;
    viewport.Height = m_windowDesc->height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    m_deviceContext->RSSetViewports(1, &viewport);

    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(cbPerObject);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;

    handleResult = m_device->CreateBuffer(&bufferDesc, NULL, &m_preObjectBuffer);
    if (FAILED(handleResult)) {
        ERROR_MSG("Failed to create buffer. %d error code.", handleResult);
        return false;
    }

    camPosition = XMVectorSet(0.0f, 3.0f, -8.0f, 0.0f);
    camTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);
    float screen = static_cast<float>(m_windowDesc->width) / static_cast<float>(m_windowDesc->height);
    camProjection = XMMatrixPerspectiveFovLH(0.5f * XM_PI, screen, 1.0f, 1000.0f);
    rot = 0.01;

    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    handleResult = D3DX11CreateShaderResourceViewFromFile(m_device, "box.jpg", NULL, NULL, &CubesTexture, NULL);
    handleResult = m_device->CreateSamplerState(&sampDesc, &CubesTexSamplerState);

    return true;
}

void Engine::Update() {
    rot += .0005f;
    if (rot > 6.26f)
        rot = 0.0f;

    cube1World = XMMatrixIdentity();

    XMVECTOR rotaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    Rotation = XMMatrixRotationAxis(rotaxis, rot);
    Translation = XMMatrixTranslation(0.0f, 0.0f, 4.0f);

    cube1World = Translation * Rotation;
    cube2World = XMMatrixIdentity();

    Rotation = XMMatrixRotationAxis(rotaxis, -rot);
    Scale = XMMatrixScaling(1.5f, 1.5f, 1.5f);
    cube2World = Rotation * Scale;
}

void Engine::Render() {
    D3DXCOLOR color(0.0f, 0.0f, 0.0f, 1.0f);

    m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
    m_deviceContext->ClearDepthStencilView(m_depthStencilView, 
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    WVP = cube1World * camView * camProjection;
    cbPerObj.WVP = XMMatrixTranspose(WVP);
    m_deviceContext->UpdateSubresource(m_preObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
    m_deviceContext->VSSetConstantBuffers(0, 1, &m_preObjectBuffer);
    m_deviceContext->PSSetShaderResources(0, 1, &CubesTexture);
    m_deviceContext->PSSetSamplers(0, 1, &CubesTexSamplerState);

    m_deviceContext->DrawIndexed(36, 0, 0);

    WVP = cube2World * camView * camProjection;
    cbPerObj.WVP = XMMatrixTranspose(WVP);
    m_deviceContext->UpdateSubresource(m_preObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
    m_deviceContext->VSSetConstantBuffers(0, 1, &m_preObjectBuffer);
    m_deviceContext->PSSetShaderResources(0, 1, &CubesTexture);
    m_deviceContext->PSSetSamplers(0, 1, &CubesTexSamplerState);

    m_deviceContext->DrawIndexed(36, 0, 0);

    m_swapChain->Present(0, 0);
}

void Engine::Release() {
    if (m_swapChain) m_swapChain->Release();
    if (m_device) m_device->Release();
    if (m_deviceContext) m_deviceContext->Release();
    if (m_renderTargetView) m_renderTargetView->Release();
    if (m_shader) m_shader->Release();
    if (m_vertexBuffer) m_vertexBuffer->Release();
    if (m_indexBuffer) m_indexBuffer->Release();
    if (m_layout) m_layout->Release();
    if (m_depthStencilView) m_depthStencilView->Release();
    if (m_depthTexture) m_depthTexture->Release();
    if (m_preObjectBuffer) m_preObjectBuffer->Release();
}

int Engine::messageWindow() {
    MSG message;
    ZeroMemory(&message, sizeof(MSG));

    while (true) {
        if (PeekMessage(&message, NULL, NULL, NULL, PM_REMOVE)) {
            if (message.message == WM_QUIT)
                break;

            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        else {
            Update();
            Render();
        }
    }

    return message.wParam;
}

LRESULT Engine::WindowProcessor(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
            DestroyWindow(hWnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}