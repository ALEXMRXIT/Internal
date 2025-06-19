#pragma once
#include "framework.h"

class GameObject;

class AbstractBaseComponent {
private:
	GameObject* m_object;

public:
	AbstractBaseComponent(GameObject* obj);

	const GameObject& gameObject() const;
	GameObject& gameObject();

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
	virtual void UpdateInterfaceInInspector(GameObject* gameObject) = 0;
#endif
};