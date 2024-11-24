#pragma once
#include "framework.h"

class Component {
private:
	std::list<Component*> m_components;

protected:
	mutable uint32_t m_uid;

public:
	Component();

	bool operator ==(const Component& component) const;

	Component(const Component&) = delete;
	Component& operator=(const Component&) = delete;

	void AddComponent(const Component* component);
	bool TryGetComponent(const Component* component) const;
	Component* GetComponent(uint32_t uid) const;
};