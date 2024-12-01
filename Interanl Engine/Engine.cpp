#include "Engine.h"
#include "debug.h"
#include "Shader.h"
#include "Mesh.h"
#include "Font.h"
#include "Component.h"
#include "GameObject.h"
#include "Config.h"

Engine engine;

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

    RegisterClassEx(&wndClassEx);
    m_windowDesc->hWnd = CreateWindowEx(NULL, m_windowDesc->title, m_windowDesc->title,
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, m_supportedResolution[config.resolution].Width,
        m_supportedResolution[config.resolution].Height, NULL, NULL, m_windowDesc->hInstance, NULL);

    ShowWindow(m_windowDesc->hWnd, m_windowDesc->nCmdShow);
    UpdateWindow(m_windowDesc->hWnd);
    SetWindowPos(m_windowDesc->hWnd, HWND_TOP, 0, 0, 
        GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);

    RECT windowRect;
    GetWindowRect(m_windowDesc->hWnd, &windowRect);
    m_windowDesc->rect = windowRect;

	return true;
}

HRESULT Engine::BuildMultiSampleQualityList(DXGI_FORMAT format) {
    ID3D11Device* pd3dDevice = nullptr;
    ID3D11DeviceContext* pd3dDeviceContext = nullptr;

    D3D_FEATURE_LEVEL featureLevel;
    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE,
        nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, 
        &pd3dDevice, &featureLevel, &pd3dDeviceContext);

    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Error create dive for build multysample", hr);
        return hr;
    }

    for (int iterator = 2; iterator < D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; ++iterator) {
        uint32_t quality;
        if (SUCCEEDED(pd3dDevice->CheckMultisampleQualityLevels(format, iterator, &quality))) {
            if (quality) m_qualityLevels.emplace_back(MultisampleQualityLevel(iterator, quality));
        }
    }

    if (pd3dDevice) pd3dDevice->Release();
    if (pd3dDeviceContext) pd3dDeviceContext->Release();

    return hr;
}

HRESULT Engine::GetSupportedResolutions() {
    IDXGIFactory1* dxgiFactory = nullptr;
    HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&dxgiFactory);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create DXGI factory.", hr);
        return hr;
    }

    IDXGIAdapter1* adapter = nullptr;
    hr = dxgiFactory->EnumAdapters1(0, &adapter);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to enumerate adapters.", hr);
        dxgiFactory->Release();
        return hr;
    }

    IDXGIOutput* output = nullptr;
    hr = adapter->EnumOutputs(0, &output);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to enumerate outputs.", hr);
        adapter->Release();
        dxgiFactory->Release();
        return hr;
    }

    UINT numModes = 0;
    hr = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 
        DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to get display mode list.", hr);
        output->Release();
        adapter->Release();
        dxgiFactory->Release();
        return hr;
    }

    std::vector<DXGI_MODE_DESC> displayModes(numModes);
    hr = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 
        DXGI_ENUM_MODES_INTERLACED, &numModes, displayModes.data());
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to get display mode list. %d error code.", hr);
        output->Release();
        adapter->Release();
        dxgiFactory->Release();
        return hr;
    }

    m_supportedResolution.assign(displayModes.begin(), displayModes.end());

    output->Release();
    adapter->Release();
    dxgiFactory->Release();

    return hr;
}

bool Engine::InitRenderDevice() {
    HRESULT handleResult{};

    handleResult = BuildMultiSampleQualityList(DXGI_FORMAT_R8G8B8A8_UNORM);
    if (FAILED(handleResult)) {
        DXUT_ERR_MSGBOX("Error build sample quality list.", handleResult);
        return false;
    }

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
    swapChainDesc.BufferDesc = m_supportedResolution[config.resolution];
    swapChainDesc.SampleDesc.Count = m_qualityLevels[config.multiply].SampleCount;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = m_windowDesc->hWnd;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);
    D3D_FEATURE_LEVEL createdFeatureLevel;

    handleResult = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        featureLevels, numFeatureLevels, D3D11_SDK_VERSION, 
        &swapChainDesc, &m_swapChain, &m_device, &createdFeatureLevel, &m_deviceContext);

    if (FAILED(handleResult)) {
        DXUT_ERR_MSGBOX("Error creating swap chain and device.", handleResult);
        return false;
    }

    ID3D11Texture2D* backBuffer;
    ZeroMemory(&backBuffer, sizeof(ID3D11Texture2D));
    handleResult = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    if (FAILED(handleResult)) {
        DXUT_ERR_MSGBOX("Error getting back buffer.", handleResult);
        return false;
    }

    handleResult = m_device->CreateRenderTargetView(backBuffer, NULL, &m_renderTargetView);
    backBuffer->Release();
    if (FAILED(handleResult)) {
        DXUT_ERR_MSGBOX("Error creating render target view.", handleResult);
        return false;
    }

    D3D11_TEXTURE2D_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(D3D11_TEXTURE2D_DESC));
    depthStencilDesc.Width = m_supportedResolution[config.resolution].Width;
    depthStencilDesc.Height = m_supportedResolution[config.resolution].Height;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = m_qualityLevels[config.multiply].SampleCount;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    handleResult = m_device->CreateTexture2D(&depthStencilDesc, NULL, &m_depthTexture);
    if (FAILED(handleResult)) {
        DXUT_ERR_MSGBOX("Error creating depth stencil texture.", handleResult);
        return false;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    depthStencilViewDesc.Format = depthStencilDesc.Format;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

    handleResult = m_device->CreateDepthStencilView(m_depthTexture, &depthStencilViewDesc, &m_depthStencilView);
    if (FAILED(handleResult)) {
        DXUT_ERR_MSGBOX("Error creating depth stencil view.", handleResult);
        return false;
    }

    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

    IDXGIFactory1* dxgiFactory = nullptr;
    handleResult = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&dxgiFactory);

    dxgiFactory->MakeWindowAssociation(m_windowDesc->hWnd, DXGI_MWA_NO_ALT_ENTER);

    IDXGIAdapter1* adapter = nullptr;
    handleResult = dxgiFactory->EnumAdapters1(0, &adapter);
    dxgiFactory->Release();

    m_shader = new Shader();
    m_font = new Font();

    handleResult = m_font->Init(m_device, adapter);
    if (FAILED(handleResult)) {
        DXUT_ERR_MSGBOX("Failed to init fonts.", handleResult);
        return false;
    }

    if (!InitScene()) {
        DXUT_ERR_MSGBOX("Error initializing scene.", handleResult);
        delete m_shader;
        delete m_font;
        return false;
    }
    return true;
}

