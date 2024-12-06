#include "Skybox.h"
#include "Shader.h"
#include "debug.h"

void Skybox::CreateSphere(int lat, int longL) {

}

void Skybox::Init(ID3D11Device* device, ID3D11DeviceContext* context) {
	HRESULT hr{};
    m_shader = new Shader();
    hr = m_shader->LoadVertexShader(device, context, "shaders\\skybox.fx");
    if (FAILED(hr))
        DXUT_ERR_MSGBOX("Failed to load Vertex shader.", hr);
    hr = m_shader->LoadPixelShader(device, context, "shaders\\skybox.fx");
    if (FAILED(hr))
        DXUT_ERR_MSGBOX("Failed to load Pixel shader.", hr);
    
    D3DX11_IMAGE_LOAD_INFO loadSMInfo;
    loadSMInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

    ID3D11Texture2D* texture = nullptr;
    hr = D3DX11CreateTextureFromFile(device, "skymap.dds", &loadSMInfo, 0, (ID3D11Resource**)&texture, 0);
    if (FAILED(hr))
        DXUT_ERR_MSGBOX("Error loading skybox texture.", hr);

    D3D11_TEXTURE2D_DESC textureDesc;
    texture->GetDesc(&textureDesc);

    D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
    ZeroMemory(&viewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    viewDesc.Format = textureDesc.Format;
    viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    viewDesc.TextureCube.MipLevels = textureDesc.MipLevels;
    viewDesc.TextureCube.MostDetailedMip = 0;

    hr = device->CreateShaderResourceView(texture, &viewDesc, &m_sharedView);
    if (FAILED(hr))
        DXUT_ERR_MSGBOX("Error create shared resources view.", hr);
}

void Skybox::Update(float deltaTime) {

}

void Skybox::Render(ID3D11DeviceContext* context) {

}
