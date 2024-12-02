#include "GameObject.h"

GameObject::GameObject() {
	m_components = nullptr;
}

void GameObject::setTransform(const Transform& trans) const {
	transform() = trans;
}
