#include "Engine.h"
#include "debug.h"
#include "Shader.h"
#include "Model.h"
#include "Font.h"
#include "Component.h"
#include "GameObject.h"
#include "Camera.h"
#include "Config.h"
#include "Location.h"
#include "Skybox.h"
#include "PrimitiveDrawable.h"
#include "MeshComponent.h"
#include "ShadowMap.h"
#include "ViewProjectonData.h"

Engine engine;
Camera camera;
Config config;
PrimitiveDrawable gizmozRect;
ShadowMap shadowMap;

bool Engine::InitWindowDevice(const WindowDescription* desc) {
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
    m_windowDesc = const_cast<WindowDescription*>(desc);

    WNDCLASSEX wndClassEx;
    ZeroMemory(&wndClassEx, sizeof(WNDCLASSEX));
    wndClassEx.cbSize = sizeof(WNDCLASSEX);
    wndClassEx.style = CS_HREDRAW | CS_VREDRAW;
    wndClassEx.lpfnWndProc = windowProcessor;
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

    RECT desiredClientRect = {
        0, 0,
        m_supportedResolution[config.resolution].Width,
        m_supportedResolution[config.resolution].Height
    };

    DWORD& windowStyle = m_windowDesc->windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_SIZEBOX);
    AdjustWindowRectEx(&desiredClientRect, windowStyle, FALSE, 0);

    m_windowDesc->hWnd = CreateWindowEx(NULL, m_windowDesc->title, m_windowDesc->title,
        windowStyle, CW_USEDEFAULT, CW_USEDEFAULT,
        desiredClientRect.right - desiredClientRect.left, 
        desiredClientRect.bottom - desiredClientRect.top, 
        NULL, NULL, m_windowDesc->hInstance, NULL);

    ShowWindow(m_windowDesc->hWnd, m_windowDesc->nCmdShow);
    UpdateWindow(m_windowDesc->hWnd);

    RECT clientRect;
    GetClientRect(m_windowDesc->hWnd, &clientRect);
    m_windowDesc->width = clientRect.right - clientRect.left;
    m_windowDesc->height = clientRect.bottom - clientRect.top;

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

    for (int iterator = 1; iterator < D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; ++iterator) {
        uint32_t quality;
        if (SUCCEEDED(pd3dDevice->CheckMultisampleQualityLevels(format, iterator, &quality))) {
            if (quality) m_qualityLevels.emplace_back(MultisampleQualityLevel(iterator, quality));
        }
    }

    if (pd3dDevice) pd3dDevice->Release();
    if (pd3dDeviceContext) pd3dDeviceContext->Release();

    return hr;
}

HRESULT Engine::InitDirectInput(HINSTANCE hInstance) {
    HRESULT hr{};
    hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION,
        IID_IDirectInput8, (void**)&m_directInput, NULL);

    hr = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
    hr = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);

    hr = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
    hr = m_keyboard->SetCooperativeLevel(m_windowDesc->hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    hr = m_mouse->SetDataFormat(&c_dfDIMouse);
    hr = m_mouse->SetCooperativeLevel(m_windowDesc->hWnd, (config.showCursor ?
        DISCL_NONEXCLUSIVE : DISCL_EXCLUSIVE) | DISCL_NOWINKEY | DISCL_FOREGROUND);

    return hr;
}

HRESULT Engine::GetSupportedResolutions(DXGI_FORMAT format) {
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
    hr = output->GetDisplayModeList(format, 
        DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to get display mode list.", hr);
        output->Release();
        adapter->Release();
        dxgiFactory->Release();
        return hr;
    }

    std::vector<DXGI_MODE_DESC> displayModes(numModes);
    hr = output->GetDisplayModeList(format, 
        DXGI_ENUM_MODES_INTERLACED, &numModes, displayModes.data());
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to get display mode list. %d error code.", hr);
        output->Release();
        adapter->Release();
        dxgiFactory->Release();
        return hr;
    }

    m_supportedResolution.clear();
    m_supportedResolution.assign(displayModes.begin(), displayModes.end());

    output->Release();
    adapter->Release();
    dxgiFactory->Release();

    if (config.automaticSetResolution)
        config.resolution = m_supportedResolution.size() - 1;

    return hr;
}

