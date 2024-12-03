#pragma once
#include "framework.h"
#include "AbstractBaseComponent.h"

class Transform : public AbstractBaseComponent {
private:
	XMMATRIX m_worldPosition;

	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMFLOAT3 m_scale;

	void UpdateWorldCoord();

public:
	Transform();
	Transform(const Transform& other);

	Transform& operator=(const Transform& other);

	XMMATRIX& getWorldMatrix() { return m_worldPosition; }

	__forceinline void setPosition(const XMFLOAT3& vector) { m_position = vector; UpdateWorldCoord(); }
	__forceinline XMFLOAT3& position() { return m_position; }
	__forceinline void setRotation(const XMFLOAT3& vector) { m_rotation = vector; UpdateWorldCoord(); }
	__forceinline XMFLOAT3& rotation() { return m_rotation; }
	__forceinline void setScale(const XMFLOAT3& vector) { m_scale = vector; UpdateWorldCoord(); }
	__forceinline XMFLOAT3& scale() { return m_scale; }
};