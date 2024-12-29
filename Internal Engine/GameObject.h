#pragma once
#include "framework.h"
#include "Component.h"

class GameObject {
private:
	Component* m_components;

public:
	std::string name;

public:
	GameObject();

	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;

	template<class TComponent>
	TComponent* AddComponent() { return m_components->AddComponent<TComponent>(); }

	template<class TComponent>
	TComponent* GetComponentByType() const { return m_components->GetComponentByType<TComponent>(); }

	__forceinline Transform& transform() const { return m_components->transform(); }

	void setPosition(const XMFLOAT3& vector) const { transform().setPosition(vector); }
	XMFLOAT3& position() const { return transform().position(); }
	void setRotation(const XMFLOAT3& vector) const { transform().setRotation(vector); }
	XMFLOAT3& rotation() const { return transform().rotation(); }
	void setScale(const XMFLOAT3& vector) const { transform().setScale(vector); }
	XMFLOAT3& scale() const { return transform().scale(); }
};