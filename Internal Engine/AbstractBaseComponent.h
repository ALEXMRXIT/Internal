#pragma once
#include "framework.h"

class GameObject;

class AbstractBaseComponent {
public:
	virtual void setMatrix(XMMATRIX& position) = 0;

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
	virtual void UpdateInterfaceInInspector(GameObject* gameObject) = 0;
#endif
};