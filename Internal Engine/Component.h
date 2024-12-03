#pragma once
#include "framework.h"
#include "Transform.h"

class Component {
private:
	std::list<AbstractBaseComponent*> m_components;
	Transform* m_transform;

public:
	Component();

	Component(const Component&) = delete;
	Component& operator=(const Component&) = delete;

	template<typename TComponent>
	TComponent* AddComponent();

	template<typename TComponent>
	TComponent* GetComponentByType() const;

	__forceinline Transform& transform() const { return *m_transform; }
};
