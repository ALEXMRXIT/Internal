#pragma once
#include "framework.h"

class Shader;
class Font;

static uint32_t m_components_count = 0;

typedef struct _windowDescription {
	const char* title;
	unsigned int width;
	unsigned int height;
	HINSTANCE hInstance;
	HWND hWnd;
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

	PerfomanceTimeInfo m_timeInfo;

	XMVECTOR camPosition;
	XMVECTOR camTarget;
	XMVECTOR camUp;

	static LRESULT WindowProcessor(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void UpdateFrequenceTime(PerfomanceTimeInfo& timeInfo) const;
public:
	Engine();
	~Engine();

	cbPerObject cbPerObj;
	ID3D11Buffer* m_preObjectBuffer;
	ID3D11SamplerState* m_textureSamplerState;
	ID3D11RasterizerState* m_cWcullMode;
	XMMATRIX camView;
	XMMATRIX camProjection;

	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	bool InitWindowDevice(const WindowDescription* desc);
	bool InitRenderDevice();
	bool InitScene();
	void FixedUpdate(float deltaTime);
	void Update(float deltaTime);
	void Render();
	void Release();
	int messageWindow();
	const WindowDescription* getWindowDesc() const;
} engine;