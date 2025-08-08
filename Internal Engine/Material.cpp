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
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    context->Map(m_meshMaterialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    MeshMaterialBuffer* material = (MeshMaterialBuffer*)mappedResource.pData;
    material->SpecularColor = m_buffer.SpecularColor;
    material->SpecularPower = m_buffer.SpecularPower;
    material->SpecularIntensity = m_buffer.SpecularIntensity;
    context->Unmap(m_meshMaterialBuffer, 0);

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
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.ByteWidth = sizeof(MeshMaterialBuffer);
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDesc.MiscFlags = 0;
        device->CreateBuffer(&bufferDesc, NULL, &m_meshMaterialBuffer);
    }

	diffuseTex->Load(device);
}

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
void MeshMaterial::UpdateInterfaceInInspector(GameObject* gameObject) {
    ImGui::Dummy(ImVec2(0.0f, 2.0f));
    if (ImGui::CollapsingHeader("Mesh Material")) {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.25f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

        const float itemSpacing = ImGui::GetStyle().ItemSpacing.y;
        const float framePadding = ImGui::GetStyle().FramePadding.y;
        const float itemHeight = ImGui::GetTextLineHeight() + framePadding * 2.0f + itemSpacing;
        const float totalHeight = 4 * itemHeight + ImGui::GetStyle().WindowPadding.y * 2.0f;

        ImGui::BeginChild("MeshMaterialBlock", ImVec2(0.0f, totalHeight + 10), true);
        {
            const float controlWidth = ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x;

            ImGui::Columns(2, "materialColumn", false);
            ImGui::SetColumnWidth(0, 150.0f);
            ImGui::SetColumnWidth(1, controlWidth);

            ImGui::Text("Diffuse Texture");
            ImGui::NextColumn();

            ImGui::BeginGroup();
            {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));

                ImVec2 fieldSize = ImVec2(ImGui::GetContentRegionAvail().x, 35);
                ImGui::BeginChild("##TextureField", fieldSize, true, ImGuiWindowFlags_NoScrollbar);
                {
                    static bool showAssetsWindow = false;
                    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                        showAssetsWindow = true;
                    if (showAssetsWindow) {
                        ImGui::Begin("Select object");
                        {
                            ImVec2 mainWindowSize = ImGui::GetMainViewport()->Size;
                            ImVec2 centerPos = ImVec2(
                                (mainWindowSize.x - 800.0f) * 0.5f,
                                (mainWindowSize.y - 600.0f) * 0.5f
                            );
                            ImGui::SetNextWindowPos(centerPos, ImGuiCond_FirstUseEver);
                            ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);

                            ImGUIDevice& gui = engine.gui();
                            for (int i = 0; i < gui.assets_count(); ++i) {
                                texture_assets_t* asset = &gui.assets()[i];
                                ImGui::PushID(i);

                                char name_utf8[MAX_NAME];
                                WideCharToMultiByte(CP_UTF8, 0, asset->name, -1, name_utf8, MAX_NAME, NULL, NULL);

                                ImGui::BeginGroup();
                                {
                                    if (ImGui::ImageButton((ImTextureID)asset->shaderView, ImVec2(128.0f, 128.0f)))
                                        setDiffuse(asset);
                                    float text_width = ImGui::CalcTextSize(name_utf8).x;
                                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (128.0f - text_width) * 0.5f);
                                    ImGui::Text("%s", name_utf8);
                                }
                                ImGui::EndGroup();

                                ImGui::PopID();
                                if (i < gui.assets_count() - 1) ImGui::SameLine();
                            }
                        }
                        ImGui::End();
                    }
                    if (diffuseTex->name) {
                        ImGui::Image((ImTextureID)diffuseTex->m_shaderView, ImVec2(20, 20));
                        ImGui::SameLine();

                        char name_utf8[MAX_NAME];
                        WideCharToMultiByte(CP_UTF8, 0, diffuseTex->name, -1, name_utf8, MAX_NAME, NULL, NULL);
                        ImGui::Text("%s", name_utf8);
                    }
                    else ImGui::TextDisabled("None (Texture)");

                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_ASSET")) {
                            IM_ASSERT(payload->DataSize == sizeof(texture_assets_t*));
                            texture_assets_t* texture = *(texture_assets_t**)payload->Data;
                            setDiffuse(texture);
                        }
                        ImGui::EndDragDropTarget();
                    }
                }
                ImGui::EndChild();

                ImGui::PopStyleColor();
                ImGui::PopStyleVar();
            }
            ImGui::EndGroup();

            ImGui::NextColumn();

            ImGui::Text("Specular Color");
            ImGui::NextColumn();
            ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_InputRGB;
            XMFLOAT4 specularColor = m_buffer.SpecularColor;
            ImGui::SetNextItemWidth(-1);
            if (ImGui::ColorEdit3("##SpecularColor", &specularColor.x, flags))
                m_buffer.SpecularColor = specularColor;
            ImGui::NextColumn();

            ImGui::Text("Specular Power");
            ImGui::NextColumn();
            ImGui::SetNextItemWidth(-1);
            ImGui::SliderFloat("##SpecularPower", &m_buffer.SpecularPower, 0.0f, 256.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::NextColumn();

            ImGui::Text("Specular Intensity");
            ImGui::NextColumn();
            ImGui::SetNextItemWidth(-1);
            ImGui::SliderFloat("##SpecularIntensity", &m_buffer.SpecularIntensity, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::NextColumn();
        }
        ImGui::EndChild();

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar();
    }
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

void MeshMaterial::setDiffuse(texture_assets_t* texture) {
    if (diffuseTex->m_shaderView) {
        diffuseTex->m_shaderView->Release();
        diffuseTex->m_shaderView = nullptr;
    }
    if (texture && texture->shaderView) {
        texture->shaderView->AddRef();
        diffuseTex->m_shaderView = texture->shaderView;
    }
    if (texture->name && wcslen(texture->name) > 0) {
        size_t len = wcslen(texture->name) + 1;
        diffuseTex->name = (wchar_t*)malloc(sizeof(wchar_t) * len);
        if (diffuseTex->name)
            wcscpy_s(diffuseTex->name, len, texture->name);
        else
            diffuseTex->name = nullptr;
    }
    else
        diffuseTex->name = nullptr;
    m_scale = XMFLOAT2(1.0f, 1.0f);
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
    else
        m_shaderView = LoadTexture(device, name);

    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));

    if (config.qualityTexture == 0)
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    else if (config.qualityTexture == 10)
        sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    else
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

    const int anisotropy = 1 + (config.qualityTexture * 2);
    sampDesc.MipLODBias = 0.0f;
    sampDesc.MaxAnisotropy = min(anisotropy, 16);
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    sampDesc.BorderColor[0] = sampDesc.BorderColor[1] = sampDesc.BorderColor[2] = sampDesc.BorderColor[3] = 0;
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

ID3D11ShaderResourceView* Material::TextureMapInfo::LoadTexture(ID3D11Device* device, const wchar_t* texture_name) {
    ID3D11ShaderResourceView* shaderView = NULL;
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
    HRESULT hr = D3DX11CreateShaderResourceViewFromFileW(device, texture_name, &loadInfo, NULL, &shaderView, NULL);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOXW(L"Failed to load texture: ", hr, texture_name);
        return NULL;
    }
    return shaderView;
}
