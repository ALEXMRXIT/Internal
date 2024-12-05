#pragma once
#include "framework.h"

class GameObject;

class Location {
private:
	std::list<GameObject*> m_objects;

public:
	Location();

	Location(const Location&) = delete;
	Location& operator=(const Location&) = delete;

	void Update(float deltaTime);

	void Insert(GameObject* obj);
};

static GameObject* Instantiate(const XMFLOAT3& pos, const XMFLOAT3& rot, const XMFLOAT3& scale);