#pragma once
class Config {
public:
	Config() {
		resolution = 0;
		multiply = 0;
		qualityTexture = 0;
		fullscreen = true;
	}

	int resolution;
	int multiply;
	int qualityTexture;
	bool fullscreen;
} config;