#pragma once
class GameObject;

class AbstractBaseComponent {
public:
	virtual ~AbstractBaseComponent() = default;

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
	virtual void UpdateInterfaceInInspector(GameObject* gameObject) = 0;
#endif
};