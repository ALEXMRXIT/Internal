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
    indexBufferData.SysMemPitch = 0;
    indexBufferData.SysMemSlicePitch = 0;
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
    m_material = nullptr;
    m_preObjectBuffer = nullptr;
    m_indices = 0;
    m_position = nullptr;
    m_obj = nullptr;
    m_selectable = false;
    m_select.selectable = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
    alpha = 0.0f;
}

void MeshComponent::Update(float deltaTime) {
    if (alpha < 1.0f) {
        alpha += deltaTime * 5.0f;
        alpha = min(alpha, 1.0f);
    }
}

void MeshComponent::Render(ID3D11DeviceContext* context) {
    IASetVertexAndIndexBuffer(context);

    m_bufferWVP.WVP = XMMatrixTranspose(*m_position * camera.getView() * camera.getProjection());
    m_bufferWVP.World = XMMatrixTranspose(*m_position);
    m_bufferWVP.texture_scale = m_material->scale();
    m_bufferWVP.texture_offset = m_material->offset();

    context->UpdateSubresource(m_preObjectBuffer, 0, NULL, &m_bufferWVP, 0, 0);
    context->VSSetConstantBuffers(1, 1, &m_preObjectBuffer);

    m_select.selectable = XMFLOAT4((float)m_selectable, alpha, 0.0f, 0.0f);
    context->UpdateSubresource(m_preObjectSelect, 0, NULL, &m_select, 0, 0);
    context->PSSetConstantBuffers(2, 1, &m_preObjectSelect);

    m_material->Bind(context);
    context->DrawIndexed(m_indices, 0, 0);
}

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
void MeshComponent::UpdateInterfaceInInspector(GameObject* gameObject) {
    if (ImGui::CollapsingHeader("Mesh Renderer")) {

    }
}
#endif

void MeshComponent::setMatrix(XMMATRIX& position) {
    m_position = &position;
}

void MeshComponent::setMaterial(const char* name, XMFLOAT2 scale, XMFLOAT2 offset){
    m_material = new MeshMaterial();
    m_material->diffuseTex = new Material::TextureMapInfo();
    m_material->diffuseTex->name = name;
    m_material->SetScale(scale, offset);
}

HRESULT MeshComponent::Init(ID3D11Device* device) {
    HRESULT hr;

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

    D3D11_BUFFER_DESC bufferDescSelectable;
    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
    bufferDescSelectable.Usage = D3D11_USAGE_DEFAULT;
    bufferDescSelectable.ByteWidth = sizeof(SelectableConstantBuffer);
    bufferDescSelectable.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDescSelectable.CPUAccessFlags = 0;
    bufferDescSelectable.MiscFlags = 0;
    hr = device->CreateBuffer(&bufferDescSelectable, NULL, &m_preObjectSelect);
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

    return hr;
}

bool MeshComponent::CreateVertex(ID3D11Device* device, const std::vector<Vertex>& vertices, uint32_t sizeType, uint32_t size) {
    setVerticesPhysics(vertices);
    if (m_vertexBuffer = new VertexBuffer())
        return m_vertexBuffer->Create(device, (void*)vertices.data(), sizeType, size);
    return false;
}

bool MeshComponent::CreateIndex(ID3D11Device* device, const std::vector<DWORD>& indices, uint32_t sizeType, uint32_t size) {
    m_indices = size;
    setIndecesPhysics(indices);
    if (m_indexBuffer = new IndexBuffer())
        return m_indexBuffer->Create(device, (void*)indices.data(), sizeType, size);
    return false;
}

void MeshComponent::IASetVertexAndIndexBuffer(ID3D11DeviceContext* context) {
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    ID3D11Buffer* vertex = *m_vertexBuffer;
    context->IASetVertexBuffers(0, 1, &vertex, &stride, &offset);
    context->IASetIndexBuffer(*m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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
    if (m_material) {
        m_material->Release();
        delete m_material;
    }
    if (m_preObjectBuffer) m_preObjectBuffer->Release();
}
