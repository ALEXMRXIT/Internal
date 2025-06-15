#include "AbstractBaseComponent.h"
#include "GameObject.h"

void AbstractBaseComponent::gameObject(GameObject* obj) {
	m_object = obj;
}

const GameObject& AbstractBaseComponent::gameObject() const {
	return static_cast<const GameObject&>(*m_object);
}

GameObject& AbstractBaseComponent::gameObject() {
	return static_cast<GameObject&>(*m_object);
}
