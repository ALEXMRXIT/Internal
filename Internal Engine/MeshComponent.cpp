#include "MeshComponent.h"
#include "debug.h"
#include "Camera.h"
#include "Material.h"
#include "Shader.h"

VertexBuffer::VertexBuffer() {
	m_vertexBuffer = nullptr;
}

bool VertexBuffer::Create(ID3D11Device* device, void* pBuffer, uint32_t sizeType, uint32_t size) {
    HRESULT handleResult{};
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeType * size;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vertexBufferData;
    ZeroMemory(&vertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
    vertexBufferData.pSysMem = pBuffer;
    handleResult = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_vertexBuffer);
    if (FAILED(handleResult)) {
        DXUT_ERR_MSGBOX("Failed to create vertex buffer.", handleResult);
        return false;
    }
	
	return true;
}

void VertexBuffer::Release() {
	if (m_vertexBuffer) m_vertexBuffer->Release();
}

IndexBuffer::IndexBuffer() {
	m_indexBuffer = nullptr;
}

bool IndexBuffer::Create(ID3D11Device* device, void* pBuffer, uint32_t sizeType, uint32_t size) {
    HRESULT handleResult{};
    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeType * size;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA indexBufferData;
    ZeroMemory(&indexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
    indexBufferData.pSysMem = pBuffer;
    handleResult = device->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_indexBuffer);
    if (FAILED(handleResult)) {
        DXUT_ERR_MSGBOX("Failed to create index buffer.", handleResult);
        return false;
    }
    
    return true;
}

void IndexBuffer::Release() {
	if (m_indexBuffer) m_indexBuffer->Release();
}

MeshComponent::MeshComponent() {
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;
    m_cWcullMode = nullptr;
    m_material = nullptr;
    m_meshShader = nullptr;
    m_layout = nullptr;
    m_preObjectBuffer = nullptr;
    indicses = 0;
}

void MeshComponent::Update(float deltaTime) {
    
}

void MeshComponent::Render(ID3D11DeviceContext* context) {
    IASetVertexAndIndexBuffer(context);
    m_meshShader->setVertexShader(context);
    m_meshShader->setPiexlShader(context);
    context->IASetInputLayout(m_layout);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_bufferWVP.World = XMMatrixTranspose(*m_position);
    m_bufferWVP.WVP = XMMatrixTranspose(*m_position * camera.getView() * camera.getProjection());
    m_bufferWVP.texture_scale = m_material->scale();
    m_bufferWVP.texture_offset = m_material->offset();
    context->UpdateSubresource(m_preObjectBuffer, 0, NULL, &m_bufferWVP, 0, 0);
    context->VSSetConstantBuffers(0, 1, &m_preObjectBuffer);
    m_material->Bind(context);
    context->RSSetState(m_cWcullMode);
    context->DrawIndexed(indicses, 0, 0);
}

void MeshComponent::setMatrix(XMMATRIX& position) {
    m_position = &position;
}

void MeshComponent::setMaterial(const char* name, XMFLOAT2 scale, XMFLOAT2 offset){
    m_material = new MeshMaterial();
    m_material->diffuseTex = new Material::TextureMapInfo();
    m_material->diffuseTex->name = name;
    m_material->SetScale(scale, offset);
}

HRESULT MeshComponent::Init(ID3D11Device* device, ID3D11DeviceContext* context) {
    HRESULT hr;
    D3D11_RASTERIZER_DESC cmdesc;
    ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
    cmdesc.FillMode = D3D11_FILL_SOLID;
    cmdesc.CullMode = D3D11_CULL_BACK;
    cmdesc.MultisampleEnable = true;
    cmdesc.FrontCounterClockwise = false;
    cmdesc.DepthClipEnable = true;
    hr = device->CreateRasterizerState(&cmdesc, &m_cWcullMode);

    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(WorldViewProjection);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    hr = device->CreateBuffer(&bufferDesc, NULL, &m_preObjectBuffer);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create buffer.", hr);
        return hr;
    }

    if (!m_material) {
        m_material = new MeshMaterial();
        m_material->diffuseTex = new Material::TextureMapInfo();
        m_material->SetScale(XMFLOAT2(1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f));
    }
    else if (m_material->diffuseTex && m_material->diffuseTex->name) {
        m_material->Load(device);
    }

    m_meshShader = new Shader();
    if (FAILED(m_meshShader->LoadVertexShader(device, context, "shaders\\mesh.fx")))
        return hr;
    if (FAILED(m_meshShader->LoadPixelShader(device, context, "shaders\\mesh.fx")))
        return hr;

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    UINT numElements = ARRAYSIZE(layout);

    LPVOID buffPtr = m_meshShader->getVertexBlob()->GetBufferPointer();
    SIZE_T size = m_meshShader->getVertexBlob()->GetBufferSize();
    hr = device->CreateInputLayout(layout, numElements, buffPtr, size, &m_layout);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create input layout.", hr);
        return hr;
    }

    return hr;
}

bool MeshComponent::CreateVertex(ID3D11Device* device, void* pBuffer, uint32_t sizeType, uint32_t size) {
    if (m_vertexBuffer = new VertexBuffer())
        return m_vertexBuffer->Create(device, pBuffer, sizeType, size);
    return false;
}

bool MeshComponent::CreateIndex(ID3D11Device* device, void* pBuffer, uint32_t sizeType, uint32_t size) {
    indicses = size;
    if (m_indexBuffer = new IndexBuffer())
        return m_indexBuffer->Create(device, pBuffer, sizeType, size);
    return false;
}

void MeshComponent::IASetVertexAndIndexBuffer(ID3D11DeviceContext* context) {
    context->IASetIndexBuffer(*m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    ID3D11Buffer* vertex = *m_vertexBuffer;
    context->IASetVertexBuffers(0, 1, &vertex, &stride, &offset);
}

void MeshComponent::Release() {
    if (m_vertexBuffer) {
        m_vertexBuffer->Release();
        delete m_vertexBuffer;
    }
    if (m_indexBuffer) {
        m_indexBuffer->Release();
        delete m_indexBuffer;
    }
    if (m_cWcullMode) m_cWcullMode->Release();
    if (m_material) {
        m_material->Release();
        delete m_material;
    }
    if (m_meshShader) {
        m_meshShader->Release();
        delete m_meshShader;
    }
    if (m_layout) m_layout->Release();
    if (m_preObjectBuffer) m_preObjectBuffer->Release();
}
