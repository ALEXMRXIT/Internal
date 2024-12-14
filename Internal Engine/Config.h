#pragma once
class Config {
public:
	Config() {
		resolution = 2;
		multiply = 3;
		qualityTexture = 10;
		fullscreen = true;
		vSync = 1;
		showCursor = true;
		cameraSpeed = 15.0f;
		mouseIntensive = 25.0f;
		additionalLShiftSpeed = 3.0f;
		primitiveBufferSize = 512;
	}

	int resolution;
	int multiply;
	int qualityTexture;
	bool fullscreen;
	int vSync;
	bool showCursor;
	float cameraSpeed;
	float mouseIntensive;
	float additionalLShiftSpeed;
	int primitiveBufferSize;
};