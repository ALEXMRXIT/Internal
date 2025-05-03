#pragma once
#include "framework.h"

class Shader;
class DirectionLight;

__declspec(align(16))
struct XMMatrixCPerBuffer {
	XMMATRIX lightView;
};

class ShadowMap {
private:
	UINT m_width, m_height = 0;
	ID3D11Texture2D* m_shadowMapTexture = nullptr;
	ID3D11Texture2D* m_depthTexture = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11DepthStencilView* m_shadowMapDepthStencilView = nullptr;
	ID3D11ShaderResourceView* m_shadowMapSRV = nullptr;
	ID3D11SamplerState* m_shadowMapSampler = nullptr;
	ID3D11Buffer* m_constantBuffer = nullptr;
	Shader* m_shadowShader = nullptr;
	ID3D11InputLayout* m_layout = nullptr;
	D3D11_VIEWPORT m_viewport;

public:
	ShadowMap();

	ShadowMap(const ShadowMap&) = delete;
	ShadowMap& operator=(const ShadowMap&) = delete;

	void Init(ID3D11Device* device);

	void Render(ID3D11DeviceContext* context, DirectionLight* light);

	void Release();
};