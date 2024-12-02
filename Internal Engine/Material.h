#pragma once
#include "framework.h"
#include "debug.h"

class Material {
public:
	Material() {}

	Material(const Material&) = delete;
	Material& operator=(const Material&) = delete;

	virtual void Bind(ID3D11DeviceContext* context) = 0;
	virtual void Load(ID3D11Device* device) = 0;

	struct TextureMapInfo {
		ID3D11ShaderResourceView* m_shaderView;
		const char* name;
		void Load(ID3D11Device* device) { 
			HRESULT hr = D3DX11CreateShaderResourceViewFromFile(device, name, NULL, NULL, &m_shaderView, NULL);
			if (FAILED(hr)) {
				DXUT_ERR_MSGBOX("Failed to load texture.", hr);
			}
		}
		void Release() {
			if (m_shaderView) m_shaderView->Release();
		}
	};
};

class MeshMaterial : public Material {
public:
	MeshMaterial();

	MeshMaterial(const MeshMaterial&) = delete;
	MeshMaterial& operator=(const MeshMaterial&) = delete;

	TextureMapInfo* diffuseTex;

	void Bind(ID3D11DeviceContext* context) override;
	void Load(ID3D11Device* device) override;

	void Release();
};