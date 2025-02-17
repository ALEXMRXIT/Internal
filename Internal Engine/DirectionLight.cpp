#include "DirectionLight.h"
#include "debug.h"
#include "ImGUI/imgui_internal.h"

DirectionLight::DirectionLight() {
    m_constantLightBuffer = nullptr;
    m_transform = nullptr;
    m_bufferLight.intensity = 1.25f;
    m_bufferLight.ambient = XMFLOAT4(0.82f, 0.91f, 0.92f, 1.0f);
    m_bufferLight.diffuse = XMFLOAT4(0.44f, 0.43f, 0.31f, 1.0f);
    m_bufferLight.darkness = 0.5f;
}

HRESULT DirectionLight::Init(ID3D11Device* device) {
    HRESULT hr{};
    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(BufferDirectionLight);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    hr = device->CreateBuffer(&bufferDesc, NULL, &m_constantLightBuffer);
    if (FAILED(hr))
        DXUT_ERR_MSGBOX("Failed to create buffer.", hr);

    const float orthoWidth = 20.0f;
    const float orthoHeight = 20.0f;
    const float nearZ = 0.1f;
    const float farZ = 1000.0f;
    m_lightProjectionMatrix = XMMatrixOrthographicLH(orthoWidth, orthoHeight, nearZ, farZ);

    m_device_loader = true;
    return hr;
}

void DirectionLight::Update(float deltaTime) {
    if (!m_device_loader) return;
}

void DirectionLight::Render(ID3D11DeviceContext* device_context) {
    if (!m_device_loader) return;
    
    ProjectionLightView();
    device_context->UpdateSubresource(m_constantLightBuffer, 0, nullptr, &m_bufferLight, 0, 0);
    device_context->PSSetConstantBuffers(0, 1, &m_constantLightBuffer);
}

XMMATRIX& DirectionLight::ProjectionLightView() {
    m_bufferLight.direction = XMFLOAT4(
        m_transform->rotation().x,
        m_transform->rotation().y,
        m_transform->rotation().z, 1.0f);

    XMVECTOR lightDirection = XMVector3Normalize(XMLoadFloat3(&m_transform->rotation()));
    XMMATRIX lightViewMatrix = XMMatrixLookAtLH(
        XMVectorSet(m_transform->position().x, m_transform->position().y, m_transform->position().z, 1.0f),
        lightDirection,
        XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
    );

    m_bufferLight.lightView = XMMatrixTranspose(lightViewMatrix * m_lightProjectionMatrix);
    return m_bufferLight.lightView;
}

void DirectionLight::Release() {
    if (m_constantLightBuffer) m_constantLightBuffer->Release();
}

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
void DirectionLight::UpdateInterfaceInInspector(GameObject* gameObject) {
    ImGui::Dummy(ImVec2(0.0f, 2.0f));
    if (ImGui::CollapsingHeader("Direction Light")) {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.25f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

        ImGui::BeginChild("Light", ImVec2(0, 135), true);
        {
            float maxTextWidth = ImGui::CalcTextSize("Light Intensity").x;
            maxTextWidth = ImMax(maxTextWidth, ImGui::CalcTextSize("Ambient Color").x);
            maxTextWidth = ImMax(maxTextWidth, ImGui::CalcTextSize("Diffuse Color").x);

            ImGui::Text("Light Intensity");
            ImGui::SameLine();
            ImGui::SetCursorPosX(maxTextWidth + ImGui::GetStyle().ItemSpacing.x);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::SliderFloat("##Intensity", &m_bufferLight.intensity, 0.0f, 10.0f, "%.2f");

            float ambientColorArray[3] = { 
                m_bufferLight.ambient.x, m_bufferLight.ambient.y, m_bufferLight.ambient.z };
            ImGui::Text("Ambient Color");
            ImGui::SameLine();
            ImGui::SetCursorPosX(maxTextWidth + ImGui::GetStyle().ItemSpacing.x);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGui::ColorEdit3("##AmbientColorEdit", ambientColorArray)) {
                XMFLOAT4 color = XMFLOAT4(ambientColorArray[0],
                    ambientColorArray[1], ambientColorArray[2], m_bufferLight.ambient.w);
                m_bufferLight.ambient = color;
            }

            float diffuseColorArray[3] = { 
                m_bufferLight.diffuse.x, m_bufferLight.diffuse.y, m_bufferLight.diffuse.z };
            ImGui::Text("Diffuse Color");
            ImGui::SameLine();
            ImGui::SetCursorPosX(maxTextWidth + ImGui::GetStyle().ItemSpacing.x);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGui::ColorEdit3("##DiffuseColorEdit", diffuseColorArray)) {
                XMFLOAT4 color = XMFLOAT4(diffuseColorArray[0],
                    diffuseColorArray[1], diffuseColorArray[2], m_bufferLight.diffuse.w);
                m_bufferLight.diffuse = color;
            }

            ImGui::Text("Drakness");
            ImGui::SameLine();
            ImGui::SetCursorPosX(maxTextWidth + ImGui::GetStyle().ItemSpacing.x);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::SliderFloat("##Drakness", &m_bufferLight.darkness, 0.0f, 1.0f, "%.2f");
        }
        ImGui::EndChild();

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar();
    }
}
#endif

void DirectionLight::setMatrix(XMMATRIX& position) { }
