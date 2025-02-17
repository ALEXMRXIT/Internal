#include "DirectionLight.h"
#include "debug.h"
#include "ImGUI/imgui_internal.h"
#include "ViewProjectonData.h"

void DirectionLight::UpdateMatrix() {
    XMFLOAT3 rotation = m_transform->rotation();
    XMVECTOR direction = XMLoadFloat3(&rotation);
    direction = XMVector3Equal(direction, XMVectorZero()) ? XMVectorSet(0, 0, 1, 0) : XMVector3Normalize(direction);
    XMVECTOR lightPos = direction * 50.0f;

    m_bufferLight.lightViewProj = XMMatrixLookAtLH(lightPos, -direction, XMVectorSet(0, 1, 0, 0));
    m_bufferLight.direction = XMFLOAT4(XMVectorGetX(direction), XMVectorGetY(direction), XMVectorGetZ(direction), 1.0f);
}

DirectionLight::DirectionLight() {
    m_constantLightBuffer = nullptr;
    m_transform = nullptr;
    m_bufferLight.intensity = 1.25f;
    m_viewProjectionData = nullptr;
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

    const float orthoWidth = 640.0f;
    const float screenAspect = 360.0f;
    const float nearZ = 0.1f;
    const float farZ = 200.0f;
    m_lightProjectionMatrix = XMMatrixOrthographicLH(orthoWidth, screenAspect, nearZ, farZ);
    UpdateMatrix();
    m_viewProjectionData = new ViewProjectonData(m_bufferLight.lightViewProj, m_lightProjectionMatrix);

    m_device_loader = true;
    return hr;
}

void DirectionLight::Update(float deltaTime) {
    if (!m_device_loader) return;
}

void DirectionLight::Render(ID3D11DeviceContext* device_context) {
    if (!m_device_loader) return;

    UpdateMatrix();
    device_context->UpdateSubresource(m_constantLightBuffer, 0, nullptr, &m_bufferLight, 0, 0);
    device_context->PSSetConstantBuffers(0, 1, &m_constantLightBuffer);
}

void DirectionLight::Release() {
    if (m_constantLightBuffer) m_constantLightBuffer->Release();
    if (m_viewProjectionData) delete m_viewProjectionData;
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
        }
        ImGui::EndChild();

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar();
    }
}
#endif

void DirectionLight::setMatrix(XMMATRIX& position) { }