bool Engine::InitScene() {
    HRESULT handleResult{};
    
    if (FAILED(m_shader->LoadVertexShader(m_device, m_deviceContext, "shader.fx")))
        return false;
    if (FAILED(m_shader->LoadPixelShader(m_device, m_deviceContext, "shader.fx")))
        return false;

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

    RenderOperation* rendOp = new RenderOperation();
    Mesh* cube1 = new Mesh();
    if (!cube1->CreateVertex(m_device, vertex, 24)) return false;
    if (!cube1->CreateIndex(m_device, indices, 36)) return false;
    if (FAILED(cube1->LoadMaterial(m_device, "box.jpg"))) return false;
    if (FAILED(cube1->Init(m_device, m_deviceContext))) return false;
    m_quewe.emplace_back(rendOp->SetRenderOperation(cube1));

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    UINT numElements = ARRAYSIZE(layout);

    LPVOID buffPtr = m_shader->getVertexBlob()->GetBufferPointer();
    SIZE_T size = m_shader->getVertexBlob()->GetBufferSize();
    handleResult = m_device->CreateInputLayout(layout, numElements, buffPtr, size, &m_layout);
    if (FAILED(handleResult)) {
        DXUT_ERR_MSGBOX("Failed to create input layout.", handleResult);
        return false;
    }

    m_deviceContext->IASetInputLayout(m_layout);
    m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (FLOAT)m_supportedResolution[config.resolution].Width;
    viewport.Height = (FLOAT)m_supportedResolution[config.resolution].Height;
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
        DXUT_ERR_MSGBOX("Failed to create buffer.", handleResult);
        return false;
    }

    camPosition = XMVectorSet(0.0f, 3.0f, -8.0f, 0.0f);
    camTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);
    float screen = (float)m_supportedResolution[config.resolution].Width / 
        (float)m_supportedResolution[config.resolution].Height;
    camProjection = XMMatrixPerspectiveFovLH(0.45f * XM_PI, screen, 1.0f, 1000.0f);

    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));
    if (config.qualityTexture == 0) sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    else if (config.qualityTexture == 10) sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    else sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.MaxAnisotropy = 1 + (config.qualityTexture * 2);
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

    D3D11_RENDER_TARGET_BLEND_DESC rtbd;
    ZeroMemory(&rtbd, sizeof(D3D11_RENDER_TARGET_BLEND_DESC));
    rtbd.BlendEnable = true;
    rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
    rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    rtbd.BlendOp = D3D11_BLEND_OP_ADD;
    rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
    rtbd.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.RenderTarget[0] = rtbd;

    handleResult = m_device->CreateSamplerState(&sampDesc, &m_textureSamplerState);
    handleResult = m_device->CreateBlendState(&blendDesc, &m_transparency);
    if (FAILED(handleResult)) {
        DXUT_ERR_MSGBOX("Failed to create blend state.", handleResult);
        return false;
    }

    setFullScreen(m_windowDesc->hWnd, config.fullscreen);

    return true;
}

void Engine::FixedUpdate(float deltaTime) {
    
}

void Engine::Update(float deltaTime) {
    for (int iterator = 0; iterator < m_quewe.size(); ++iterator) {
        m_quewe[iterator]->Update(deltaTime);
    }
}

void Engine::Render() {
    D3DXCOLOR color(0.0f, 0.0f, 0.0f, 1.0f);
    
    m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
    m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    
    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    m_deviceContext->OMSetBlendState(m_transparency, blendFactor, 0xffffffff);
    
    for (int iterator = 0; iterator < m_quewe.size(); ++iterator)
        m_quewe[iterator]->Render(m_deviceContext);
    
    wchar_t buffer[64];
    swprintf_s(buffer, 64, L"Fps: %d", m_timeInfo.fps);
    m_font->Render(m_deviceContext, buffer);

    m_swapChain->Present(config.vSync, 0);
}

