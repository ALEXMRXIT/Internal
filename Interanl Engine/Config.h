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
	}

	int resolution;
	int multiply;
	int qualityTexture;
	bool fullscreen;
	int vSync;
	bool showCursor;
} config;