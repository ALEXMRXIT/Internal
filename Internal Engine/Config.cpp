#include "Config.h"

Config::Config() {
	resolution = 7;
	multiply = 0;
	qualityTexture = 10;
	fullscreen = true;
	vSync = 0;
	showCursor = true;
	cameraSpeed = 50.0f;
	mouseIntensive = 25.0f;
	additionalLShiftSpeed = 3.0f;
	primitiveBufferSize = 512;
	debugRaycast = false;
}
