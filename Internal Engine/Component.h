#pragma once
#include "framework.h"
#include "Transform.h"

class Component {
private:
	std::list<AbstractBaseComponent*> m_components;
	Transform* m_transform; // компонент этот вытащен, так как очень часто используется. Сделано ради оптимизации

public:
	Component();

	Component(const Component&) = delete;
	Component& operator=(const Component&) = delete;

	template<class TComponent>
	TComponent* AddComponent() {
		if (GetComponentByType<TComponent>())
			return nullptr;

		TComponent* component = new TComponent();
		m_components.emplace_back(component);
		return component;
	}

	template<class TComponent>
	TComponent* GetComponentByType() const {
		for (AbstractBaseComponent* component : m_components) {
			if (dynamic_cast<TComponent*>(component))
				return static_cast<TComponent*>(component);
		}
		return nullptr;
	}

	uint32_t size() const { return (uint32_t)m_components.size(); }
	std::list<AbstractBaseComponent*> components() const { return m_components; }

	__forceinline Transform& transform() const { return *m_transform; }
};
