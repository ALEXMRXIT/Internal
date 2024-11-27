#pragma once
#include "framework.h"

class Component {
private:
	std::list<Component*> m_components;

public:
	bool operator ==(const Component& component) const;

	Component(const Component&) = delete;
	Component& operator=(const Component&) = delete;

	void AddComponent(Component* component);
	bool TryGetComponent(Component* component) const;
	Component* GetComponent(uint32_t uid) const;
};