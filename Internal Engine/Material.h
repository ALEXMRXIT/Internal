#pragma once
#include "framework.h"
#include "AbstractBaseComponent.h"

class Material {
public:
	Material() = default;
	~Material() = default;

	Material(const Material&) = delete;
	Material& operator=(const Material&) = delete;

	virtual void Bind(ID3D11DeviceContext* context) = 0;
	virtual void Load(ID3D11Device* device) = 0;
	virtual void SetScale(XMFLOAT2 scale, XMFLOAT2 offset) = 0;

	struct TextureMapInfo {
		ID3D11ShaderResourceView* m_shaderView;
		ID3D11SamplerState* m_textureSamplerState;
		ID3D11SamplerState* m_shadowSamplerState;
		char* name;
		void Load(ID3D11Device* device);
		void Release();
	};
};

class MeshMaterial : public Material, public AbstractBaseComponent {
public:
	__declspec(align(16))
	struct MeshMaterialBuffer {
		XMFLOAT4 SpecularColor;
		float SpecularPower;
		float SpecularIntensity;
		float padding[2];
	};

private:
	XMFLOAT2 m_scale;
	XMFLOAT2 m_offset;
	XMFLOAT4 m_color;

	ID3D11Buffer* m_meshMaterialBuffer;
	MeshMaterialBuffer m_buffer;

public:
	MeshMaterial(GameObject* obj);

	MeshMaterial(const MeshMaterial&) = delete;
	MeshMaterial& operator=(const MeshMaterial&) = delete;

	TextureMapInfo* diffuseTex;

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
	void UpdateInterfaceInInspector(GameObject* gameObject) override;
#endif

	void Bind(ID3D11DeviceContext* context) override;
	void Load(ID3D11Device* device) override;
	void SetScale(XMFLOAT2 scale, XMFLOAT2 offset) override;

	void setColor(XMFLOAT4 color) { m_color = color; }
	void setScale(XMFLOAT2 scale) { m_scale = scale; }
	void setOffset(XMFLOAT2 offset) { m_offset = offset; }

	XMFLOAT2 scale() const { return m_scale; }
	XMFLOAT2 offset() const { return m_offset; }
	XMFLOAT4 color() const { return m_color; }

	void Release();
};