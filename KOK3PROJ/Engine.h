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

typedef struct _Vertex {
	XMFLOAT3 position;
	XMFLOAT2 texCoord;
	_Vertex(float x, float y, float z, float tx, float ty) : 
		position(x, y, z), texCoord(tx, ty) {}
} Vertex, *LPVertex;

class Engine {
private:
	WindowDescription* m_windowDesc;
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	Shader* m_shader;
	ID3D11Buffer* m_indexBuffer;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11InputLayout* m_layout;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11Texture2D* m_depthTexture;
	ID3D11Buffer* m_preObjectBuffer;

	XMMATRIX WVP;
	XMMATRIX cube1World;
	XMMATRIX cube2World;
	XMMATRIX World;
	XMMATRIX camView;
	XMMATRIX camProjection;

	XMVECTOR camPosition;
	XMVECTOR camTarget;
	XMVECTOR camUp;
	cbPerObject cbPerObj;
	XMMATRIX Rotation;
	XMMATRIX Scale;
	XMMATRIX Translation;
	float rot;
	ID3D11ShaderResourceView* CubesTexture;
	ID3D11SamplerState* CubesTexSamplerState;

	static LRESULT WindowProcessor(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	Engine();
	~Engine();

	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	bool InitWindowDevice(const WindowDescription* desc);
	bool InitRenderDevice();
	bool InitScene();
	void Update();
	void Render();
	void Release();
	int messageWindow();
};