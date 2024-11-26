#pragma once
#include "framework.h"

class Transform {
public:
	XMMATRIX Position;
	XMMATRIX Rotation;
	XMMATRIX Scale;

	Transform();
	Transform(const Transform& other);

	Transform& operator=(const Transform& other);
};