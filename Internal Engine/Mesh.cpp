#include "Mesh.h"
#include "debug.h"
#include "Camera.h"
#include "Material.h"
#include "Shader.h"

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
    Position = XMMatrixIdentity();
    m_cWcullMode = nullptr;
    m_material = nullptr;
    m_meshShader = nullptr;
    m_layout = nullptr;
}

void Mesh::Update(float deltaTime) {
    
}

void Mesh::Render(ID3D11DeviceContext* context) {
    IASetVertexAndIndexBuffer(context);
    m_meshShader->setVertexShader(context);
    m_meshShader->setPiexlShader(context);
    context->IASetInputLayout(m_layout);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    engine.cbPerObj.WVP = XMMatrixTranspose(Position * camera.getView() * camera.getProjection());
    context->UpdateSubresource(::engine.m_preObjectBuffer, 0, NULL, &::engine.cbPerObj, 0, 0);
    context->VSSetConstantBuffers(0, 1, &::engine.m_preObjectBuffer);
    m_material->Bind(context);
    context->PSSetSamplers(0, 1, &::engine.m_textureSamplerState);
    context->DrawIndexed(36, 0, 0);
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

    m_material = new MeshMaterial();
    m_material->diffuseTex = new Material::TextureMapInfo();
    m_material->diffuseTex->name = "box.jpg";
    m_material->Load(device);

    m_meshShader = new Shader();
    if (FAILED(m_meshShader->LoadVertexShader(device, context, "shaders\\mesh.fx")))
        return hr;
    if (FAILED(m_meshShader->LoadPixelShader(device, context, "shaders\\mesh.fx")))
        return hr;

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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
}
