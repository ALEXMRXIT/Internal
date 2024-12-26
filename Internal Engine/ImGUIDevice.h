#ifdef INTERNAL_ENGINE_GUI_INTERFACE
#pragma once
#include "Engine.h"
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_dx11.h"
#include "ImGUI/imgui_impl_win32.h"

class ImGUIDevice {
private:
	void InitWindowStyle(void);

public:
	ImGUIDevice();

	ImGUIDevice(const ImGUIDevice&) = delete;
	ImGUIDevice* operator=(const ImGUIDevice&) = delete;

	void Init(ID3D11Device* device, ID3D11DeviceContext* context);
	void Render();

	void Release();
};
#endif