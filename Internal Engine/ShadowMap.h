#pragma once
#include "framework.h"

class Shader;
class DirectionLight;

class ShadowMap {
private:
	ID3D11Texture2D* m_depthTexture = nullptr;
	ID3D11DepthStencilView* m_shadowMapDepthStencilView = nullptr;
	ID3D11RasterizerState* m_pShadowRasterizerState = nullptr;
	ID3D11ShaderResourceView* m_shadowMapSRV = nullptr;
	Shader* m_shadowShader = nullptr;
	ID3D11InputLayout* m_pShadowLayout = nullptr;
	D3D11_VIEWPORT m_viewport;

private:
	HRESULT CreateShadowMapViews(ID3D11Device* device);

public:
	ShadowMap() {  }

	ShadowMap(const ShadowMap&) = delete;
	ShadowMap& operator=(const ShadowMap&) = delete;

	HRESULT SaveShadowMapToFile(ID3D11Device* device, const char* filename);
	HRESULT LoadShadowMapFromFile(ID3D11Device* device, UINT width, UINT height, const char* filename);
	HRESULT RecreateShadowMapResources(ID3D11Device* device, UINT width, UINT height);

	void Init(ID3D11Device* device);

	void Render(ID3D11DeviceContext* context, DirectionLight* light);

	inline ID3D11ShaderResourceView* ShadowShaderResources() const { return m_shadowMapSRV; }

	void Release();
};