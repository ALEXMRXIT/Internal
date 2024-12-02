#include "Material.h"

MeshMaterial::MeshMaterial() {
	diffuseTex = nullptr;
}

void MeshMaterial::Bind(ID3D11DeviceContext* context) {
	context->PSSetShaderResources(0, 1, &diffuseTex->m_shaderView);
}

void MeshMaterial::Load(ID3D11Device* device) {
	diffuseTex->Load(device);
}

void MeshMaterial::Release() {
	if (diffuseTex) {
		diffuseTex->Release();
		delete diffuseTex;
	}
}