bool Engine::InitRenderDevice() {
    HRESULT hr{};

    hr = BuildMultiSampleQualityList(DXGI_FORMAT_R8G8B8A8_UNORM);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Error build sample quality list.", hr);
        return false;
    }

    hr = InitDirectInput(m_windowDesc->hInstance);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Error create Direct Input system.", hr);
        return false;
    }

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
    swapChainDesc.BufferDesc = m_supportedResolution[config.resolution];
    swapChainDesc.SampleDesc.Count = m_qualityLevels[config.multiply].SampleCount;
    swapChainDesc.SampleDesc.Quality = m_qualityLevels[config.multiply].QualityLevel - 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.OutputWindow = m_windowDesc->hWnd;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = config.fullscreen ? DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 0;

    D3D_DRIVER_TYPE driverTypes[] = {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);
    D3D_FEATURE_LEVEL createdFeatureLevel;
    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG) || defined(_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
    flags |= D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;
#endif
    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++) {
        hr = D3D11CreateDeviceAndSwapChain(nullptr, driverTypes[driverTypeIndex], nullptr,
            flags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &swapChainDesc, 
            &m_swapChain, &m_device, &createdFeatureLevel, &m_deviceContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Error creating swap chain and device.", hr);
        return false;
    }

    ID3D11Texture2D* backBuffer;
    ZeroMemory(&backBuffer, sizeof(ID3D11Texture2D));
    hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Error getting back buffer.", hr);
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
    depthStencilDesc.SampleDesc.Quality = m_qualityLevels[config.multiply].QualityLevel - 1;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    hr = m_device->CreateTexture2D(&depthStencilDesc, NULL, &m_depthTexture);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Error creating depth stencil texture.", hr);
        return false;
    }

    D3D11_TEXTURE2D_DESC backBufferDesc;
    backBuffer->GetDesc(&backBufferDesc);

    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    ZeroMemory(&renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
    renderTargetViewDesc.Format = backBufferDesc.Format;
    if (backBufferDesc.SampleDesc.Count > 1)
        renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
    else
        renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;
    hr = m_device->CreateRenderTargetView(backBuffer, &renderTargetViewDesc, &m_renderTargetView);
    backBuffer->Release();
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Error creating render target view.", hr);
        return false;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    depthStencilViewDesc.Format = depthStencilDesc.Format;
    if (backBufferDesc.SampleDesc.Count > 1)
        depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    else
        depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    hr = m_device->CreateDepthStencilView(m_depthTexture, &depthStencilViewDesc, &m_depthStencilView);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Error creating depth stencil view.", hr);
        return false;
    }

    ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));
    m_viewport.TopLeftX = 0;
    m_viewport.TopLeftY = 0;
    m_viewport.Width = (FLOAT)m_supportedResolution[config.resolution].Width;
    m_viewport.Height = (FLOAT)m_supportedResolution[config.resolution].Height;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;

    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    hr = m_device->CreateBlendState(&blendDesc, &m_blending);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create blend state.", hr);
        return false;
    }

    if (!InitScene()) {
        DXUT_ERR_MSGBOX("Error initializing scene.", hr);
        delete m_font;
        return false;
    }

    D3D11_RASTERIZER_DESC cmdesc;
    ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
    cmdesc.FillMode = D3D11_FILL_SOLID;
    cmdesc.CullMode = D3D11_CULL_BACK;
    cmdesc.FrontCounterClockwise = FALSE;
    cmdesc.DepthBias = 0;
    cmdesc.DepthBiasClamp = 0.0f;
    cmdesc.SlopeScaledDepthBias = 0.0f;
    cmdesc.DepthClipEnable = TRUE;
    cmdesc.ScissorEnable = FALSE;
    cmdesc.MultisampleEnable = FALSE;
    cmdesc.AntialiasedLineEnable = FALSE;
    hr = m_device->CreateRasterizerState(&cmdesc, &m_cWcullMode);
    if (FAILED(hr))
        DXUT_ERR_MSGBOX("Failed to create rasterizer.", hr);

    m_meshShader = new Shader();
    hr = m_meshShader->LoadVertexShader(m_device, "VS", "shaders\\mesh.fx");
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Error loading vertex shader.", hr);
    hr = m_meshShader->LoadPixelShader(m_device, "PS", "shaders\\mesh.fx");
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Error loading pixel shader.", hr);

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    LPVOID buffPtr = m_meshShader->getVertexBlob()->GetBufferPointer();
    SIZE_T size = m_meshShader->getVertexBlob()->GetBufferSize();
    hr = m_device->CreateInputLayout(layout, numElements, buffPtr, size, &m_layout);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create input layout.", hr);
    }

    return true;
}

