#include "Transform.h"
#include "MeshComponent.h"

void Transform::UpdateWorldCoord() {
    XMMATRIX translation = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
    XMMATRIX rotation = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
    XMMATRIX scaling = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
    m_worldPosition = scaling * rotation * translation;
}

Transform::Transform() {
    m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
    UpdateWorldCoord();
}

Transform::Transform(const Transform& other) {
    *this = other;
}

Transform& Transform::operator=(const Transform& other) {
    if (this != &other) {
        m_position = other.m_position;
        m_rotation = other.m_rotation;
        m_scale = other.m_scale;
        m_worldPosition = other.m_worldPosition;
        UpdateWorldCoord();
    }
    return *this;
}

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
void Transform::UpdateInterfaceInInspector(GameObject* gameObject) {
    if (ImGui::CollapsingHeader("Transform")) {
        static int selectedButton = 1;
        static int last_selected = 1;
        ImGui::BeginChild("ButtonBlock", ImVec2(0, 40), true);
        {
            float availableWidth = ImGui::GetContentRegionAvail().x;
            float buttonWidth = (availableWidth - ImGui::GetStyle().ItemSpacing.x) / 2;

            if (selectedButton == 1) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.5f, 1.0f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.6f, 1.0f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.69f, 1.0f, 1.0f));
            }
            if (ImGui::Button("World", ImVec2(buttonWidth, 20))) {
                last_selected = 1;
            }
            if (selectedButton == 1) ImGui::PopStyleColor(3);

            ImGui::SameLine();

            if (selectedButton == 2) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.5f, 1.0f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.6f, 1.0f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.69f, 1.0f, 1.0f));
            }
            if (ImGui::Button("Local", ImVec2(buttonWidth, 20))) {
                last_selected = 2;
            }
            if (selectedButton == 2) ImGui::PopStyleColor(3);
        }
        ImGui::EndChild();

        ImGui::Dummy(ImVec2(0.0f, 2.0f));
        ImGui::BeginGroup();
        {
            XMFLOAT3 pos = gameObject->position();
            float position[3] = { pos.x, pos.y, pos.z };

            const float availableWidth = ImGui::GetContentRegionAvail().x;
            const float labelWidth = 85.0f;
            const float axisLabelWidth = 20.0f;
            float dragFloatWidth = (availableWidth - labelWidth - axisLabelWidth * 3 - ImGui::GetStyle().ItemSpacing.x * 5) / 3;

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 5.0f));
            const char* posText = "Position:";
            ImGui::Text("%s", posText);
            ImGui::SameLine();
            ImGui::SetCursorPosX(labelWidth);

            ImGui::Text("X:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(dragFloatWidth);
            if (ImGui::DragFloat("##PosX", &position[0], 0.1f))
                gameObject->setPosition(XMFLOAT3(position[0], position[1], position[2]));
            ImGui::SameLine();

            ImGui::Text("Y:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(dragFloatWidth);
            if (ImGui::DragFloat("##PosY", &position[1], 0.1f))
                gameObject->setPosition(XMFLOAT3(position[0], position[1], position[2]));
            ImGui::SameLine();

            ImGui::Text("Z:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(dragFloatWidth);
            if (ImGui::DragFloat("##PosZ", &position[2], 0.1f))
                gameObject->setPosition(XMFLOAT3(position[0], position[1], position[2]));

            XMFLOAT3 rot = gameObject->rotation();
            float rotation[3] = { rot.x, rot.y, rot.z };

            const char* rotText = "Rotation:";
            ImGui::Text("%s", rotText);
            ImGui::SameLine();
            ImGui::SetCursorPosX(labelWidth);

            ImGui::Text("X:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(dragFloatWidth);
            if (ImGui::DragFloat("##RotX", &rotation[0], 0.1f))
                gameObject->setRotation(XMFLOAT3(rotation[0], rotation[1], rotation[2]));
            ImGui::SameLine();

            ImGui::Text("Y:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(dragFloatWidth);
            if (ImGui::DragFloat("##RotY", &rotation[1], 0.1f))
                gameObject->setRotation(XMFLOAT3(rotation[0], rotation[1], rotation[2]));
            ImGui::SameLine();

            ImGui::Text("Z:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(dragFloatWidth);
            if (ImGui::DragFloat("##RotZ", &rotation[2], 0.1f))
                gameObject->setRotation(XMFLOAT3(rotation[0], rotation[1], rotation[2]));

            XMFLOAT3 scale = gameObject->scale();
            float scaleData[3] = { scale.x, scale.y, scale.z };

            const char* scaleText = "Scale:";
            ImGui::Text("%s", scaleText);
            ImGui::SameLine();
            ImGui::SetCursorPosX(labelWidth);

            ImGui::Text("X:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(dragFloatWidth);
            if (ImGui::DragFloat("##ScaleX", &scaleData[0], 0.1f))
                gameObject->setScale(XMFLOAT3(scaleData[0], scaleData[1], scaleData[2]));
            ImGui::SameLine();

            ImGui::Text("Y:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(dragFloatWidth);
            if (ImGui::DragFloat("##ScaleY", &scaleData[1], 0.1f))
                gameObject->setScale(XMFLOAT3(scaleData[0], scaleData[1], scaleData[2]));
            ImGui::SameLine();

            ImGui::Text("Z:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(dragFloatWidth);
            if (ImGui::DragFloat("##ScaleZ", &scaleData[2], 0.1f))
                gameObject->setScale(XMFLOAT3(scaleData[0], scaleData[1], scaleData[2]));
            ImGui::PopStyleVar();
        }

        ImGui::EndGroup();
        selectedButton = last_selected;
    }
}
#endif