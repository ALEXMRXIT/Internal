#pragma once
#include "framework.h"

class GameObject;
class Transform;

class Location {
public:
	Location(const Location&) = delete;
	Location& operator=(const Location&) = delete;
};

static GameObject* Instantiate(const XMFLOAT3& pos, const XMFLOAT3& rot, const XMFLOAT3& scale);