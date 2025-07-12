#include "Transform.h"
#include "MeshComponent.h"

XMMATRIX Transform::GetWorldMatrix() const {
    XMMATRIX translation = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
    XMMATRIX rot = XMMatrixRotationRollPitchYaw(m_rotation.X(), m_rotation.Y(), m_rotation.Z());
    XMMATRIX scaling = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
    return scaling * rot * translation;
}

Transform::Transform(GameObject* obj) : AbstractBaseComponent(obj) {
    m_position = Vector3(0.0f, 0.0f, 0.0f);
    m_rotation = Quaternion::Identity();
    m_scale = Vector3(1.0f, 1.0f, 1.0f);
}

void Transform::position(const Vector3 position) {
    m_position = position;
}

void Transform::rotation(Quaternion quaternion) {
    m_rotation = quaternion;
}

void Transform::scale(const Vector3 vector) {
    m_scale = vector;
}

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
void Transform::UpdateInterfaceInInspector(GameObject* gameObject) {
    ImGui::Dummy(ImVec2(0.0f, 2.0f));
    if (ImGui::CollapsingHeader("Transform")) {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.25f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

        ImGui::BeginChild("ButtonBlock", ImVec2(0, 95), true);
        {
            float availableWidth = ImGui::GetContentRegionAvail().x;
            float buttonWidth = (availableWidth - ImGui::GetStyle().ItemSpacing.x) / 2;

            ImGui::BeginGroup();
            {
                const float availableWidth = ImGui::GetContentRegionAvail().x;
                const float labelWidth = 85.0f;
                const float axisLabelWidth = 20.0f;
                float dragFloatWidth = (availableWidth - labelWidth - axisLabelWidth * 3 - ImGui::GetStyle().ItemSpacing.x * 5) / 3;

                Transform* transform = gameObject->GetComponentByType<Transform>();
                bool gameObjectStatic = gameObject->IsStatic();
                ImGuiSliderFlags flags = ImGuiSliderFlags_AlwaysClamp;

                {
                    Vector3 pos = transform->position();

                    const char* posText = "Position:";
                    ImGui::Text("%s", posText);
                    ImGui::SameLine();
                    ImGui::SetCursorPosX(labelWidth);

                    bool positionChange = false;

                    if (gameObjectStatic)
                        ImGui::BeginDisabled();
                    ImGui::Text("X:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(dragFloatWidth);
                    positionChange |= ImGui::DragFloat("##PosX", &pos.x, 0.25f, -FLT_MAX, FLT_MAX, "%.2f", flags);
                    ImGui::SameLine();

                    ImGui::Text("Y:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(dragFloatWidth);
                    positionChange |= ImGui::DragFloat("##PosY", &pos.y, 0.25f, -FLT_MAX, FLT_MAX, "%.2f", flags);
                    ImGui::SameLine();

                    ImGui::Text("Z:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(dragFloatWidth);
                    positionChange |= ImGui::DragFloat("##PosZ", &pos.z, 0.25f, -FLT_MAX, FLT_MAX, "%.2f", flags);
                    if (gameObjectStatic)
                        ImGui::EndDisabled();

                    if (positionChange)
                        transform->position(pos);
                }

                {
                    Quaternion quaternion = transform->rotation();
                    float angleX = XMConvertToDegrees(quaternion.X());
                    float angleY = XMConvertToDegrees(quaternion.Y());
                    float angleZ = XMConvertToDegrees(quaternion.Z());

                    const char* rotText = "Rotation:";
                    ImGui::Text("%s", rotText);
                    ImGui::SameLine();
                    ImGui::SetCursorPosX(labelWidth);

                    bool rotationChanged = false;

                    if (gameObjectStatic)
                        ImGui::BeginDisabled();
                    ImGui::Text("X:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(dragFloatWidth);
                    rotationChanged |= ImGui::DragFloat("##RotX", &angleX, 0.25f, -FLT_MAX, FLT_MAX, "%.2f", flags);
                    ImGui::SameLine();

                    ImGui::Text("Y:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(dragFloatWidth);
                    rotationChanged |= ImGui::DragFloat("##RotY", &angleY, 0.25f, -FLT_MAX, FLT_MAX, "%.2f", flags);
                    ImGui::SameLine();

                    ImGui::Text("Z:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(dragFloatWidth);
                    rotationChanged |= ImGui::DragFloat("##RotZ", &angleZ, 0.25f, -FLT_MAX, FLT_MAX, "%.2f", flags);
                    if (gameObjectStatic)
                        ImGui::EndDisabled();

                    if (rotationChanged)
                        transform->rotation(Quaternion(angleX, angleY, angleZ, 0.0f));
                }

                {
                    Vector3 scale = transform->scale();

                    const char* scaleText = "Scale:";
                    ImGui::Text("%s", scaleText);
                    ImGui::SameLine();
                    ImGui::SetCursorPosX(labelWidth);

                    bool scaleChange = false;

                    if (gameObjectStatic)
                        ImGui::BeginDisabled();
                    ImGui::Text("X:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(dragFloatWidth);
                    scaleChange |= ImGui::DragFloat("##ScaleX", &scale.x, 0.25f, -FLT_MAX, FLT_MAX, "%.2f", flags);
                    ImGui::SameLine();

                    ImGui::Text("Y:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(dragFloatWidth);
                    scaleChange |= ImGui::DragFloat("##ScaleY", &scale.y, 0.25f, -FLT_MAX, FLT_MAX, "%.2f", flags);
                    ImGui::SameLine();

                    ImGui::Text("Z:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(dragFloatWidth);
                    scaleChange |= ImGui::DragFloat("##ScaleZ", &scale.z, 0.25f, -FLT_MAX, FLT_MAX, "%.2f", flags);
                    if (gameObjectStatic)
                        ImGui::EndDisabled();

                    if (scaleChange)
                        transform->scale(scale);
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