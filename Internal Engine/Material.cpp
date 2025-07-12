#include "Material.h"
#include "Engine.h"
#include "ShadowMap.h"
#include "GameObject.h"
#include "debug.h"

MeshMaterial::MeshMaterial(GameObject* obj) : AbstractBaseComponent(obj) {
	diffuseTex = nullptr;
    m_meshMaterialBuffer = nullptr;

    m_buffer.SpecularColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    m_buffer.SpecularPower = 32.0f;
    m_buffer.SpecularIntensity = .5f;
}

void MeshMaterial::Bind(ID3D11DeviceContext* context) {
    context->UpdateSubresource(m_meshMaterialBuffer, 0, nullptr, &m_buffer, 0, 0);
    context->PSSetConstantBuffers(1, 1, &m_meshMaterialBuffer);
	context->PSSetShaderResources(0, 1, &diffuseTex->m_shaderView);
	ID3D11ShaderResourceView* shadowResourceView = shadowMap.ShadowShaderResources();
	context->PSSetShaderResources(1, 1, &shadowResourceView);
	context->PSSetSamplers(0, 1, &diffuseTex->m_textureSamplerState);
	context->PSSetSamplers(1, 1, &diffuseTex->m_shadowSamplerState);
}

void MeshMaterial::Load(ID3D11Device* device) {
    if (!m_meshMaterialBuffer) {
        D3D11_BUFFER_DESC bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.ByteWidth = sizeof(MeshMaterialBuffer);
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        device->CreateBuffer(&bufferDesc, NULL, &m_meshMaterialBuffer);
    }

	diffuseTex->Load(device);
}

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
void MeshMaterial::UpdateInterfaceInInspector(GameObject* gameObject) {

}
#endif

void MeshMaterial::SetScale(XMFLOAT2 scale, XMFLOAT2 offset) {
	m_scale = scale;
	m_offset = offset;
}

void MeshMaterial::Release() {
	if (diffuseTex) {
		diffuseTex->Release();
		delete diffuseTex;
	}
    if (m_meshMaterialBuffer) m_meshMaterialBuffer->Release();
}

inline void Material::TextureMapInfo::Load(ID3D11Device* device) {
    HRESULT hr{};
    if (!name) {
        D3D11_TEXTURE2D_DESC texDesc;
        ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
        texDesc.Width = 1;
        texDesc.Height = 1;
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 1;
        texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        texDesc.SampleDesc.Count = 1;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        uint32_t whitePixel = 0x00000000;
        D3D11_SUBRESOURCE_DATA initData;
        ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
        initData.pSysMem = &whitePixel;
        initData.SysMemPitch = 4;
        initData.SysMemSlicePitch = 4;

        ID3D11Texture2D* pTexture = nullptr;
        hr = device->CreateTexture2D(&texDesc, &initData, &pTexture);
        if (SUCCEEDED(hr)) {
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
            ZeroMemory(&srvDesc, sizeof(srvDesc));
            srvDesc.Format = texDesc.Format;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = 1;
            hr = device->CreateShaderResourceView(pTexture, &srvDesc, &m_shaderView);
            pTexture->Release();
            if (FAILED(hr)) {
                DXUT_ERR_MSGBOX("Failed to create default SRV.", hr);
                return;
            }
        }
        else {
            DXUT_ERR_MSGBOX("Failed to create default texture.", hr);
            return;
        }
    }
    else {
        if (m_shaderView) m_shaderView->Release();
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
        loadInfo.Format = DXGI_FORMAT_FROM_FILE;
        loadInfo.Filter = D3DX11_FILTER_LINEAR;
        loadInfo.MipFilter = D3DX11_FILTER_LINEAR;
        loadInfo.pSrcInfo = nullptr;

        HRESULT hr = D3DX11CreateShaderResourceViewFromFileW(device, name, &loadInfo, NULL, &m_shaderView, NULL);
        if (FAILED(hr)) {
            DXUT_ERR_MSGBOX("Failed to load texture.", hr);
            return;
        }
    }

    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));

    if (config.qualityTexture == 0)
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    else if (config.qualityTexture == 10)
        sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    else
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

    const int anisotropy = 1 + (config.qualityTexture * 2);
    sampDesc.MaxAnisotropy = min(anisotropy, 16);
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = device->CreateSamplerState(&sampDesc, &m_textureSamplerState);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create Sampler state.", hr);
    }

    D3D11_SAMPLER_DESC shadowSampDesc;
    ZeroMemory(&shadowSampDesc, sizeof(D3D11_SAMPLER_DESC));
    shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    shadowSampDesc.MaxAnisotropy = 1;
    shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    shadowSampDesc.BorderColor[0] = 1.0f;
    shadowSampDesc.BorderColor[1] = 1.0f;
    shadowSampDesc.BorderColor[2] = 1.0f;
    shadowSampDesc.BorderColor[3] = 1.0f;
    shadowSampDesc.MinLOD = 0;
    shadowSampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = device->CreateSamplerState(&shadowSampDesc, &m_shadowSamplerState);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create shadow Sampler state.", hr);
    }
}

inline void Material::TextureMapInfo::Release() {
    if (name) free(name);
	if (m_shaderView) m_shaderView->Release();
	if (m_textureSamplerState) m_textureSamplerState->Release();
}
