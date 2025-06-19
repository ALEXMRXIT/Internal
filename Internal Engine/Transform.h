#pragma once
#include "framework.h"
#include "AbstractBaseComponent.h"
#include "Quaternion.h"

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
#include "ImGUIDevice.h"
#endif

class GameObject;

class Transform : public AbstractBaseComponent {
private:
    XMFLOAT3 m_position;
    Quaternion m_rotation;
    XMFLOAT3 m_scale;

public:
    Transform(GameObject* obj);

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
    void UpdateInterfaceInInspector(GameObject* gameObject) override;
#endif

    void position(const XMFLOAT3 position);
    XMFLOAT3 position() const { return m_position; }

    void rotation(Quaternion quaternion);
    Quaternion rotation() const { return m_rotation; }

    void scale(const XMFLOAT3 vector);
    XMFLOAT3 scale() const { return m_scale; }

    XMMATRIX GetWorldMatrix() const;
};