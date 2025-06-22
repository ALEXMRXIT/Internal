#include "DirectionLight.h"
#include "debug.h"
#include "ImGUI/imgui_internal.h"
#include "ViewProjectonData.h"
#include "GameObject.h"

DirectionLight::DirectionLight(GameObject* obj) : AbstractBaseComponent(obj) {
    m_directionBuffer = nullptr;

    directionType = 0;
    shadowType = 0;

    m_directionOption.AmbiendColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    m_directionOption.baked = 0;
    m_directionOption.intensity = 1.0f;
    m_directionOption.shadowStrength = 0.6f;
    m_directionOption.bias = 0.005f;
}

HRESULT DirectionLight::Init(ID3D11Device* device) {
    HRESULT hr{};

    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(BufferDirectionOption);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    hr = device->CreateBuffer(&bufferDesc, NULL, &m_directionBuffer);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create buffer.", hr);
        return hr;
    }

    const float dst = 250.0f;
    m_lightProjectionMatrix = XMMatrixOrthographicOffCenterLH(
        -dst, dst,
        -dst, dst,
        0.1f, 600.0f
    );

    m_device_loader = true;
    return hr;
}

void DirectionLight::Render(ID3D11DeviceContext* context) {
    Quaternion quat = gameObject().GetComponentByType<Transform>()->rotation();

    XMFLOAT4 lightDir = Quaternion::QuaternionToDirection(quat);

    m_directionOption.LightDirection = lightDir;

    context->UpdateSubresource(m_directionBuffer, 0, nullptr, &m_directionOption, 0, 0);
    context->PSSetConstantBuffers(0, 1, &m_directionBuffer);
}

void DirectionLight::Release() {
    if (m_directionBuffer) m_directionBuffer->Release();
}

BufferDirectionLight DirectionLight::UpdateMatrixByDirectionLight(XMMATRIX worldPos) {
    BufferDirectionLight buffer;
    const Transform* transform = (const Transform*)gameObject().GetComponentByType<Transform>();
    Quaternion rotation = transform->rotation();
    XMVECTOR direction = XMVector3Normalize(rotation.m_quat);

    XMMATRIX view = XMMatrixLookAtLH(XMVectorZero(), direction, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0));

    XMStoreFloat4(&buffer.direction, direction);
    buffer.lightViewProj = XMMatrixTranspose(worldPos * view * m_lightProjectionMatrix);
    return buffer;
}

XMMATRIX DirectionLight::GetViewProjectionMatrix() {
    if (!gameObject().IsStatic()) {
        const Transform* transform = (const Transform*)gameObject().GetComponentByType<Transform>();
        Quaternion rotation = transform->rotation();
        XMVECTOR direction = XMVector3Normalize(rotation.m_quat);

        XMMATRIX view = XMMatrixLookAtLH(XMVectorZero(), direction, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0));
        m_viewProjectionCache = view * m_lightProjectionMatrix;
    }
    return m_viewProjectionCache;
}

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
void DirectionLight::UpdateInterfaceInInspector(GameObject* gameObject) {
    ImGui::Dummy(ImVec2(0.0f, 2.0f));
    if (ImGui::CollapsingHeader("Light")) {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.25f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

        ImGui::BeginChild("LightBlock", ImVec2(0.0f, 175.0f), true);
        {
            const char* types[] = { "Direction Light" };
            const char* shadows[] = { "Soft Shadow" };

            const float controlWidth = ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x;

            ImGui::Columns(2, "lightColumns", false);
            ImGui::SetColumnWidth(0, 120.0f);
            ImGui::SetColumnWidth(1, controlWidth);

            ImGui::Text("Type");
            ImGui::NextColumn();
            DirectionLight* directionLight = gameObject->GetComponentByType<DirectionLight>();
            ImGui::SetNextItemWidth(-1);
            ImGui::Combo("##Type", &directionLight->directionType, types, IM_ARRAYSIZE(types));
            ImGui::NextColumn();

            ImGui::Text("Color");
            ImGui::NextColumn();
            ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_InputRGB;
            XMFLOAT4 col = m_directionOption.AmbiendColor;
            float color[3] = { col.x, col.y, col.z };
            ImGui::SetNextItemWidth(-1);
            if (ImGui::ColorEdit3("##Color", color, flags))
                m_directionOption.AmbiendColor = XMFLOAT4(color[0], color[1], color[2], 1.0f);
            ImGui::NextColumn();

            ImGui::Text("Intensity");
            ImGui::NextColumn();
            ImGui::SetNextItemWidth(-1);
            ImGui::SliderFloat("##Intensity", &m_directionOption.intensity, 0.0f, 10.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::NextColumn();

            ImGui::Text("Shadow Type");
            ImGui::NextColumn();
            ImGui::SetNextItemWidth(-1);
            ImGui::Combo("##ShadowType", &directionLight->shadowType, shadows, IM_ARRAYSIZE(shadows));
            ImGui::NextColumn();

            ImGui::SetCursorPosX(40.0f);
            ImGui::Text("Strength");
            ImGui::NextColumn();
            ImGui::SetNextItemWidth(-1);
            ImGui::SliderFloat("##Strength", &m_directionOption.shadowStrength, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::NextColumn();

            ImGui::SetCursorPosX(40.0f);
            ImGui::Text("Bias");
            ImGui::NextColumn();
            ImGui::SetNextItemWidth(-1);
            ImGui::SliderFloat("##Bias", &m_directionOption.bias, 0.0f, 0.1f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::NextColumn();

            ImGui::Columns(1);
        }
        ImGui::EndChild();

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar();
    }
}
#endif