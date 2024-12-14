#include "Skybox.h"
#include "Shader.h"
#include "debug.h"
#include "Camera.h"
#include "Engine.h"

void Skybox::CreateSphere(ID3D11Device* device, int llines, int longlines) {
    HRESULT hr{};
    verticesNum = ((llines - 2) * longlines) + 2;
    indexesNum = ((llines - 3) * (longlines) * 2) + (longlines * 2);

    std::vector<Vertex> vertices(verticesNum);
    std::vector<DWORD> indices(indexesNum * 3);

    vertices[0].position = XMFLOAT3(0.0f, 0.0f, 1.0f);
    vertices[0].texCoord = XMFLOAT2(0.5f, 0.0f);

    int vertexIndex = 1;
    for (int i = 1; i < llines - 1; ++i) {
        float phi = i * XM_PI / (llines - 1);
        for (int j = 0; j < longlines; ++j) {
            float theta = j * XM_2PI / longlines;
            float x = sin(phi) * cos(theta);
            float y = cos(phi);
            float z = sin(phi) * sin(theta);
            vertices[vertexIndex].position = XMFLOAT3(x, y, z);
            vertices[vertexIndex].texCoord = XMFLOAT2((float)j / longlines, (float)i / (llines - 1));
            vertexIndex++;
        }
    }

    vertices[verticesNum - 1].position = XMFLOAT3(0.0f, 0.0f, -1.0f);
    vertices[verticesNum - 1].texCoord = XMFLOAT2(0.5f, 1.0f);

    int k = 0;
    for (int j = 0; j < longlines - 1; ++j) {
        indices[k++] = 0;
        indices[k++] = j + 1;
        indices[k++] = j + 2;
    }
    indices[k++] = 0;
    indices[k++] = longlines;
    indices[k++] = 1;

    for (int i = 0; i < llines - 3; ++i) {
        for (int j = 0; j < longlines - 1; ++j) {
            indices[k++] = i * longlines + j + 1;
            indices[k++] = i * longlines + j + 2;
            indices[k++] = (i + 1) * longlines + j + 1;

            indices[k++] = (i + 1) * longlines + j + 1;
            indices[k++] = i * longlines + j + 2;
            indices[k++] = (i + 1) * longlines + j + 2;
        }
        indices[k++] = (i * longlines) + longlines;
        indices[k++] = (i * longlines) + 1;
        indices[k++] = ((i + 1) * longlines) + longlines;

        indices[k++] = ((i + 1) * longlines) + longlines;
        indices[k++] = (i * longlines) + 1;
        indices[k++] = ((i + 1) * longlines) + 1;
    }

    for (int j = 0; j < longlines - 1; ++j) {
        indices[k++] = verticesNum - 1;
        indices[k++] = (verticesNum - 1) - (j + 1);
        indices[k++] = (verticesNum - 1) - (j + 2);
    }
    indices[k++] = verticesNum - 1;
    indices[k++] = (verticesNum - 1) - longlines;
    indices[k++] = verticesNum - 2;

    CreateVertex(device, vertices.data(), sizeof(Vertex), verticesNum);
    CreateIndex(device, indices.data(), sizeof(DWORD), indexesNum * 3);
}

Skybox::Skybox() {
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;
    m_cullMode = nullptr;
    m_shader = nullptr;
    m_preObjectBuffer = nullptr;
    m_sharedView = nullptr;
    m_depthState = nullptr;
    m_textureSamplerState = nullptr;
    verticesNum = 0;
    indexesNum = 0;
}

