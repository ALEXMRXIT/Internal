#ifdef INTERNAL_ENGINE_GUI_INTERFACE
#pragma once
#include "Engine.h"
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_dx11.h"
#include "ImGUI/imgui_impl_win32.h"

class GameObject;

class ImGUIDevice {
private:
	bool m_styleSelectedState;
	int m_selectedStyle;
	float m_fontSize;

private:
	void InitWindowStyle(void);

	void DefaultStyle(void);
	void BlueStyle(void);
	void BlackStyle(void);
	void WhiteStyle(void);

	bool DisplayHierarchy(GameObject* parent);

public:
	ImGUIDevice();

	ImGUIDevice(const ImGUIDevice&) = delete;
	ImGUIDevice* operator=(const ImGUIDevice&) = delete;

	void Init(ID3D11Device* device, ID3D11DeviceContext* context);
	void Render();

	void Release();
};
#endif