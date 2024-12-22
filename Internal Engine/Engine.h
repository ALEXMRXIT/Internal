#pragma once
#include "framework.h"
#include "Config.h"

class Shader;
class Font;
class Camera;
class MeshComponent;
class Location;
class Skybox;
class PrimitiveDrawable;

typedef struct _windowDescription {
	const char* title;
	HINSTANCE hInstance;
	DWORD windowStyle;
	HWND hWnd;
	RECT rect;
	bool fullScreen;
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

typedef struct bufferLight {
	XMFLOAT4 direction;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
} BufferDirectionLight, *LPbufferDirectionLight;

class Engine {
private:
	WindowDescription* m_windowDesc;
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11Texture2D* m_depthTexture;
	ID3D11BlendState* m_transparency;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;
	LPDIRECTINPUT8 m_directInput;
	ID3D11Buffer* m_constantLightBuffer;

	std::vector<MeshComponent*> m_meshes;
	std::vector<MultisampleQualityLevel> m_qualityLevels;
	std::vector<DXGI_MODE_DESC> m_supportedResolution;
	Location* m_location;
	Skybox* m_skybox;
	BufferDirectionLight m_bufferLight;

	PerfomanceTimeInfo m_timeInfo;
	Font* m_font;

	static LRESULT windowProcessor(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void UpdateFrequenceTime(PerfomanceTimeInfo& timeInfo) const;
	HRESULT BuildMultiSampleQualityList(DXGI_FORMAT format);
	HRESULT InitDirectInput(HINSTANCE hInstance);
	void UpdateInput(float deltaTime);
public:
	Engine() {}

	MeshComponent* lastSelected;

	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	bool InitWindowDevice(const WindowDescription* desc);
	bool InitRenderDevice();
	HRESULT GetSupportedResolutions();
	bool InitScene();
	void FixedUpdate(float deltaTime);
	void Update(float deltaTime);
	void Render();
	void Release();
	int messageWindow();
	void Raycast();
	void addMeshRenderer(MeshComponent* mesh);
	void setFullScreen(HWND hwnd, bool fullscreen);
	const WindowDescription* getWindowDesc() const;
	RECT& getWindowRect();
	IDXGISwapChain* getChain() const;
	const DXGI_MODE_DESC& getSupportedResolution() const;
	const wchar_t* toStringVSync() const;

	ID3D11Device* device() const { return m_device; }
};

extern Engine engine;
extern Config config;
extern Camera camera;
extern PrimitiveDrawable draw;