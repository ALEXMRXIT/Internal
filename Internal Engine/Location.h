#pragma once
#include "framework.h"
#include "Skybox.h"
#include "ResourceLoader.h"
#include "FileSharedBuffer.h"
#include "DirectionLight.h"
#include "servlib.h"

class GameObject;

class Location {
private:
	std::list<GameObject*> m_objects;
	ResourceLoader* m_loader;
	MeshLoader* m_meshLoader;

public:
	Skybox* m_skybox;
	DirectionLight* m_directionLight;

public:
	Location(ID3D11Device* device);

	Location(const Location&) = delete;
	Location& operator=(const Location&) = delete;

	void Update(float deltaTime);
	void Insert(GameObject* obj);

	std::list<GameObject*>& staticObjects() { return m_objects; }
};

SERVLIBFINLINE static SERVLIBCALL GameObject* Instantiate(const XMFLOAT3 pos);
SERVLIBFINLINE static SERVLIBCALL GameObject* Instantiate(const XMFLOAT3 pos, const Quaternion quaternion);
SERVLIBFINLINE static SERVLIBCALL GameObject* Instantiate(const XMFLOAT3 pos, const Quaternion quaternion, const XMFLOAT3 scale);