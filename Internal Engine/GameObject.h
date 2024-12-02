#pragma once

class Component;

class GameObject {
private:
	Component* m_components;

public:
	GameObject();

	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;
};