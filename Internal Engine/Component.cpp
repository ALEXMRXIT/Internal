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
	for (Component& component : m_components) {
		if (typeid(*component) == typeid(TComponent))
			return static_cast<TComponent*>(component);
	}
	return nullptr;
}
