#include "Material.h"
#include "Engine.h"

MeshMaterial::MeshMaterial() {
	diffuseTex = nullptr;
}

void MeshMaterial::Bind(ID3D11DeviceContext* context) {
	context->PSSetShaderResources(0, 1, &diffuseTex->m_shaderView);
	context->PSSetSamplers(0, 1, &diffuseTex->m_textureSamplerState);
	
}

void MeshMaterial::Load(ID3D11Device* device) {
	diffuseTex->Load(device);
}

void MeshMaterial::SetScale(XMFLOAT2 scale, XMFLOAT2 offset) {
	m_scale = scale;
	m_offset = offset;
}

void MeshMaterial::Release() {
	if (diffuseTex) {
		diffuseTex->Release();
		delete diffuseTex;
	}
}

inline void Material::TextureMapInfo::Load(ID3D11Device* device) {
	D3DX11_IMAGE_LOAD_INFO loadInfo;
	ZeroMemory(&loadInfo, sizeof(D3DX11_IMAGE_LOAD_INFO));
	loadInfo.Width = D3DX11_DEFAULT;
	loadInfo.Height = D3DX11_DEFAULT;
	loadInfo.Depth = D3DX11_DEFAULT;
	loadInfo.FirstMipLevel = 0;
	loadInfo.MipLevels = D3DX11_DEFAULT;
	loadInfo.Usage = D3D11_USAGE_DEFAULT;
	loadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	loadInfo.CpuAccessFlags = 0;
	loadInfo.MiscFlags = 0;
	loadInfo.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
	loadInfo.Filter = D3DX11_FILTER_LINEAR;
	loadInfo.MipFilter = D3DX11_FILTER_LINEAR;
	loadInfo.pSrcInfo = nullptr;

	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(device, name, &loadInfo, NULL, &m_shaderView, NULL);
	if (FAILED(hr)) DXUT_ERR_MSGBOX("Failed to load texture.", hr);

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));
	if (config.qualityTexture == 0) sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	else if (config.qualityTexture == 10) sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	else sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	const int anisotropy = 1 + (config.qualityTexture * 2);
	sampDesc.MaxAnisotropy = min(anisotropy, 16);
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&sampDesc, &m_textureSamplerState);
	if (FAILED(hr)) DXUT_ERR_MSGBOX("Failed to create Sampler state.", hr);
}

inline void Material::TextureMapInfo::Release() {
	if (m_shaderView) m_shaderView->Release();
	if (m_textureSamplerState) m_textureSamplerState->Release();
}
