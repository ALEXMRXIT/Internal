#pragma once
class Config {
public:
	Config() {
		resolution = 2;
		multiply = 0;
		qualityTexture = 0;
		fullscreen = false;
		vSync = 1;
	}

	int resolution;
	int multiply;
	int qualityTexture;
	bool fullscreen;
	int vSync;
} config;