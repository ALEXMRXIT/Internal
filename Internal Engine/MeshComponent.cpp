#include "MeshComponent.h"
#include "debug.h"
#include "Camera.h"
#include "Material.h"
#include "Shader.h"
#include "ShadowMap.h"

VertexBuffer::VertexBuffer() {
	m_vertexBuffer = nullptr;
}

bool VertexBuffer::Create(ID3D11Device* device, void* pBuffer, uint32_t sizeType, uint32_t size) {
    HRESULT handleResult{};
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
    vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
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
    indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
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

MeshComponent::MeshComponent(GameObject* obj) : AbstractBaseComponent(obj) {
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;
    m_preObjectBuffer = nullptr;
    m_indices = 0;
    model = nullptr;
    m_spawned = false;
    m_shadowConstantBuffer = nullptr;
}

void MeshComponent::Update(float deltaTime) {

}

static XMMATRIX GetInverseTransposeWorldMatrix(const XMMATRIX& worldMatrix) {
    XMMATRIX world3x3 = worldMatrix;
    world3x3.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

    XMMATRIX inverseWorld;
    XMVECTOR det;
    inverseWorld = XMMatrixInverse(&det, world3x3);
    XMMATRIX inverseTranspose = XMMatrixTranspose(inverseWorld);
    return inverseTranspose;
}

void MeshComponent::Render(ID3D11DeviceContext* context, DirectionLight* directionLight) {
    if (!m_device_loader) return;

    Transform* transform = gameObject().GetComponentByType<Transform>();
    MeshMaterial* material = gameObject().GetComponentByType<MeshMaterial>();

    XMMATRIX worldPosition = transform->GetWorldMatrix();
    m_bufferWVP.WVP = XMMatrixTranspose(worldPosition * camera.getView() * camera.getProjection());
    m_bufferWVP.World = XMMatrixTranspose(worldPosition);
    m_bufferWVP.ViewProjection = XMMatrixTranspose(camera.getView() * camera.getProjection());
    m_bufferWVP.InverseWorld = GetInverseTransposeWorldMatrix(worldPosition);
    m_bufferWVP.LightPos = XMMatrixTranspose(directionLight->GetViewProjectionMatrix());
    m_bufferWVP.texture_scale = material ? material->scale() : XMFLOAT2(1.0f, 1.0f);
    m_bufferWVP.texture_offset = material ? material->offset() : XMFLOAT2(0.0f, 0.0f);

    context->UpdateSubresource(m_preObjectBuffer, 0, nullptr, &m_bufferWVP, 0, 0);
    context->VSSetConstantBuffers(0, 1, &m_preObjectBuffer);

    IASetVertexAndIndexBuffer(context);
    if (material)
        material->Bind(context);
    context->DrawIndexed(m_indices, 0, 0);
}

void MeshComponent::RenderShadow(ID3D11DeviceContext* context, DirectionLight* directionLight) {
    XMMATRIX worldPosition = gameObject().GetComponentByType<Transform>()->GetWorldMatrix();
    if (!directionLight->gameObject().IsStatic())
        m_shadowCache = directionLight->UpdateMatrixByDirectionLight(worldPosition);

    context->UpdateSubresource(m_shadowConstantBuffer, 0, nullptr, &m_shadowCache, 0, 0);
    context->VSSetConstantBuffers(0, 1, &m_shadowConstantBuffer);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    ID3D11Buffer* vertex = *m_vertexBuffer;
    context->IASetVertexBuffers(0, 1, &vertex, &stride, &offset);
    context->IASetIndexBuffer(*m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    context->DrawIndexed(m_indices, 0, 0);
}

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
void MeshComponent::UpdateInterfaceInInspector(GameObject* gameObject) {
    ImGui::Dummy(ImVec2(0.0f, 2.0f));
    if (ImGui::CollapsingHeader("Mesh Renderer")) {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.25f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    
        ImGui::BeginChild("MeshRenderer", ImVec2(0, 120), true);
        {
            //MeshMaterial* material = (gameObject->GetComponentByType<MeshComponent>()->material());
            //if (material) {
            //    if (material->diffuseTex && material->diffuseTex->m_shaderView) {
            //        ImGui::Image((void*)material->diffuseTex->m_shaderView, ImVec2(100, 100));
            //        ImGui::SameLine();
            //
            //        ImGui::BeginGroup();
            //        {
            //            static char buffer[MAX_PATH];
            //            snprintf(buffer, MAX_PATH, "Name: %s", material->diffuseTex->name);
            //            ImGui::Text(buffer);
            //
            //            XMFLOAT2 tiling = material->scale();
            //            float til[2] = { tiling.x, tiling.y };
            //
            //            ImGui::Dummy(ImVec2(0.0f, 2.0f));
            //            ImGui::Text("Tiling");
            //            ImGui::SameLine();
            //            ImGui::Text("X:");
            //            ImGui::SameLine();
            //            ImGui::SetNextItemWidth(50.0f);
            //            if (ImGui::DragFloat("##TexX", &til[0], 0.1f))
            //                material->setScale(XMFLOAT2(til[0], til[1]));
            //
            //            ImGui::SameLine();
            //            ImGui::Text("Y:");
            //            ImGui::SameLine();
            //            ImGui::SetNextItemWidth(50.0f);
            //            if (ImGui::DragFloat("##TexY", &til[1], 0.1f))
            //                material->setScale(XMFLOAT2(til[0], til[1]));
            //
            //            XMFLOAT2 offset = material->offset();
            //            float off[2] = { offset.x, offset.y };
            //
            //            ImGui::Dummy(ImVec2(0.0f, 2.0f));
            //            ImGui::Text("Offset");
            //            ImGui::SameLine();
            //            ImGui::Text("X:");
            //            ImGui::SameLine();
            //            ImGui::SetNextItemWidth(50.0f);
            //            if (ImGui::DragFloat("##OffX", &off[0], 0.01f))
            //                material->setOffset(XMFLOAT2(off[0], off[1]));
            //
            //            ImGui::SameLine();
            //            ImGui::Text("Y:");
            //            ImGui::SameLine();
            //            ImGui::SetNextItemWidth(50.0f);
            //            if (ImGui::DragFloat("##OffY", &off[1], 0.01f))
            //                material->setOffset(XMFLOAT2(off[0], off[1]));
            //        }
            //        ImGui::EndGroup();
            //    }
            //}
        }
        ImGui::EndChild();
    
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar();
    }
}
#endif

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
    D3D11_BUFFER_DESC bufferShadowConstant;
    ZeroMemory(&bufferShadowConstant, sizeof(D3D11_BUFFER_DESC));
    bufferShadowConstant.Usage = D3D11_USAGE_DEFAULT;
    bufferShadowConstant.ByteWidth = sizeof(BufferDirectionLight);
    bufferShadowConstant.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferShadowConstant.CPUAccessFlags = 0;
    bufferShadowConstant.MiscFlags = 0;
    hr = device->CreateBuffer(&bufferShadowConstant, NULL, &m_shadowConstantBuffer);
    if (FAILED(hr))
        DXUT_ERR_MSGBOX("Failed to create matrix buffer.", hr);
    m_device_loader = true;
    return hr;
}

bool MeshComponent::CreateVertex(ID3D11Device* device, const std::vector<Vertex>& vertices, uint32_t sizeType, uint32_t size) {
    if (m_vertexBuffer = new VertexBuffer())
        return m_vertexBuffer->Create(device, (void*)vertices.data(), sizeType, size);
    return false;
}

bool MeshComponent::CreateIndex(ID3D11Device* device, const std::vector<DWORD>& indices, uint32_t sizeType, uint32_t size) {
    m_indices = size;
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
    if (m_preObjectBuffer) m_preObjectBuffer->Release();
    if (m_shadowConstantBuffer) m_shadowConstantBuffer->Release();
}
