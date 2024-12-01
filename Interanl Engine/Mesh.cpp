#include "Mesh.h"
#include "debug.h"
#include "Camera.h"

VertexBuffer::VertexBuffer() {
	m_vertexBuffer = nullptr;
}

bool VertexBuffer::Create(ID3D11Device* device, void* pBuffer, unsigned int size) {
    HRESULT handleResult{};
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * size;
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

bool IndexBuffer::Create(ID3D11Device* device, void* pBuffer, unsigned int size) {
    HRESULT handleResult{};
    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(DWORD) * size;
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

Mesh::Mesh() {
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;
    rot = 0.01f;
    Position = XMMatrixIdentity();
    m_sharedResourceView = nullptr;
    m_cWcullMode = nullptr;
}

void Mesh::Update(float deltaTime) {
    //rot += deltaTime * 1.0f;
    if (rot > 6.26f)
        rot = 0.0f;

    Position = XMMatrixIdentity();
    XMVECTOR rotaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    Rotation = XMMatrixRotationAxis(rotaxis, rot);
    Translation = XMMatrixTranslation(0.0f, 0.0f, 4.0f);
    Position = Translation * Rotation;
}

void Mesh::Render(ID3D11DeviceContext* context) {
    IASetVertexAndIndexBuffer(context);
    ::engine.cbPerObj.WVP = XMMatrixTranspose(Position * camera.getView() * ::engine.camProjection);
    context->UpdateSubresource(::engine.m_preObjectBuffer, 0, NULL, &::engine.cbPerObj, 0, 0);
    context->VSSetConstantBuffers(0, 1, &::engine.m_preObjectBuffer);
    context->PSSetShaderResources(0, 1, &m_sharedResourceView);
    context->PSSetSamplers(0, 1, &::engine.m_textureSamplerState);
    context->DrawIndexed(36, 0, 0);
}

HRESULT Mesh::LoadMaterial(ID3D11Device* device, const char* name) {
    return D3DX11CreateShaderResourceViewFromFile(device, name, NULL, NULL, &m_sharedResourceView, NULL);
}

HRESULT Mesh::Init(ID3D11Device* device, ID3D11DeviceContext* context) {
    HRESULT hr;
    D3D11_RASTERIZER_DESC cmdesc;
    ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
    cmdesc.FillMode = D3D11_FILL_SOLID;
    cmdesc.CullMode = D3D11_CULL_BACK;
    cmdesc.MultisampleEnable = true;
    cmdesc.FrontCounterClockwise = false;
    cmdesc.DepthClipEnable = true;
    hr = device->CreateRasterizerState(&cmdesc, &m_cWcullMode);
    context->RSSetState(m_cWcullMode);
    return hr;
}

bool Mesh::CreateVertex(ID3D11Device* device, void* pBuffer, unsigned int size) {
    if (m_vertexBuffer = new VertexBuffer())
        return m_vertexBuffer->Create(device, pBuffer, size);
    return false;
}

bool Mesh::CreateIndex(ID3D11Device* device, void* pBuffer, unsigned int size) {
    if (m_indexBuffer = new IndexBuffer())
        return m_indexBuffer->Create(device, pBuffer, size);
    return false;
}

void Mesh::IASetVertexAndIndexBuffer(ID3D11DeviceContext* context) {
    context->IASetIndexBuffer(*m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    ID3D11Buffer* vertex = *m_vertexBuffer;
    context->IASetVertexBuffers(0, 1, &vertex, &stride, &offset);
}

void Mesh::Release() {
    if (m_vertexBuffer) {
        m_vertexBuffer->Release();
        delete m_vertexBuffer;
    }
    if (m_indexBuffer) {
        m_indexBuffer->Release();
        delete m_indexBuffer;
    }
    if (m_sharedResourceView) m_sharedResourceView->Release();
    if (m_cWcullMode) m_cWcullMode->Release();
}