bool Engine::InitScene() {
    setFullScreen(m_windowDesc->hWnd, config.fullscreen);
    m_debugRaycast = config.debugRaycast;
    camera.SetProjection();

    m_viewProjectionData = new ViewProjectonData(camera.getView(), camera.getProjection());

    m_font = new Font();
    m_font->Init(m_device);

    shadowMap.Init(m_device);
    m_location = new Location(m_device);

    gizmozRect.Init(m_device, m_deviceContext);
#ifdef INTERNAL_ENGINE_GUI_INTERFACE
    m_gui = new ImGUIDevice();
    m_gui->Init(m_device, m_deviceContext);
    CreateRenderTexture(m_supportedResolution[config.resolution].Width, m_supportedResolution[config.resolution].Height);
#endif

    return true;
}

void Engine::UpdateInput(float deltaTime) {
    DIMOUSESTATE mouseCurrState{};
    BYTE keyboardState[256]{};

    HRESULT hr = m_keyboard->GetDeviceState(sizeof(BYTE) * 256, (LPVOID)&keyboardState);
    if (FAILED(hr))
        m_keyboard->Acquire();
    hr = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);
    if (FAILED(hr))
        m_mouse->Acquire();

    if (keyboardState[DIK_ESCAPE] & 0x80)
        PostMessage(m_windowDesc->hWnd, WM_DESTROY, 0, 0);

#define LSHIFTSPEED (keyboardState[DIK_LSHIFT] & 0x80) ? config.additionalLShiftSpeed : 1.0f
    const float speed = config.cameraSpeed * (LSHIFTSPEED) * deltaTime;
    const float intensivity = (config.mouseIntensive / 100.0f) * deltaTime;
#undef LSHIFTSPEED

    if (keyboardState[DIK_A] & 0x80)
        camera.verticalLeftRight -= speed;
    if (keyboardState[DIK_D] & 0x80)
        camera.verticalLeftRight += speed;
    if (keyboardState[DIK_W] & 0x80)
        camera.horizontalBackForward += speed;
    if (keyboardState[DIK_S] & 0x80)
        camera.horizontalBackForward -= speed;
    if (mouseCurrState.rgbButtons[1] & 0x80) {
        camera.yaw += mouseCurrState.lX * intensivity;
        camera.pitch += mouseCurrState.lY * intensivity;
    }
}

void Engine::CreateRenderTexture(int width, int height) {
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    HRESULT hr = m_device->CreateTexture2D(&textureDesc, nullptr, &m_renderTexture);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create render texture.", hr);
    }

    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    ZeroMemory(&renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    hr = m_device->CreateRenderTargetView(m_renderTexture, &renderTargetViewDesc, &m_renderTextureRTV);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create RTV for render texture.", hr);
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    
    hr = m_device->CreateShaderResourceView(m_renderTexture, &srvDesc, &m_renderTextureSRV);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create SRV for render texture.", hr);
    }
}

Engine::Engine() {
    m_SwapChainOccluded = false;
}

void Engine::FixedUpdate(float deltaTime) {
    
}

void Engine::Update(float deltaTime) {
    camera.Update();
    m_location->Update(deltaTime);
    for (int iterator = 0; iterator < m_meshes.size(); ++iterator)
        m_meshes[iterator]->Update(deltaTime);
    m_location->m_skybox->Update(deltaTime);
    m_location->m_directionLight->Update(deltaTime);
}

void Engine::Render() {
    shadowMap.Render(m_deviceContext, m_location->m_directionLight);

    for (int iterator = 0; iterator < m_meshes.size(); ++iterator) {
        if (GameObject* obj = m_meshes[iterator]->gameObject()) {
            if (obj->isEnabled() && !obj->isTransparent())
                m_meshes[iterator]->RenderShadow(m_deviceContext, m_location->m_directionLight->viewProjection());
        }
    }

    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

    float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColor);
    m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
    m_deviceContext->OMSetRenderTargets(1, &m_renderTextureRTV, m_depthStencilView);
