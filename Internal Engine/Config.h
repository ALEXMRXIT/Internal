#pragma once
class Config {
public:
	Config() {
		resolution = 19;
		multiply = 3;
		qualityTexture = 10;
		fullscreen = true;
		vSync = 1;
		showCursor = true;
		cameraSpeed = 15.0f;
		mouseIntensive = 25.0f;
		additionalLShiftSpeed = 2.0f;
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
} config;