#pragma once
#include "framework.h"

class Transform {
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

	void setPosition(const XMFLOAT3& vector) { m_position = vector; UpdateWorldCoord(); }
	XMFLOAT3& position() { return m_position; }
	void setRotation(const XMFLOAT3& vector) { m_rotation = vector; UpdateWorldCoord(); }
	XMFLOAT3& rotation() { return m_rotation; }
	void setScale(const XMFLOAT3& vector) { m_scale = vector; UpdateWorldCoord(); }
	XMFLOAT3& scale() { return m_scale; }
};