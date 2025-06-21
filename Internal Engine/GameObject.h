#pragma once
#include "framework.h"
#include "Component.h"
#include "servlib.h"

class Model;
class Transform;

class GameObject {
private:
	Component* m_components;
	GameObject* m_parent;
	GameObject* m_firstChild;
	bool m_enable;
	bool m_static;

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
	ALWAYS_INLINE TComponent* AddComponent(GameObject* obj) { return m_components->AddComponent<TComponent>(obj); }

	template<class TComponent>
	ALWAYS_INLINE TComponent* GetComponentByType() const { return m_components->GetComponentByType<TComponent>(); }

	uint32_t sizeComponents() const { return m_components->size(); }
	ALWAYS_INLINE const std::vector<AbstractBaseComponent*> getComponents() const { return m_components->components(); }

	void SetParent(GameObject* parent);
	void AddChild(GameObject* child);
	void RemoveChild(GameObject* child);
	GameObject* Parent() const { return m_parent; }
	GameObject* FirstChild() const { return m_firstChild; }

	ALWAYS_INLINE void Enable(bool enable) { m_enable = enable; }
	ALWAYS_INLINE bool IsEnabled() const { return m_enable; }
	ALWAYS_INLINE void Static(bool st) { m_static = st; }
	ALWAYS_INLINE bool IsStatic() const { return m_static; }
	ALWAYS_INLINE bool IsTransparent() const { return false; }
};