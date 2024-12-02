#include "Transform.h"

void Transform::UpdateWorldCoord() {
    m_worldPosition = 
        XMMatrixTranslation(m_position.x, m_position.y, m_position.z) *
        XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z) *
        XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
}

Transform::Transform() {
    m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
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
    }
    return *this;
}