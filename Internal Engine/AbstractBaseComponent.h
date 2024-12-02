#pragma once
#include "Component.h"

class AbstractBaseComponent {
public:
	Component* m_parent;

	AbstractBaseComponent();
};