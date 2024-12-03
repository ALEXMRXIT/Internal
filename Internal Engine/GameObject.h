#pragma once
#include "framework.h"
#include "Component.h"

class GameObject {
private:
	Component* m_components;

	__forceinline Transform& transform() const { return m_components->transform(); }

public:
	GameObject();

	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;

	void setPosition(const XMFLOAT3& vector) { transform().setPosition(vector); }
	XMFLOAT3& position() { return transform().position(); }
	void setRotation(const XMFLOAT3& vector) { transform().setRotation(vector); }
	XMFLOAT3& rotation() { return transform().rotation(); }
	void setScale(const XMFLOAT3& vector) { transform().setScale(vector); }
	XMFLOAT3& scale() { return transform().scale(); }
};