#pragma once
#include "framework.h"
#include "Component.h"

class GameObject {
private:
	Component* m_components;

public:
	GameObject();

	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;

	Transform& transform() const { return m_components->transform(); }
	void setTransform(const Transform& trans) const;

	
};