#pragma once
#include "framework.h"
#include "ResourceLoader.h"
#include "FileSharedBuffer.h"

class GameObject;

class Location {
private:
	std::list<GameObject*> m_objects;
	ResourceLoader* m_loader;
	MeshLoader* m_meshLoader;

public:
	Location();

	Location(const Location&) = delete;
	Location& operator=(const Location&) = delete;

	void Update(float deltaTime);
	void Insert(GameObject* obj);

	std::list<GameObject*> staticObjects() const { return m_objects; }
};

static GameObject* Instantiate(const XMFLOAT3& pos, const XMFLOAT3& rot, const XMFLOAT3& scale);