void Skybox::Init(ID3D11Device* device, ID3D11DeviceContext* context) {
	HRESULT hr{};
    CreateSphere(device, 10, 10);
    m_shader = new Shader();
    hr = m_shader->LoadVertexShader(device, context, "VS", "shaders\\skybox.fx");
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Failed to load Vertex shader.", hr);
    hr = m_shader->LoadPixelShader(device, context, "PS", "shaders\\skybox.fx");
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Failed to load Pixel shader.", hr);
    
    D3DX11_IMAGE_LOAD_INFO loadSMInfo;
    loadSMInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

    ID3D11Texture2D* texture = nullptr;
    hr = D3DX11CreateTextureFromFile(device, "mesh\\skymap.dds", &loadSMInfo, 0, (ID3D11Resource**)&texture, 0);
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Error loading skybox texture.", hr);

    D3D11_TEXTURE2D_DESC textureDesc;
    texture->GetDesc(&textureDesc);

    D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
    ZeroMemory(&viewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    viewDesc.Format = textureDesc.Format;
    viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    viewDesc.TextureCube.MipLevels = textureDesc.MipLevels;
    viewDesc.TextureCube.MostDetailedMip = 0;

    hr = device->CreateShaderResourceView(texture, &viewDesc, &m_sharedView);
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Error create shared resources view.", hr);

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

    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));
    if (config.qualityTexture == 0) sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    else if (config.qualityTexture == 10) sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    else sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    const int anisotropy = 1 + (config.qualityTexture * 2);
    sampDesc.MaxAnisotropy = min(anisotropy, 16);
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = device->CreateSamplerState(&sampDesc, &m_textureSamplerState);
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Failed to create Sampler state.", hr);
}

void Skybox::Update(float deltaTime) {
    m_pos = XMMatrixIdentity();
    XMMATRIX scale = XMMatrixScaling(5.0f, 5.0f, 5.0f);
    XMMATRIX translate = XMMatrixTranslation(
        XMVectorGetX(camera.getPos()),
        XMVectorGetY(camera.getPos()),
        XMVectorGetZ(camera.getPos()));
    m_pos = scale * translate;
}

void Skybox::Render(ID3D11DeviceContext* context) {
    IASetVertexAndIndexBuffer(context);
    m_wvp.WVP = m_pos * camera.getView() * camera.getProjection();
    m_wvp.WVP = XMMatrixTranspose(m_wvp.WVP);
    m_shader->setVertexShader(context);
    m_shader->setPiexlShader(context);
    context->UpdateSubresource(m_preObjectBuffer, 0, NULL, &m_wvp, 0, 0);
    context->VSSetConstantBuffers(0, 1, &m_preObjectBuffer);
    context->PSSetShaderResources(0, 1, &m_sharedView);
    context->PSSetSamplers(0, 1, &m_textureSamplerState);
    context->OMSetDepthStencilState(m_depthState, 0);
    context->RSSetState(m_cullMode);
    context->DrawIndexed(indexesNum * 3, 0, 0);
}

void Skybox::IASetVertexAndIndexBuffer(ID3D11DeviceContext* context) {
    context->IASetIndexBuffer(*m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    ID3D11Buffer* vertex = *m_vertexBuffer;
    context->IASetVertexBuffers(0, 1, &vertex, &stride, &offset);
}

bool Skybox::CreateVertex(ID3D11Device* device, void* pBuffer, uint32_t sizeType, uint32_t size) {
    if (m_vertexBuffer = new VertexBuffer())
        return m_vertexBuffer->Create(device, pBuffer, sizeType, size);
    return false;
}

bool Skybox::CreateIndex(ID3D11Device* device, void* pBuffer, uint32_t sizeType, uint32_t size) {
    if (m_indexBuffer = new IndexBuffer())
        return m_indexBuffer->Create(device, pBuffer, sizeType, size);
    return false;
}

void Skybox::Release() {
    if (m_vertexBuffer) {
        m_vertexBuffer->Release();
        delete m_vertexBuffer;
    }
    if (m_indexBuffer) {
        m_indexBuffer->Release();
        delete m_indexBuffer;
    }
    if (m_cullMode) m_cullMode->Release();
    if (m_shader) {
        m_shader->Release();
        delete m_shader;
    }
    if (m_preObjectBuffer) m_preObjectBuffer->Release();
    if (m_sharedView) m_sharedView->Release();
    if (m_depthState) m_depthState->Release();
    if (m_textureSamplerState) m_textureSamplerState->Release();
}
