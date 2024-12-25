#include "Engine.h"
#include "debug.h"
#include "Shader.h"
#include "MeshComponent.h"
#include "Font.h"
#include "Component.h"
#include "GameObject.h"
#include "Camera.h"
#include "Config.h"
#include "Location.h"
#include "Skybox.h"
#include "PrimitiveDrawable.h"

Engine engine;
Camera camera;
Config config;
PrimitiveDrawable draw;

bool Engine::InitWindowDevice(const WindowDescription* desc) {
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

    m_windowDesc->windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_SIZEBOX);

    m_windowDesc->hWnd = CreateWindowEx(NULL, m_windowDesc->title, m_windowDesc->title,
        m_windowDesc->windowStyle, CW_USEDEFAULT, CW_USEDEFAULT, m_supportedResolution[config.resolution].Width,
        m_supportedResolution[config.resolution].Height, NULL, NULL, m_windowDesc->hInstance, NULL);

    ShowWindow(m_windowDesc->hWnd, m_windowDesc->nCmdShow);
    UpdateWindow(m_windowDesc->hWnd);

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
    hr = m_mouse->SetCooperativeLevel(m_windowDesc->hWnd, config.showCursor ? DISCL_NONEXCLUSIVE : DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
    hr = m_mouse->Acquire();

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
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = m_windowDesc->hWnd;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

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

    hr = m_device->CreateRenderTargetView(backBuffer, NULL, &m_renderTargetView);
    backBuffer->Release();
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Error creating render target view.", hr);
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

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    depthStencilViewDesc.Format = depthStencilDesc.Format;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    hr = m_device->CreateDepthStencilView(m_depthTexture, &depthStencilViewDesc, &m_depthStencilView);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Error creating depth stencil view.", hr);
        return false;
    }

    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

    IDXGIFactory1* dxgiFactory = nullptr;
    hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&dxgiFactory);
    dxgiFactory->MakeWindowAssociation(m_windowDesc->hWnd, DXGI_MWA_NO_ALT_ENTER);

    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (FLOAT)m_supportedResolution[config.resolution].Width;
    viewport.Height = (FLOAT)m_supportedResolution[config.resolution].Height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_deviceContext->RSSetViewports(1, &viewport);

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
    rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    blendDesc.AlphaToCoverageEnable = true;
    blendDesc.RenderTarget[0] = rtbd;

    hr = m_device->CreateBlendState(&blendDesc, &m_transparency);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create blend state.", hr);
        return false;
    }

    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(BufferDirectionLight);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    hr = m_device->CreateBuffer(&bufferDesc, NULL, &m_constantLightBuffer);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create buffer.", hr);
        return false;
    }

    if (!InitScene()) {
        DXUT_ERR_MSGBOX("Error initializing scene.", hr);
        delete m_font;
        return false;
    }
    return true;
}

bool Engine::InitScene() {
    setFullScreen(m_windowDesc->hWnd, config.fullscreen);
    camera.SetProjection();

    m_font = new Font();
    m_font->Init(m_device);
    m_location = new Location();

    m_skybox = new Skybox();
    m_skybox->Init(m_device);

    draw.Init(m_device, m_deviceContext);

    return true;
}

void Engine::UpdateInput(float deltaTime) {
    DIMOUSESTATE mouseCurrState{};
    BYTE keyboardState[256]{};

    HRESULT hr = m_keyboard->GetDeviceState(sizeof(BYTE) * 256, (LPVOID)&keyboardState);
    if (FAILED(hr))
        m_keyboard->Acquire();
    m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

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

Engine::Engine() {
    m_SwapChainOccluded = false;
}

void Engine::FixedUpdate(float deltaTime) {
    
}

void Engine::Update(float deltaTime) {
    camera.Update();
    m_location->Update(deltaTime);
    for (int iterator = 0; iterator < m_meshes.size(); ++iterator) {
        m_meshes[iterator]->Update(deltaTime);
    }
    m_skybox->Update(deltaTime);
}

void Engine::Render() {
    D3DXCOLOR color(0.0f, 0.0f, 0.0f, 1.0f);
    
    m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
    m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    
    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    m_deviceContext->OMSetBlendState(m_transparency, blendFactor, 0xffffffff);
    
    for (int iterator = 0; iterator < m_meshes.size(); ++iterator) {
        m_bufferLight.direction = XMFLOAT4(-15.0f, -15.0f, 1.0f, 1.0f);
        m_bufferLight.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
        m_bufferLight.diffuse = XMFLOAT4(1.0f, 0.8f, 0.8f, 1.0f);
        m_deviceContext->UpdateSubresource(m_constantLightBuffer, 0, nullptr, &m_bufferLight, 0, 0);
        m_deviceContext->PSSetConstantBuffers(0, 1, &m_constantLightBuffer);
        m_meshes[iterator]->Render(m_deviceContext);
    }

    m_skybox->Render(m_deviceContext);
    draw.Render();
    
    wchar_t buffer[128];
    swprintf_s(buffer, 128, L"(Internal Game Engine) DirectX 11 FPS: %d VSync: %s", m_timeInfo.fps, toStringVSync());
    m_font->Render(m_deviceContext, buffer);
    
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
    if (m_skybox) {
        m_skybox->Release();
        delete m_skybox;
    }
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

            UpdateInput(m_timeInfo.deltaTime);
            Update(m_timeInfo.deltaTime);
            if (m_SwapChainOccluded && m_swapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) {
                Sleep(10);
                continue;
            }
            m_SwapChainOccluded = false;
            Render();
        }
    }

    return message.wParam;
}

