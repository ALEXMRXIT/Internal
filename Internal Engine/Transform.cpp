#include "Transform.h"
#include "MeshComponent.h"

void Transform::UpdateWorldCoord() {
    XMMATRIX translation = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
    XMMATRIX rotation = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
    XMMATRIX scaling = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

    XMMATRIX localMatrix = scaling * rotation * translation;

    if (m_gameObject && m_gameObject->Parent()) {
        XMMATRIX parentWorldMatrix = m_gameObject->Parent()->transform().getWorldMatrix();
        m_worldMatrix = XMMatrixMultiply(localMatrix, parentWorldMatrix);
    }
    else {
        m_worldMatrix = localMatrix;
    }

    XMVECTOR worldPos = XMVector3TransformCoord(XMVectorSet(0, 0, 0, 1), m_worldMatrix);
    XMStoreFloat3(&m_localPosition, worldPos);

    if (m_gameObject) {
        GameObject* child = m_gameObject->FirstChild();
        while (child) {
            child->transform().UpdateWorldCoord();
            child = child->m_next;
        }
    }
}

Transform::Transform() {
    m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_localPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
    m_worldMatrix = XMMatrixIdentity();
    m_gameObject = nullptr;
    UpdateWorldCoord();
}

Transform::Transform(const Transform& other) {
    *this = other;
}

Transform& Transform::operator=(const Transform& other) {
    if (this != &other) {
        m_position = other.m_position;
        m_localPosition = other.m_localPosition;
        m_rotation = other.m_rotation;
        m_scale = other.m_scale;
        m_worldMatrix = other.m_worldMatrix;
        UpdateWorldCoord();
    }
    return *this;
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
                ImGui::PopStyleVar();

                XMFLOAT3 rot = gameObject->rotation();
                float rotation[3] = { rot.x, rot.y, rot.z };

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 5.0f));
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
                ImGui::PopStyleVar();

                XMFLOAT3 scale = gameObject->scale();
                float scaleData[3] = { scale.x, scale.y, scale.z };

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 5.0f));
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
        }
        ImGui::EndChild();

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar();
    }
}
#endif

void Transform::setLocalPosition(const XMFLOAT3& position) {
    if (m_gameObject->Parent()) {
        XMFLOAT3 parentWorldPos = m_gameObject->Parent()->position();
        m_localPosition = XMFLOAT3(
            position.x - parentWorldPos.x,
            position.y - parentWorldPos.y,
            position.z - parentWorldPos.z
        );
    }
    UpdateWorldCoord();
}
