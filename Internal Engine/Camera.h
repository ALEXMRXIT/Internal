#pragma once
#include "framework.h"

extern class Camera {
private:
	XMVECTOR position;
	XMMATRIX view;
	XMVECTOR target;
	XMVECTOR up;
	XMVECTOR forward;
	XMVECTOR right;
	XMMATRIX projection;

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

	inline const XMMATRIX& getView() const { return view; }
	inline const XMMATRIX& getProjection() const { return projection; }
	inline const XMVECTOR& getPos() const { return position; }
} camera;