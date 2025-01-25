#pragma once
#include "framework.h"
#include "AbstractBaseComponent.h"

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
#include "ImGUIDevice.h"
#endif

class GameObject;

class Transform : public AbstractBaseComponent {
private:
    XMFLOAT3 m_position;
    XMFLOAT3 m_localPosition;
    XMFLOAT3 m_rotation;
    XMFLOAT3 m_scale;

    XMMATRIX m_worldMatrix;

    GameObject* m_gameObject;

public:
    void UpdateWorldCoord();

public:
    Transform();
    Transform(const Transform& other);

    Transform& operator=(const Transform& other);

    void setMatrix(XMMATRIX& position) override {};

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
    void UpdateInterfaceInInspector(GameObject* gameObject) override;
#endif

    __forceinline void setPosition(const XMFLOAT3& position);
    __forceinline void setLocalPosition(const XMFLOAT3& position);

    __forceinline XMFLOAT3& position() { return m_position; }
    __forceinline XMFLOAT3& localPosition() { return m_localPosition; }

    __forceinline void setRotation(const XMFLOAT3& vector) {
        m_rotation = vector;
        UpdateWorldCoord();
    }

    __forceinline XMFLOAT3& rotation() { return m_rotation; }

    __forceinline void setScale(const XMFLOAT3& vector) {
        m_scale = vector;
        UpdateWorldCoord();
    }

    __forceinline XMFLOAT3& scale() { return m_scale; }
    __forceinline XMMATRIX& getWorldMatrix() { return m_worldMatrix; }

    void setGameObject(GameObject* obj) { m_gameObject = obj; }
};