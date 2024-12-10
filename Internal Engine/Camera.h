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
	XMMATRIX projection;
	XMMATRIX world;

public:
	Camera();
	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;

	float verticalLeftRight;
	float horizontalBackForward;

	float yaw;
	float pitch;

	void Update();
	void SetProjection();

	XMMATRIX getView() const { return view; }
	XMMATRIX getProjection() const { return projection; }
	XMMATRIX getWorld() const { return world; }
	XMVECTOR getPos() const { return position; }
} camera;