#endif

    m_deviceContext->RSSetViewports(1, &m_viewport);

    m_location->m_skybox->Render(m_deviceContext);
    m_deviceContext->OMSetDepthStencilState(nullptr, 0);
    m_location->m_directionLight->Render(m_deviceContext);

    m_deviceContext->IASetInputLayout(m_layout);
    m_meshShader->setVertexShader(m_deviceContext);
    m_meshShader->setPiexlShader(m_deviceContext);
    m_deviceContext->RSSetState(m_cWcullMode);
    m_deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
    // рендерим все непрозрачные объекты
    for (int iterator = 0; iterator < m_meshes.size(); ++iterator) {
        if (GameObject* obj = m_meshes[iterator]->gameObject()) {
            if (obj->isEnabled() && !obj->isTransparent())
                m_meshes[iterator]->Render(m_deviceContext, *m_viewProjectionData);
        }
    }

    // рендерим все прозрачные объекты (включая directionLight, skybox...)
    m_deviceContext->OMSetBlendState(m_blending, nullptr, 0xFFFFFFFF);
    for (int iterator = 0; iterator < m_meshes.size(); ++iterator) {
        if (GameObject* obj = m_meshes[iterator]->gameObject()) {
            if (obj->isEnabled() && obj->isTransparent())
                m_meshes[iterator]->Render(m_deviceContext, *m_viewProjectionData);
        }
    }

    gizmozRect.Render();

    //static wchar_t buffer[128];
    //swprintf_s(buffer, 128, L"(Internal Game Engine) DirectX 11 FPS: %d VSync: %s", m_timeInfo.fps, toStringVSync());
    //m_font->Render(m_deviceContext, buffer);

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
    m_gui->Render();
#endif

    HRESULT hr = m_swapChain->Present(min(config.vSync, 2), 0);
    m_SwapChainOccluded = hr == DXGI_STATUS_OCCLUDED;
}

void Engine::Release() {
    if (m_swapChain) m_swapChain->Release();
    if (m_device) m_device->Release();
    if (m_deviceContext) m_deviceContext->ClearState();
    if (m_renderTargetView) m_renderTargetView->Release();
    if (m_depthStencilView) m_depthStencilView->Release();
    if (m_depthTexture) m_depthTexture->Release();

    for (int iterator = 0; iterator < m_meshes.size(); ++iterator)
        m_meshes[iterator]->Release();
    m_meshes.clear();

    if (m_font) m_font->Release();
#ifdef INTERNAL_ENGINE_GUI_INTERFACE
    m_gui->Release();
#endif
    if (m_cWcullMode) m_cWcullMode->Release();
    if (m_meshShader) {
        m_meshShader->Release();
        delete m_meshShader;
    }
    if (m_layout) m_layout->Release();
    if (m_viewProjectionData) delete m_viewProjectionData;
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
            if (m_SwapChainOccluded && m_swapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) {
                Sleep(10);
                continue;
            }

            UpdateFrequenceTime(m_timeInfo);

            while (m_timeInfo.accumulator >= m_timeInfo.targetFrameTime) {
                FixedUpdate(m_timeInfo.targetFrameTime);
                m_timeInfo.accumulator -= m_timeInfo.targetFrameTime;
            }

            UpdateInput(m_timeInfo.deltaTime);
            Update(m_timeInfo.deltaTime);
            m_SwapChainOccluded = false;
            Render();
        }
    }

    return message.wParam;
}

