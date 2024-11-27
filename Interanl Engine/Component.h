#pragma once
#include "framework.h"

class AbstractBaseComponent;

class Component {
private:
	std::list<AbstractBaseComponent*> m_components;

public:
	Component(const Component&) = delete;
	Component& operator=(const Component&) = delete;

	template<typename TComponent>
	TComponent* AddComponent() {
		if (GetComponentByType<TComponent>())
			return nullptr;

		TComponent* component = new TComponent();
		m_components.emplace_back(component);
		return component;
	}

	template<typename TComponent>
	TComponent* GetComponentByType() const {
		for (Component& component : m_components) {
			if (typeid(*component) == typeid(TComponent))
				return static_cast<TComponent*>(component);
		}
		return nullptr;
	}
};