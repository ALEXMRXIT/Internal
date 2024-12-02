#pragma once
#include "framework.h"
#include "PrimitiveType.h"

class Shader;
class Font;
class GameObject;
class Camera;

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

struct cbPerObject {
	XMMATRIX  WVP;
};

class InterfaceRenderObject {
public:
	virtual void Update(float deltaTime) = 0;
	virtual void Render(ID3D11DeviceContext* context) = 0;
	virtual void Release() = 0;
};

class RenderOperation {
public:
	InterfaceRenderObject* m_renderObject;

	RenderOperation* SetRenderOperation(InterfaceRenderObject* obj) {
		m_renderObject = obj;
		return this;
	}

	void Update(float deltaTime) {
		m_renderObject->Update(deltaTime);
	}

	void Render(ID3D11DeviceContext* context) {
		m_renderObject->Render(context);
	}

	void Release() {
		if (m_renderObject) m_renderObject->Release();
	}
};

typedef struct _multisampleityLevel {
	uint32_t SampleCount;
	uint32_t QualityLevel;

	_multisampleityLevel(uint32_t count, uint32_t quality) :
		SampleCount(count), QualityLevel(quality) { };
} MultisampleQualityLevel, LPMultisampleQualityLevel;

extern class Engine {
private:
	WindowDescription* m_windowDesc;
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	Shader* m_shader;
	Font* m_font;
	ID3D11InputLayout* m_layout;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11Texture2D* m_depthTexture;
	std::vector<RenderOperation*> m_quewe;
	ID3D11BlendState* m_transparency;
	std::vector<MultisampleQualityLevel> m_qualityLevels;
	std::vector<DXGI_MODE_DESC> m_supportedResolution;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;
	LPDIRECTINPUT8 m_directInput;

	PerfomanceTimeInfo m_timeInfo;

	static LRESULT windowProcessor(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void UpdateFrequenceTime(PerfomanceTimeInfo& timeInfo) const;
	HRESULT BuildMultiSampleQualityList(DXGI_FORMAT format);
	HRESULT InitDirectInput(HINSTANCE hInstance);
	void UpdateInput(float deltaTime);
public:
	Engine();
	~Engine();

	cbPerObject cbPerObj;
	ID3D11Buffer* m_preObjectBuffer;
	ID3D11SamplerState* m_textureSamplerState;
	XMMATRIX camProjection;

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
	void setFullScreen(HWND hwnd, bool fullscreen);
	const WindowDescription* getWindowDesc() const;
	RECT& getWindowRect();
	IDXGISwapChain* getChain() const;
	const DXGI_MODE_DESC& getSupportedResolutin() const;

	static GameObject* Instantiate(primitive_type_e type, XMVECTOR position);
} engine;