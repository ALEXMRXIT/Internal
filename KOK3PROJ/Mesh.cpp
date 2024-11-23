#include "Mesh.h"
#include "debug.h"

VertexBuffer::VertexBuffer() {
	m_vertexBuffer = nullptr;
}

bool VertexBuffer::Create(ID3D11Device* device, void* pBuffer, unsigned int size) {
	CHECK_ASSERT(m_vertexBuffer == NULL, "VertexBuffer already created!");

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
        ERROR_MSG("Failed to create vertex buffer. %d error code.", handleResult);
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
    CHECK_ASSERT(m_indexBuffer == NULL, "IndexBuffer already created!");

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
        ERROR_MSG("Failed to create index buffer. %d error code.", handleResult);
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
}

void Mesh::Render(MeshRenderData* renderData) {
    MeshRenderData& data = *renderData;
    rot += .001f;
    if (rot > 6.26f)
        rot = 0.0f;

    data.m_transformMatrix = XMMatrixIdentity();
    XMVECTOR rotaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    Rotation = XMMatrixRotationAxis(rotaxis, rot);
    Translation = XMMatrixTranslation(0.0f, 0.0f, 4.0f);
    data.m_transformMatrix = Translation * Rotation;
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
}