void Engine::Raycast(int mouseX, int mouseY) {
#ifndef INTERNAL_ENGINE_GUI_INTERFACE
    int screenWidth = getSupportedResolution().Width;
    int screenHeight = getSupportedResolution().Height;
#else
    int screenWidth = m_sceneSize.x;
    int screenHeight = m_sceneSize.y;
#endif

    XMVECTOR pickRayInViewSpacePos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
    mouseX = mouseX - static_cast<int>(m_scenePos.x);
    mouseY = mouseY - static_cast<int>(m_scenePos.y);
#endif

#ifndef INTERNAL_ENGINE_GUI_INTERFACE
    if (config.fullscreen) {
        mouseX = (int)((float)mouseX * (float)screenWidth / (float)GetSystemMetrics(SM_CXSCREEN));
        mouseY = (int)((float)mouseY * (float)screenHeight / (float)GetSystemMetrics(SM_CYSCREEN));
    }
#endif

    float pointX = (((2.0f * (float)mouseX) / screenWidth) - 1) / camera.getProjection()(0, 0);
    float pointY = -(((2.0f * (float)mouseY) / screenHeight) - 1.0f) / camera.getProjection()(1, 1);
    float pointZ = 1.0f;

    XMVECTOR pickRayInViewSpaceDir = XMVectorSet(pointX, pointY, pointZ, 1.0f);

    XMMATRIX pickRayToWorldSpaceMatrix;
    XMVECTOR matInvDeter;
    pickRayToWorldSpaceMatrix = XMMatrixInverse(&matInvDeter, camera.getView());

    XMVECTOR pickRayInWorldSpacePos = XMVector3TransformCoord(pickRayInViewSpacePos, pickRayToWorldSpaceMatrix);
    XMVECTOR pickRayInWorldSpaceDir = XMVector3TransformNormal(pickRayInViewSpaceDir, pickRayToWorldSpaceMatrix);
    pickRayInWorldSpaceDir = XMVector3Normalize(pickRayInWorldSpaceDir);
    
#ifdef INTERNAL_ENGINE_DEBUG_RAYCAST
    if (m_debugRaycast) {
        XMFLOAT3 startPosFloat3;
        XMStoreFloat3(&startPosFloat3, pickRayInWorldSpacePos);

        XMFLOAT3 newEndPosFloat3;
        XMStoreFloat3(&newEndPosFloat3, pickRayInWorldSpacePos + pickRayInWorldSpaceDir * 100.0f);
        gizmozRect.DrawLine(startPosFloat3, newEndPosFloat3, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
    }
#endif

    float closestDistance = FLT_MAX;
    Model* closestMesh = nullptr;

    std::vector<Model*>::iterator it = m_meshes.begin();
    while (it != m_meshes.end()) {
        float distance = 0.0f;
        if (distance = (*it)->IntersectRayWithMesh(pickRayInWorldSpacePos, pickRayInWorldSpaceDir, (*it)->mesh())) {
            if (distance < closestDistance) {
                closestDistance = distance;
                closestMesh = *it;
            }
        }
        ++it;
    }

    if (closestMesh != nullptr) {
        GameObject* obj = closestMesh->gameObject();
        if (lastSelected)
            lastSelected->selectable = false;
        obj->selectable = true;
        lastSelected = obj;
    }
}

void Engine::addMeshRenderer(Model* mesh) {
    if (mesh) m_meshes.emplace_back(mesh);
}

void Engine::setFullScreen(HWND hwnd, bool fullscreen) {
    static WINDOWPLACEMENT prevPlacement = { sizeof(WINDOWPLACEMENT) };
    static RECT prevRect = { 0 };
    DWORD style = GetWindowLong(hwnd, GWL_STYLE);
    MONITORINFO mi = { sizeof(MONITORINFO) };

    if (!fullscreen) {
        SetWindowLong(hwnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(hwnd, &prevPlacement);
        SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
    else {
        GetWindowPlacement(hwnd, &prevPlacement);
        GetWindowRect(hwnd, &prevRect);

        if (GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &mi)) {
            SetWindowLong(hwnd, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN);

            SetWindowPos(hwnd, HWND_TOP,
                mi.rcMonitor.left, mi.rcMonitor.top,
                mi.rcMonitor.right - mi.rcMonitor.left,
                mi.rcMonitor.bottom - mi.rcMonitor.top,
                SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    }
    //engine.getChain()->SetFullscreenState(fullscreen, NULL);
}

const WindowDescription* Engine::getWindowDesc() const {
    return m_windowDesc;
}

IDXGISwapChain* Engine::getChain() const {
    return m_swapChain;
}

const DXGI_MODE_DESC& Engine::getSupportedResolution() const {
    return m_supportedResolution[config.resolution];
}

const wchar_t* Engine::toStringVSync() const {
    switch (config.vSync) {
    case 0: return L"Off";
    case 1: return L"Double buffering";
    case 2: return L"Triple Buffering";
    }
}

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

LRESULT Engine::windowProcessor(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
#ifdef INTERNAL_ENGINE_GUI_INTERFACE
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
#endif

    switch (msg) {
    case WM_CREATE: {
        LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
    } break;
    case WM_KEYDOWN: {
        if (wParam == VK_ESCAPE) {
            DestroyWindow(hWnd);
        }
    } break;
    case WM_LBUTTONDOWN: {
#ifdef INTERNAL_ENGINE_GUI_INTERFACE
        if (engine.getRaycast()) {
            int mouseX = GET_X_LPARAM(lParam);
            int mouseY = GET_Y_LPARAM(lParam);
            engine.Raycast(mouseX, mouseY);
        }
#endif
    } break;
    case WM_DESTROY: {
        PostQuitMessage(0);
        break;
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