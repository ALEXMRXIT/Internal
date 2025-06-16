#include "Transform.h"
#include "MeshComponent.h"

XMMATRIX Transform::GetWorldMatrix() const {
    XMMATRIX translation = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
    XMMATRIX rot = XMMatrixRotationQuaternion(m_rotation.quat);
    XMMATRIX scaling = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
    return scaling * rot * translation;
}

Transform::Transform() {
    m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_rotation = Quaternion::Identity();
    m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
}

void Transform::position(const XMFLOAT3 position) {
    m_position = position;
}

void Transform::rotation(Quaternion quaternion) {
    m_rotation = quaternion;
}

void Transform::scale(const XMFLOAT3 vector) {
    m_scale = vector;
}

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
void Transform::UpdateInterfaceInInspector(GameObject* gameObject) {
    ImGui::Dummy(ImVec2(0.0f, 2.0f));
    if (ImGui::CollapsingHeader("Transform")) {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.25f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

        ImGui::BeginChild("ButtonBlock", ImVec2(0, 115), true);
        {
            float availableWidth = ImGui::GetContentRegionAvail().x;
            float buttonWidth = (availableWidth - ImGui::GetStyle().ItemSpacing.x) / 2;

            ImGui::BeginGroup();
            {
                const float availableWidth = ImGui::GetContentRegionAvail().x;
                const float labelWidth = 85.0f;
                const float axisLabelWidth = 20.0f;
                float dragFloatWidth = (availableWidth - labelWidth - axisLabelWidth * 3 - ImGui::GetStyle().ItemSpacing.x * 5) / 3;

                {
                    XMFLOAT3 pos = gameObject->transform().position();
                    float position[3] = { pos.x, pos.y, pos.z };

                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 5.0f));
                    const char* posText = "Position:";
                    ImGui::Text("%s", posText);
                    ImGui::SameLine();
                    ImGui::SetCursorPosX(labelWidth);

                    ImGui::Text("X:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(dragFloatWidth);
                    ImGui::DragFloat("##PosX", &position[0], 0.1f);
                    ImGui::SameLine();

                    ImGui::Text("Y:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(dragFloatWidth);
                    ImGui::DragFloat("##PosY", &position[1], 0.1f);
                    ImGui::SameLine();

                    ImGui::Text("Z:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(dragFloatWidth);
                    ImGui::DragFloat("##PosZ", &position[2], 0.1f);
                    ImGui::PopStyleVar();
                    gameObject->transform().position(XMFLOAT3(position[0], position[1], position[2]));
                }

                {
                    XMFLOAT3 rot = gameObject->transform().rotation().ToEulerAngles();
                    float rotation[3] = { XMConvertToDegrees(rot.x), XMConvertToDegrees(rot.y), XMConvertToDegrees(rot.z) };

                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 5.0f));
                    const char* rotText = "Rotation:";
                    ImGui::Text("%s", rotText);
                    ImGui::SameLine();
                    ImGui::SetCursorPosX(labelWidth);

                    bool rotationChanged = false;
                    float newRotation[3] = { rotation[0], rotation[1], rotation[2] };

                    ImGui::Text("X:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(dragFloatWidth);
                    rotationChanged |= ImGui::DragFloat("##RotX", &newRotation[0], 0.25f, -90.0f, 90.0f, "%.2f");
                    ImGui::SameLine();

                    ImGui::Text("Y:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(dragFloatWidth);
                    rotationChanged |= ImGui::DragFloat("##RotY", &newRotation[1], 0.25f, -90.0f, 90.0f, "%.2f");
                    ImGui::SameLine();

                    ImGui::Text("Z:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(dragFloatWidth);
                    rotationChanged |= ImGui::DragFloat("##RotZ", &newRotation[2], 0.25f, -90.0f, 90.0f, "%.2f");

                    if (rotationChanged) {
                        gameObject->transform().rotation(Quaternion::EulerAngles(
                            newRotation[0],
                            newRotation[1],
                            newRotation[2]
                        ));
                    }
                    ImGui::PopStyleVar();
                }

                {
                    XMFLOAT3 scale = gameObject->transform().scale();
                    float scaleData[3] = { scale.x, scale.y, scale.z };

                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 5.0f));
                    const char* scaleText = "Scale:";
                    ImGui::Text("%s", scaleText);
                    ImGui::SameLine();
                    ImGui::SetCursorPosX(labelWidth);

                    ImGui::Text("X:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(dragFloatWidth);
                    ImGui::DragFloat("##ScaleX", &scaleData[0], 0.1f);
                    ImGui::SameLine();

                    ImGui::Text("Y:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(dragFloatWidth);
                    ImGui::DragFloat("##ScaleX", &scaleData[1], 0.1f);
                    ImGui::SameLine();

                    ImGui::Text("Z:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(dragFloatWidth);
                    ImGui::DragFloat("##ScaleX", &scaleData[2], 0.1f);
                    ImGui::PopStyleVar();
                    gameObject->transform().scale(XMFLOAT3(scaleData[0], scaleData[1], scaleData[2]));
                }
            }
            ImGui::EndGroup();
        }
        ImGui::EndChild();

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar();
    }
}
#endif