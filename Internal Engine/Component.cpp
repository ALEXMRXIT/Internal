#include "Component.h"
#include "Engine.h"

Component::Component() {
	m_transform = AddComponent<Transform>();
}

template<typename TComponent>
inline TComponent* Component::AddComponent() {
	if (GetComponentByType<TComponent>())
		return nullptr;

	TComponent* component = new TComponent();
	m_components.emplace_back(component);
	return component;
}

template<typename TComponent>
inline TComponent* Component::GetComponentByType() const {
	for (AbstractBaseComponent* component : m_components) {
		if (dynamic_cast<TComponent*>(component))
			return static_cast<TComponent*>(component);
	}
	return nullptr;
}
