#include "Component.h"
#include "Engine.h"

bool Component::operator==(const Component& component) const {
	return true;
}

void Component::AddComponent(Component* component) {
	if (!TryGetComponent(component)) {
		m_components.emplace_back(component);
	}
}

bool Component::TryGetComponent(Component* component) const {
	return std::find(m_components.cbegin(), m_components.cend(), component) != m_components.cend();
}

Component* Component::GetComponent(uint32_t uid) const {
    auto it = std::find_if(m_components.cbegin(), m_components.cend(), 
		[uid](const Component* comp) { return comp->m_uid == uid; });
    if (it != m_components.cend())
        return *it;
	return nullptr;
}