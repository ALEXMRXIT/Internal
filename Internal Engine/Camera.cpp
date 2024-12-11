#include "Camera.h"
#include "Engine.h"

Camera::Camera() {
	position = XMVectorSet(0.0f, 5.0f, -8.0f, 0.0f);
	target = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	view = XMMatrixLookAtLH(position, target, up);

	forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	right = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	horizontalBackForward = 0.0f;
	verticalLeftRight = 0.0f;

	yaw = 0.0f;
	pitch = 0.0f;
}

void Camera::Update() {
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f);

	forward = XMVector3TransformCoord(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationMatrix);
	right = XMVector3TransformCoord(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), rotationMatrix);
	up = XMVector3Cross(forward, right);

	position += verticalLeftRight * right;
	position += horizontalBackForward * forward;

	verticalLeftRight = 0.0f;
	horizontalBackForward = 0.0f;

	target = position + forward;
	view = XMMatrixLookAtLH(position, target, up);
	world = view * XMMatrixTranslationFromVector(position);
}

void Camera::SetProjection() {
	float screen = (float)engine.getSupportedResolution().Width / (float)engine.getSupportedResolution().Height;
	projection = XMMatrixPerspectiveFovLH(0.35f * XM_PI, screen, 0.3f, 1000.0f);
}
