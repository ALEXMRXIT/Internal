#pragma once
#include "framework.h"

class Shader;
class Font;

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
	virtual void IASetVertexAndIndexBuffer(ID3D11DeviceContext* context) = 0;
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

	void IASetVertexAndIndexBuffer(ID3D11DeviceContext* context) {
		m_renderObject->IASetVertexAndIndexBuffer(context);
	}

	void Render() {
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
	Font* m_font;
	ID3D11InputLayout* m_layout;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11Texture2D* m_depthTexture;
	std::vector<RenderOperation*> m_quewe;
	ID3D11RasterizerState* m_cWcullMode;
	ID3D11BlendState* m_transparency;

	XMMATRIX camView;
	XMMATRIX camProjection;
	XMMATRIX m_viewProjectionMatrix;

	XMVECTOR camPosition;
	XMVECTOR camTarget;
	XMVECTOR camUp;
	ID3D11ShaderResourceView* m_sharedResourceView;

	static LRESULT WindowProcessor(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	Engine();
	~Engine();

	cbPerObject cbPerObj;
	ID3D11Buffer* m_preObjectBuffer;
	ID3D11SamplerState* m_textureSamplerState;

	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	bool InitWindowDevice(const WindowDescription* desc);
	bool InitRenderDevice();
	bool InitScene();
	void Update();
	void Render();
	void Release();
	int messageWindow();
	const WindowDescription* getWindowDesc() const;
} engine;