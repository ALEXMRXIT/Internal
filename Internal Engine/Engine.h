#pragma once
#include "framework.h"
#include "Config.h"
#include "Camera.h"

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
#include "ImGUIDevice.h"
class ImGUIDevice;
#endif

class Shader;
class Font;
class Model;
class Location;
class Skybox;
class GameObject;
class PrimitiveDrawable;
class ShadowMap;

typedef struct _windowDescription {
	const char* title;
	HINSTANCE hInstance;
	DWORD windowStyle;
	HWND hWnd;
	LONG width;
	LONG height;
	int nCmdShow;
} WindowDescription, *LPWindowDescription;

typedef struct _perfomanceTimeInfo {
	LARGE_INTEGER frequency;
	LARGE_INTEGER start;
	LARGE_INTEGER end;
	float deltaTime;
	int fps;
	int frameCount;
	float elapsedTime;
	float targetFrameTime;
	float accumulator;
} PerfomanceTimeInfo, *LPPerfomanceTimeInfo;

typedef struct _multisampleityLevel {
	uint32_t SampleCount;
	uint32_t QualityLevel;

	_multisampleityLevel(uint32_t count, uint32_t quality) :
		SampleCount(count), QualityLevel(quality) { };
} MultisampleQualityLevel, LPMultisampleQualityLevel;

class Engine {
private:
	WindowDescription* m_windowDesc = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11DepthStencilView* m_depthStencilView = nullptr;
	ID3D11Texture2D* m_depthTexture = nullptr;
	ID3D11BlendState* m_blending = nullptr;
	ID3D11RasterizerState* m_cWcullMode = nullptr;
	ID3D11InputLayout* m_layout = nullptr;
	IDirectInputDevice8* m_keyboard = nullptr;
	IDirectInputDevice8* m_mouse = nullptr;
	LPDIRECTINPUT8 m_directInput = nullptr;
	ID3D11Texture2D* m_renderTexture = nullptr;
	ID3D11ShaderResourceView* m_renderTextureSRV = nullptr;
	ID3D11RenderTargetView* m_renderTextureRTV = nullptr;
	D3D11_VIEWPORT m_viewport;
#ifdef INTERNAL_ENGINE_GUI_INTERFACE
	ImGUIDevice* m_gui = nullptr;
	XMFLOAT2 m_scenePos;
	XMFLOAT2 m_sceneSize;
#endif

	std::vector<Model*> m_meshes;
	std::vector<MultisampleQualityLevel> m_qualityLevels;
	std::vector<DXGI_MODE_DESC> m_supportedResolution;
	Location* m_location = nullptr;
	Shader* m_meshShader = nullptr;

	PerfomanceTimeInfo m_timeInfo;
	Font* m_font = nullptr;

	bool m_SwapChainOccluded;
	bool m_raycastEnabled;
	bool m_debugRaycast;

private:
	static LRESULT windowProcessor(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void UpdateFrequenceTime(PerfomanceTimeInfo& timeInfo) const;
	HRESULT BuildMultiSampleQualityList(DXGI_FORMAT format);
	HRESULT InitDirectInput(HINSTANCE hInstance);
	void UpdateInput(float deltaTime);
	void CreateRenderTexture(int width, int height);
public:
	Engine();

	GameObject* lastSelected = nullptr;
	static Camera* m_camera;

	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	bool InitWindowDevice(const WindowDescription* desc);
	bool InitRenderDevice();
	HRESULT GetSupportedResolutions(DXGI_FORMAT format = DXGI_FORMAT_R16G16B16A16_FLOAT);
	bool InitScene();
	void FixedUpdate(float deltaTime);
	void Update(float deltaTime);
	void Render();
	void Release();
	int messageWindow();
	void Raycast(int mouseX, int mouseY);
	void addMeshRenderer(Model* mesh);
	void setFullScreen(HWND hwnd, bool fullscreen);
	const WindowDescription* getWindowDesc() const;
	IDXGISwapChain* getChain() const;
	const DXGI_MODE_DESC& getSupportedResolution() const;
	const wchar_t* toStringVSync() const;

	static Camera& main_camera() { return *m_camera; }

	ID3D11Device* device() const { return m_device; }
	ID3D11DeviceContext* deviceContext() const { return m_deviceContext; }
	ID3D11ShaderResourceView* GetRenderTextureSRV() const { return m_renderTextureSRV; }
	Location* location() const { return m_location; }
	void setRaycast(bool status) { m_raycastEnabled = status; }
	void setDebugRaycast(bool status) { m_debugRaycast = status; }
	bool getRaycast() const { return m_raycastEnabled; }
#ifdef INTERNAL_ENGINE_GUI_INTERFACE
	void setMousePosition(XMFLOAT2 scenePos, XMFLOAT2 sceneSize) { m_scenePos = scenePos; m_sceneSize = sceneSize; }
	__forceinline ImGUIDevice& gui() { return *m_gui; }
#endif
};

extern Engine engine;
extern Config config;
extern PrimitiveDrawable gizmozRect;
extern ShadowMap shadowMap;