void Engine::Raycast() {
    int screenWidth = getSupportedResolution().Width;
    int screenHeight = getSupportedResolution().Height;

    XMVECTOR pickRayInViewSpacePos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

    POINT cursorPos;
    GetCursorPos(&cursorPos);

    if (config.fullscreen) {
        cursorPos.x = (int)((float)cursorPos.x * (float)screenWidth / (float)GetSystemMetrics(SM_CXSCREEN));
        cursorPos.y = (int)((float)cursorPos.y * (float)screenHeight / (float)GetSystemMetrics(SM_CYSCREEN));
    }

    float pointX = (((2.0f * (float)cursorPos.x) / screenWidth) - 1) / camera.getProjection()(0, 0);
    float pointY = -(((2.0f * (float)cursorPos.y) / screenHeight) - 1.0f) / camera.getProjection()(1, 1);
    float pointZ = 1.0f;

    XMVECTOR pickRayInViewSpaceDir = XMVectorSet(pointX, pointY, pointZ, 1.0f);

    XMMATRIX pickRayToWorldSpaceMatrix;
    XMVECTOR matInvDeter;
    pickRayToWorldSpaceMatrix = XMMatrixInverse(&matInvDeter, camera.getView());

    XMVECTOR pickRayInWorldSpacePos = XMVector3TransformCoord(pickRayInViewSpacePos, pickRayToWorldSpaceMatrix);
    XMVECTOR pickRayInWorldSpaceDir = XMVector3TransformNormal(pickRayInViewSpaceDir, pickRayToWorldSpaceMatrix);
    pickRayInWorldSpaceDir = XMVector3Normalize(pickRayInWorldSpaceDir);

    float closestDistance = FLT_MAX;
    MeshComponent* closestMesh = nullptr;

    std::vector<MeshComponent*>::iterator it = m_meshes.begin();
    while (it != m_meshes.end()) {
        float distance = 0.0f;
        if (distance = (*it)->IntersectRayWithMesh(pickRayInWorldSpacePos, pickRayInWorldSpaceDir, *it)) {
            if (distance < closestDistance) {
                closestDistance = distance;
                closestMesh = *it;
            }
        }
        ++it;
    }

    if (closestMesh != nullptr) {
        GameObject* obj = closestMesh->gameObject();
        MeshComponent* mesh = obj->GetComponentByType<MeshComponent>();
        if (lastSelected)
            lastSelected->setSelectable(false);
        mesh->setSelectable(true);
        lastSelected = mesh;
    }
}

void Engine::addMeshRenderer(MeshComponent* mesh) {
    if (mesh) m_meshes.emplace_back(mesh);
}

void Engine::setFullScreen(HWND hwnd, bool fullscreen) {
    if (!fullscreen) {
        RECT windowRect;
        GetWindowRect(hwnd, &windowRect);
        engine.getWindowRect() = windowRect;

        SetWindowLongPtr(hwnd, GWL_STYLE, m_windowDesc->windowStyle);
        SetWindowPos(hwnd, HWND_TOP, 0, 0,
            m_supportedResolution[config.resolution].Width,
            m_supportedResolution[config.resolution].Height,
            SWP_FRAMECHANGED | SWP_NOACTIVATE);
        ShowWindow(hwnd, SW_NORMAL);
    }
    else {
        SetWindowLongPtr(hwnd, GWL_STYLE, m_windowDesc->windowStyle & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));
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

LRESULT Engine::windowProcessor(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
        } return 0;
        case WM_KEYDOWN: {
            if (wParam == VK_ESCAPE) {
                DestroyWindow(hWnd);
            }
            else if (wParam == VK_SPACE) {
                config.fullscreen = !config.fullscreen;
                engine.setFullScreen(hWnd, config.fullscreen);
            }
        } return 0;
        case WM_LBUTTONDOWN: {
            engine.Raycast();
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