void Engine::Release() {
    if (m_swapChain) m_swapChain->Release();
    if (m_device) m_device->Release();
    if (m_deviceContext) m_deviceContext->Release();
    if (m_renderTargetView) m_renderTargetView->Release();
    if (m_shader) m_shader->Release();
    if (m_layout) m_layout->Release();
    if (m_depthStencilView) m_depthStencilView->Release();
    if (m_depthTexture) m_depthTexture->Release();
    if (m_preObjectBuffer) m_preObjectBuffer->Release();

    for (int iterator = 0; iterator < m_quewe.size(); ++iterator)
        m_quewe[iterator]->Release();
    m_quewe.clear();

    if (m_font) m_font->Release();
}

int Engine::messageWindow() {
    MSG message;
    ZeroMemory(&message, sizeof(MSG));

    QueryPerformanceFrequency(&m_timeInfo.frequency);
    QueryPerformanceCounter(&m_timeInfo.start);
    m_timeInfo.frameCount = 0;
    m_timeInfo.elapsedTime = 0.0f;
    m_timeInfo.targetFrameTime = 1.0f / 30.0f;
    m_timeInfo.accumulator = 0.0f;

    while (true) {
        if (PeekMessage(&message, NULL, NULL, NULL, PM_REMOVE)) {
            if (message.message == WM_QUIT)
                break;

            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        else {
            UpdateFrequenceTime(m_timeInfo);

            while (m_timeInfo.accumulator >= m_timeInfo.targetFrameTime) {
                FixedUpdate(m_timeInfo.targetFrameTime);
                m_timeInfo.accumulator -= m_timeInfo.targetFrameTime;
            }

            Update(m_timeInfo.deltaTime);
            Render();
        }
    }

    return message.wParam;
}

void Engine::setFullScreen(HWND hwnd, bool fullscreen) {
    if (!fullscreen) {
        RECT windowRect;
        GetWindowRect(hwnd, &windowRect);
        engine.getWindowRect() = windowRect;

        SetWindowLongPtr(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
        SetWindowPos(hwnd, HWND_TOP, 0, 0,
            GetSystemMetrics(SM_CXSCREEN),
            GetSystemMetrics(SM_CYSCREEN),
            SWP_FRAMECHANGED | SWP_NOACTIVATE);
        ShowWindow(hwnd, SW_NORMAL);
    }
    else {
        SetWindowLongPtr(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));
        RECT rect{};
        if (IDXGISwapChain* chain = engine.getChain()) {
            IDXGIOutput* output = nullptr;
            chain->GetContainingOutput(&output);
            DXGI_OUTPUT_DESC desc;
            ZeroMemory(&desc, sizeof(DXGI_OUTPUT_DESC));
            output->GetDesc(&desc);
            rect = desc.DesktopCoordinates;
        }
        SetWindowPos(hwnd, HWND_NOTOPMOST,
            rect.left, rect.top,
            rect.right, rect.bottom,
            SWP_FRAMECHANGED | SWP_NOACTIVATE);
        ShowWindow(hwnd, SW_MAXIMIZE);
    }
}

const WindowDescription* Engine::getWindowDesc() const {
    return m_windowDesc;
}

RECT& Engine::getWindowRect() {
    return m_windowDesc->rect;
}

IDXGISwapChain* Engine::getChain() const {
    return m_swapChain;
}

const DXGI_MODE_DESC& Engine::getSupportedResolutin() const {
    return m_supportedResolution[config.resolution];
}

GameObject* Engine::Instantiate(primitive_type_e type, XMVECTOR position) {
    switch (type) {
    case PT_SPHERE:
    case PT_CUBE:

        break;
    }

    return nullptr;
}

LRESULT Engine::WindowProcessor(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
        } return 0;
        case WM_KEYDOWN: {
            if (wParam == VK_ESCAPE)
                DestroyWindow(hWnd);
            else if (wParam == VK_SPACE) {
                config.fullscreen = !config.fullscreen;
                engine.setFullScreen(hWnd, config.fullscreen);
            }
        } return 0;
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void Engine::UpdateFrequenceTime(PerfomanceTimeInfo& timeInfo) const {
    QueryPerformanceCounter(&timeInfo.end);
    const LONGLONG diffQuadTime = timeInfo.end.QuadPart - timeInfo.start.QuadPart;
    timeInfo.deltaTime = diffQuadTime / static_cast<float>(timeInfo.frequency.QuadPart);
    timeInfo.start = timeInfo.end;

    timeInfo.frameCount++;
    timeInfo.elapsedTime += timeInfo.deltaTime;

    if (timeInfo.elapsedTime > 1.0f) {
        timeInfo.fps = timeInfo.frameCount;
        timeInfo.frameCount = 0;
        timeInfo.elapsedTime = 0.0f;
    }

    timeInfo.accumulator += timeInfo.deltaTime;
}