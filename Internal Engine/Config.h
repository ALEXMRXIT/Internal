#pragma once
class Config {
public:
	Config();

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
	bool debugRaycast;
};