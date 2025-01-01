#include "Skybox.h"
#include "Shader.h"
#include "debug.h"
#include "Camera.h"
#include "Engine.h"

Skybox::Skybox(MeshComponent& component) : m_component(component) {
    m_cullMode = nullptr;
    m_shader = nullptr;
    m_preObjectBuffer = nullptr;
    m_depthState = nullptr;
    m_layout = nullptr;
}

HRESULT Skybox::Init(ID3D11Device* device) {
	HRESULT hr{};
    m_shader = new Shader();
    hr = m_shader->LoadVertexShader(device, "VS", "shaders\\skybox.fx");
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Failed to load Vertex shader.", hr);
    hr = m_shader->LoadPixelShader(device, "PS", "shaders\\skybox.fx");
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Failed to load Pixel shader.", hr);

    D3D11_RASTERIZER_DESC cmdesc;
    ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
    cmdesc.FillMode = D3D11_FILL_SOLID;
    cmdesc.CullMode = D3D11_CULL_NONE;
    cmdesc.FrontCounterClockwise = false;
    hr = device->CreateRasterizerState(&cmdesc, &m_cullMode);

    D3D11_DEPTH_STENCIL_DESC depthDesc;
    ZeroMemory(&depthDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
    depthDesc.DepthEnable = TRUE;
    depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

    hr = device->CreateDepthStencilState(&depthDesc, &m_depthState);
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Error create depth stencil state.", hr);

    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(WorldViewProjection);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;

    hr = device->CreateBuffer(&bufferDesc, NULL, &m_preObjectBuffer);
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Failed to create buffer.", hr);

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    UINT numElements = ARRAYSIZE(layout);

    LPVOID buffPtr = m_shader->getVertexBlob()->GetBufferPointer();
    SIZE_T size = m_shader->getVertexBlob()->GetBufferSize();
    hr = device->CreateInputLayout(layout, numElements, buffPtr, size, &m_layout);
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Failed to create input layout.", hr);
    hr = m_component.Init(device);
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Failed to Init MeshComponent by Skybox.", hr);
    m_device_loader = true;
    return hr;
}

void Skybox::Update(float deltaTime) {
    if (!m_device_loader) return;
    m_pos = XMMatrixIdentity();
    XMMATRIX scale = XMMatrixScaling(5.0f, 0.5f, 5.0f);
    XMMATRIX translate = XMMatrixTranslation(
        XMVectorGetX(camera.getPos()),
        XMVectorGetY(camera.getPos()),
        XMVectorGetZ(camera.getPos()));
    m_pos = scale * translate;
}

void Skybox::Render(ID3D11DeviceContext* context) {
    if (!m_device_loader) return;
    m_wvp.WVP = m_pos * camera.getView() * camera.getProjection();
    m_wvp.WVP = XMMatrixTranspose(m_wvp.WVP);
    m_shader->setVertexShader(context);
    m_shader->setPiexlShader(context);
    context->IASetInputLayout(m_layout);
    context->UpdateSubresource(m_preObjectBuffer, 0, NULL, &m_wvp, 0, 0);
    context->VSSetConstantBuffers(0, 1, &m_preObjectBuffer);
    context->OMSetDepthStencilState(m_depthState, 0);
    context->RSSetState(m_cullMode);
    m_component.Render(context);
}

bool Skybox::CreateVertex(ID3D11Device* device, const std::vector<Vertex>& vertices, uint32_t sizeType, uint32_t size) {
    return m_component.CreateVertex(device, vertices, sizeType, size);
}

bool Skybox::CreateIndex(ID3D11Device* device, const std::vector<DWORD>& indices, uint32_t sizeType, uint32_t size) {
    return m_component.CreateIndex(device, indices, sizeType, size);
}

void Skybox::setMaterial(const char* name, XMFLOAT2 scale, XMFLOAT2 offset) {
    m_component.setMaterial(name, scale, offset);
}

void Skybox::Release() {
    if (m_cullMode) m_cullMode->Release();
    if (m_shader) {
        m_shader->Release();
        delete m_shader;
    }
    if (m_preObjectBuffer) m_preObjectBuffer->Release();
    if (m_depthState) m_depthState->Release();
    if (m_layout) m_layout->Release();
}
