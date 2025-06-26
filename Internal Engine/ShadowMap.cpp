#include "ShadowMap.h"
#include "debug.h"
#include "GameObject.h"
#include "Model.h"
#include "Shader.h"
#include "DirectionLight.h"

HRESULT ShadowMap::SaveShadowMapToFile(ID3D11Device* device, const char* filename) {
    HRESULT result{};
    D3D11_TEXTURE2D_DESC desc;
    m_depthTexture->GetDesc(&desc);

    ID3D11DeviceContext* context = nullptr;
    device->GetImmediateContext(&context);
    context->Flush();

    desc.Usage = D3D11_USAGE_STAGING;
    desc.BindFlags = 0;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

    ID3D11Texture2D* stagingTex = nullptr;
    result = device->CreateTexture2D(&desc, nullptr, &stagingTex);
    if (FAILED(result))
        DXUT_ERR_MSGBOX("Failed to create texture.", result);
    context->CopyResource(stagingTex, m_depthTexture);

    D3D11_MAPPED_SUBRESOURCE mapped;
    result = context->Map(stagingTex, 0, D3D11_MAP_READ, 0, &mapped);
    if (FAILED(result)) {
        stagingTex->Release();
        DXUT_ERR_MSGBOX("Failed to map texture.", result);
        return result;
    }

    FILE* file = fopen(filename, "wb");
    if (!file) {
        context->Unmap(stagingTex, 0);
        stagingTex->Release();
        DXUT_ERR_MSGBOX("Failed to open file for writing.", E_FAIL);
        return E_FAIL;
    }

    const size_t dataSize = (size_t)desc.Width * desc.Height * 4;
    size_t bytesWritten = fwrite((const void*)mapped.pData, 1, dataSize, file);
    if (bytesWritten != dataSize) {
        result = E_FAIL;
        DXUT_ERR_MSGBOX("Failed to write all texture data.", result);
    }

    fclose(file);
    context->Unmap(stagingTex, 0);
    stagingTex->Release();
    return result;
}

HRESULT ShadowMap::LoadShadowMapFromFile(ID3D11Device* device, UINT width, UINT height, const char* filename) {
    HRESULT result{};
    FILE* file = fopen(filename, "rb");
    if (!file) {
        DXUT_ERR_MSGBOX("Failed to open file for reading.", E_FAIL);
        return E_FAIL;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    const size_t expectedSize = width * height * 4;
    if (fileSize != expectedSize) {
        fclose(file);
        DXUT_ERR_MSGBOX("File size doesn't match texture dimensions.", E_FAIL);
        return E_FAIL;
    }

    BYTE* data = new BYTE[fileSize];
    size_t bytesRead = fread((void*)data, 1, fileSize, file);
    fclose(file);

    if (bytesRead != fileSize) {
        DXUT_ERR_MSGBOX("Failed to read all texture data.", E_FAIL);
        return E_FAIL;
    }

    if (m_depthTexture) m_depthTexture->Release();
    if (m_shadowMapDepthStencilView) m_shadowMapDepthStencilView->Release();
    if (m_shadowMapSRV) m_shadowMapSRV->Release();

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R32_TYPELESS;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = data;
    initData.SysMemPitch = width * 4;

    result = device->CreateTexture2D(&desc, &initData, &m_depthTexture);
    if (FAILED(result))
        DXUT_ERR_MSGBOX("Failed to create texture from file.", result);

    result = CreateShadowMapViews(device);

    return result;
}

HRESULT ShadowMap::CreateShadowMapViews(ID3D11Device* device) {
    HRESULT result{};

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;
    result = device->CreateDepthStencilView(m_depthTexture, &dsvDesc, &m_shadowMapDepthStencilView);
    if (FAILED(result))
        DXUT_ERR_MSGBOX("Failed to create depth stencil view.", result);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;
    result = device->CreateShaderResourceView(m_depthTexture, &srvDesc, &m_shadowMapSRV);
    if (FAILED(result))
        DXUT_ERR_MSGBOX("Failed to create shared resource view.", result);

    return result;
}

HRESULT ShadowMap::RecreateShadowMapResources(ID3D11Device* device, UINT width, UINT height) {
    HRESULT result{};

    ID3D11DeviceContext* context = nullptr;
    device->GetImmediateContext(&context);
    context->Flush();

    if (m_depthTexture) m_depthTexture->Release();
    if (m_shadowMapDepthStencilView) m_shadowMapDepthStencilView->Release();
    if (m_shadowMapSRV) m_shadowMapSRV->Release();

    D3D11_TEXTURE2D_DESC depthBufferDesc;
    ZeroMemory(&depthBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
    depthBufferDesc.Width = width;
    depthBufferDesc.Height = height;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;
    result = device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthTexture);
    if (FAILED(result))
        DXUT_ERR_MSGBOX("Failed to create texture.", result);

    result = CreateShadowMapViews(device);

    ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));
    m_viewport.TopLeftX = 0;
    m_viewport.TopLeftY = 0;
    m_viewport.Width = (FLOAT)width;
    m_viewport.Height = (FLOAT)height;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;

    return result;
}

