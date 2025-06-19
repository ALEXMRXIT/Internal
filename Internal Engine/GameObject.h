#pragma once
#include "framework.h"
#include "Component.h"

class Model;
class Transform;

class GameObject {
private:
	Component* m_components;
	GameObject* m_parent;
	GameObject* m_firstChild;
	bool m_enable;

public:
	std::string name;
	int selectedTag;
	int selectedLayer;
	Model* model;
	bool serialize;
	bool selectable;

	GameObject* m_prev;
	GameObject* m_next;

public:
	GameObject();

	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;

	template<class TComponent>
	TComponent* AddComponent(GameObject* obj) { return m_components->AddComponent<TComponent>(obj); }

	template<class TComponent>
	TComponent* GetComponentByType() const { return m_components->GetComponentByType<TComponent>(); }

	uint32_t sizeComponents() const { return m_components->size(); }
	std::list<AbstractBaseComponent*> getComponents() const { return m_components->components(); }

	void SetParent(GameObject* parent);
	void AddChild(GameObject* child);
	void RemoveChild(GameObject* child);
	GameObject* Parent() const { return m_parent; }
	GameObject* FirstChild() const { return m_firstChild; }

	void Enable(bool enable);
	bool isEnabled() const { return m_enable; }
	bool isTransparent() const;
};