#ifdef INTERNAL_ENGINE_GUI_INTERFACE
#pragma once
#include "Engine.h"
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_dx11.h"
#include "ImGUI/imgui_impl_win32.h"

class GameObject;

#define MAX_NAME 256

enum AssetType {
	ASSET_TEXTURE,
	ASSET_MODEL_OBJ
};

typedef struct texture_assets_s {
	wchar_t name[MAX_NAME];
	wchar_t path[MAX_PATH];
	AssetType assetsType;
	ID3D11ShaderResourceView* shaderView;
	bool loaded;
} texture_assets_t, *LPTexture_assets_t;

class ImGUIDevice {
private:
	bool m_styleSelectedState;
	int m_selectedStyle;
	float m_fontSize;

	texture_assets_t* m_assets;
	int m_assets_count;

private:
	void InitWindowStyle(void);

	void DefaultStyle(void);
	void WhiteStyle(void);

	void CleanupTextures();

	bool DisplayHierarchy(GameObject* parent);

public:
	ImGUIDevice();

	ImGUIDevice(const ImGUIDevice&) = delete;
	ImGUIDevice* operator=(const ImGUIDevice&) = delete;

	void LoadTexture(ID3D11Device* device, const wchar_t* path);
	void LoadModel(ID3D11Device* device, const wchar_t* path);

	void Init(ID3D11Device* device, ID3D11DeviceContext* context);
	void Render();

	void Release();

	__forceinline texture_assets_t* assets() { return m_assets; }
	__forceinline int assets_count() const { return m_assets_count; }
};
#endif