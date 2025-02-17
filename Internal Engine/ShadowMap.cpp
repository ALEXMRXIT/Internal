#include "ShadowMap.h"
#include "debug.h"
#include "GameObject.h"
#include "Location.h"
#include "Model.h"
#include "Shader.h"
#include "DirectionLight.h"

ShadowMap::ShadowMap() {

}

void ShadowMap::Init(ID3D11Device* device) {
    m_width = 1024;
    m_height = 1024;

    D3D11_TEXTURE2D_DESC texDesc;
    ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
    texDesc.Width = m_width;
    texDesc.Height = m_height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;

    HRESULT hr = device->CreateTexture2D(&texDesc, nullptr, &m_shadowMapTexture);
    if (FAILED(hr))
        DXUT_ERR_MSGBOX("Failed to create texture.", hr);

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    hr = device->CreateDepthStencilView(m_shadowMapTexture, &dsvDesc, &m_shadowMapDepthStencilView);
    if (FAILED(hr))
        DXUT_ERR_MSGBOX("Failed to create depth stencil view.", hr);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;

    hr = device->CreateShaderResourceView(m_shadowMapTexture, &srvDesc, &m_shadowMapSRV);
    if (FAILED(hr))
        DXUT_ERR_MSGBOX("Failed to create shader resource view.", hr);

    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = device->CreateSamplerState(&samplerDesc, &m_shadowMapSampler);
    if (FAILED(hr))
        DXUT_ERR_MSGBOX("Failed to create shadow map sampler state.", hr);

    ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));
    m_viewport.TopLeftX = 0;
    m_viewport.TopLeftY = 0;
    m_viewport.Width = (FLOAT)m_width;
    m_viewport.Height = (FLOAT)m_height;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;

    m_shadowShader = new Shader();
    hr = m_shadowShader->LoadVertexShader(device, "VS", "shaders\\shadowmap.fx");
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Error loading vertex shader.", hr);
    hr = m_shadowShader->LoadPixelShader(device, "PS", "shaders\\shadowmap.fx");
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Error loading pixel shader.", hr);

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    UINT numElements = ARRAYSIZE(layout);

    LPVOID buffPtr = m_shadowShader->getVertexBlob()->GetBufferPointer();
    SIZE_T size = m_shadowShader->getVertexBlob()->GetBufferSize();
    hr = device->CreateInputLayout(layout, numElements, buffPtr, size, &m_layout);
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Failed to create input layout.", hr);

    D3D11_BUFFER_DESC matrixBufferDesc;
    ZeroMemory(&matrixBufferDesc, sizeof(D3D11_BUFFER_DESC));
    matrixBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    matrixBufferDesc.ByteWidth = sizeof(XMMatrixCPerBuffer);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = 0;
    matrixBufferDesc.MiscFlags = 0;

    hr = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_constantBuffer);
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Failed to create matrix buffer.", hr);
}

void ShadowMap::Render(ID3D11DeviceContext* context, DirectionLight* light) {
    context->RSSetViewports(1, &m_viewport);
    context->ClearDepthStencilView(m_shadowMapDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    context->OMSetRenderTargets(0, nullptr, m_shadowMapDepthStencilView);

    m_shadowShader->setVertexShader(context);
    m_shadowShader->setPiexlShader(context);
    context->IASetInputLayout(m_layout);

    XMMatrixCPerBuffer view;
    view.lightView = light->ProjectionLightView();

    context->UpdateSubresource(m_constantBuffer, 0, nullptr, &view, 0, 0);
    context->VSSetConstantBuffers(0, 1, &m_constantBuffer);

    ID3D11DepthStencilView* nullDSV = nullptr;
    context->OMSetRenderTargets(0, nullptr, nullDSV);

    ID3D11RenderTargetView* nullRTV = nullptr;
    context->OMSetRenderTargets(1, &nullRTV, nullptr);

    context->PSSetShaderResources(1, 1, &m_shadowMapSRV);

    context->PSSetSamplers(1, 1, &m_shadowMapSampler);
}

void ShadowMap::Release() {
    if (m_shadowMapSRV) m_shadowMapSRV->Release();
    if (m_shadowMapDepthStencilView) m_shadowMapDepthStencilView->Release();
    if (m_shadowMapTexture) m_shadowMapTexture->Release();
}
