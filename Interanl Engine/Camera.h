#pragma once
#include "framework.h"

extern class Camera {
private:
	XMVECTOR position;
	XMMATRIX rotation;
	XMMATRIX view;
	XMVECTOR target;
	XMVECTOR up;
	XMVECTOR forward;
	XMVECTOR right;

public:
	Camera();
	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;

	float verticalLeftRight;
	float horizontalBackForward;

	float yaw;
	float pitch;

	void Update();

	XMMATRIX getView() const { return view; }
} camera;