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
	virtual void SetScale(XMFLOAT2 scale, XMFLOAT2 offset) = 0;

	struct TextureMapInfo {
		ID3D11ShaderResourceView* m_shaderView;
		ID3D11SamplerState* m_textureSamplerState;
		const char* name;
		void Load(ID3D11Device* device);
		void Release();
	};
};

class MeshMaterial : public Material {
private:
	XMFLOAT2 m_scale;
	XMFLOAT2 m_offset;

public:
	MeshMaterial();

	MeshMaterial(const MeshMaterial&) = delete;
	MeshMaterial& operator=(const MeshMaterial&) = delete;

	TextureMapInfo* diffuseTex;

	void Bind(ID3D11DeviceContext* context) override;
	void Load(ID3D11Device* device) override;
	void SetScale(XMFLOAT2 scale, XMFLOAT2 offset) override;

	XMFLOAT2 scale() const { return m_scale; }
	XMFLOAT2 offset() const { return m_offset; }

	void Release();
};