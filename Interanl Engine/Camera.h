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

	float yaw;
	float pitch;

public:
	Camera();
	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;

	float verticalLeftRight;
	float horizontalBackForward;

	void Update();

	XMMATRIX getView() const { return view; }
} camera;