void ShadowMap::Init(ID3D11Device* device, DirectionLight* light) {
    XMFLOAT2 shadowSize = light->GetShadowSize();
    HRESULT hr = RecreateShadowMapResources(device, shadowSize.x, shadowSize.y);

    D3D11_RASTERIZER_DESC rasterDesc;
    ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.FrontCounterClockwise = FALSE;
    rasterDesc.DepthBias = 1000;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.SlopeScaledDepthBias = 2.0f;
    rasterDesc.DepthClipEnable = TRUE;
    rasterDesc.ScissorEnable = FALSE;
    rasterDesc.MultisampleEnable = FALSE;
    rasterDesc.AntialiasedLineEnable = FALSE;
    hr = device->CreateRasterizerState(&rasterDesc, &m_pShadowRasterizerState);
    if (FAILED(hr))
        DXUT_ERR_MSGBOX("Failed to create rasterizer.", hr);

    m_shadowShader = new Shader();
    hr = m_shadowShader->LoadVertexShader(device, "VS", "shaders\\shadowmap.fx");
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Error loading vertex shader.", hr);

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    UINT numElements = ARRAYSIZE(layout);

    LPVOID buffPtr = m_shadowShader->getVertexBlob()->GetBufferPointer();
    SIZE_T size = m_shadowShader->getVertexBlob()->GetBufferSize();
    hr = device->CreateInputLayout(layout, numElements, buffPtr, size, &m_pShadowLayout);
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Failed to create input layout.", hr);
}

void ShadowMap::Render(ID3D11DeviceContext* context, DirectionLight* light) {
    ID3D11ShaderResourceView* nullSRVs[2] = { nullptr, nullptr };
    context->PSSetShaderResources(1, 2, nullSRVs);
    context->OMSetRenderTargets(0, nullptr, m_shadowMapDepthStencilView);
    context->ClearDepthStencilView(m_shadowMapDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    context->RSSetViewports(1, &m_viewport);

    context->IASetInputLayout(m_pShadowLayout);
    m_shadowShader->setVertexShader(context);
    context->PSSetShader(nullptr, nullptr, 0);

    context->RSSetState(m_pShadowRasterizerState);
}

void ShadowMap::Release() {
    if (m_shadowMapSRV) m_shadowMapSRV->Release();
    if (m_depthTexture) m_depthTexture->Release();
    if (m_shadowMapDepthStencilView) m_shadowMapDepthStencilView->Release();
    if (m_pShadowRasterizerState) m_pShadowRasterizerState->Release();
    if (m_shadowShader) {
        m_shadowShader->Release();
        delete m_shadowShader;
    }
    if (m_pShadowLayout) m_pShadowLayout->Release();
}
