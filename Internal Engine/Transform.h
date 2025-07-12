#pragma once
#include "framework.h"
#include "AbstractBaseComponent.h"
#include "Quaternion.h"
#include "Vector3.h"

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
#include "ImGUIDevice.h"
#endif

class GameObject;

class Transform : public AbstractBaseComponent {
private:
    Vector3 m_position;
    Quaternion m_rotation;
    Vector3 m_scale;

public:
    Transform(GameObject* obj);

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
    void UpdateInterfaceInInspector(GameObject* gameObject) override;
#endif

    void position(const Vector3 position);
    Vector3 position() const { return m_position; }

    void rotation(Quaternion quaternion);
    Quaternion rotation() const { return m_rotation; }

    void scale(const Vector3 vector);
    Vector3 scale() const { return m_scale; }

    XMMATRIX GetWorldMatrix() const;
};