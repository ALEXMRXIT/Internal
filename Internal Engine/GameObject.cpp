#include "GameObject.h"

GameObject::GameObject() {
	m_components = new Component();
	enable = true;
	serialize = true;
	selectable = false;
}
