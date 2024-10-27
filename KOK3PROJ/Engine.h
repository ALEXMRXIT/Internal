#pragma once
#include "framework.h"

class Shader;

typedef struct _WindowDescription {
	const char* title;
	unsigned int width;
	unsigned int height;
	HINSTANCE hInstance;
	HWND hWnd;
	bool fullScreen;
	int nCmdShow;
} WindowDescription, *LPWindowDescription;

struct cbPerObject {
	XMMATRIX  WVP;
};

class MeshRenderData {
public:
	XMMATRIX m_transformMatrix;
};

class RenderObject {
public:
	virtual void Render(MeshRenderData* renderData) = 0;
	virtual void Release() = 0;
};

class RenderOperation {
public:
	MeshRenderData* m_renderData;
	RenderObject* m_renderObject;

	void CreateRenderOperation() {
		MeshRenderData* data = new MeshRenderData();
		data->m_transformMatrix = XMMatrixIdentity();
		m_renderData = data;
	}

	RenderOperation* SetRenderOperation(RenderObject* obj) {
		m_renderObject = obj;
		return this;
	}

	void UpdateMesh() {
		m_renderObject->Render(m_renderData);
	}

	void Release() {
		delete m_renderData;
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
	ID3D11InputLayout* m_layout;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11Texture2D* m_depthTexture;
	ID3D11Buffer* m_preObjectBuffer;
	std::vector<RenderOperation*> m_quewe;

	XMMATRIX camView;
	XMMATRIX camProjection;

	XMVECTOR camPosition;
	XMVECTOR camTarget;
	XMVECTOR camUp;
	cbPerObject cbPerObj;
	ID3D11ShaderResourceView* CubesTexture;
	ID3D11SamplerState* CubesTexSamplerState;

	static LRESULT WindowProcessor(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	Engine();
	~Engine();

	XMMATRIX m_viewProjectionMatrix;

	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	bool InitWindowDevice(const WindowDescription* desc);
	bool InitRenderDevice();
	bool InitScene();
	void Update();
	void Render();
	void Release();
	int messageWindow();
} engine;