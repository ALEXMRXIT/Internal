#include "Component.h"

Component::Component() {
	m_transform = AddComponent<Transform>();
}
