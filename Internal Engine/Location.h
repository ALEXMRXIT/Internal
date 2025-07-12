#pragma once
#include "framework.h"
#include "Skybox.h"
#include "ResourceLoader.h"
#include "FileSharedBuffer.h"
#include "DirectionLight.h"
#include "servlib.h"
#include "Camera.h"

class GameObject;

class Location {
private:
	std::list<GameObject*> m_objects;
	ResourceLoader* m_loader;
	MeshLoader* m_meshLoader;

public:
	Skybox* m_skybox;
	Camera* m_main_camera;
	DirectionLight* m_directionLight;

public:
	Location(ID3D11Device* device);

	Location(const Location&) = delete;
	Location& operator=(const Location&) = delete;

	void Update(float deltaTime);
	void Insert(GameObject* obj);

	std::list<GameObject*>& staticObjects() { return m_objects; }
};

ALWAYS_INLINE static SERVLIBCALL GameObject* Instantiate();
ALWAYS_INLINE static SERVLIBCALL GameObject* Instantiate(const Vector3 pos);
ALWAYS_INLINE static SERVLIBCALL GameObject* Instantiate(const Vector3 pos, const Quaternion quaternion);
ALWAYS_INLINE static SERVLIBCALL GameObject* Instantiate(const Vector3 pos, const Quaternion quaternion, const Vector3 scale);