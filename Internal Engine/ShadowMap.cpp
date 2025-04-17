#include "ShadowMap.h"
#include "debug.h"
#include "GameObject.h"
#include "Model.h"
#include "Shader.h"
#include "DirectionLight.h"

void ShadowMap::Init(ID3D11Device* device) {
    m_width = 1024;
    m_height = 1024;

    D3D11_TEXTURE2D_DESC depthBufferDesc;
    ZeroMemory(&depthBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
    depthBufferDesc.Width = m_width;
    depthBufferDesc.Height = m_height;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;
    HRESULT hr = device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthTexture);
    if (FAILED(hr))
        DXUT_ERR_MSGBOX("Failed to create texture.", hr);

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;
    hr = device->CreateDepthStencilView(m_depthTexture, &dsvDesc, &m_shadowMapDepthStencilView);
    if (FAILED(hr))
        DXUT_ERR_MSGBOX("Failed to create depth stencil view.", hr);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = depthBufferDesc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    hr = device->CreateShaderResourceView(m_depthTexture, &srvDesc, &m_shadowMapSRV);
    if (FAILED(hr))
        DXUT_ERR_MSGBOX("Failed to create shared resource view.", hr);

    D3D11_RASTERIZER_DESC rasterDesc;
    ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.FrontCounterClockwise = FALSE;
    rasterDesc.DepthBias = 100;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.SlopeScaledDepthBias = 1.0f;
    rasterDesc.DepthClipEnable = TRUE;
    rasterDesc.ScissorEnable = FALSE;
    rasterDesc.MultisampleEnable = FALSE;
    rasterDesc.AntialiasedLineEnable = FALSE;
    hr = device->CreateRasterizerState(&rasterDesc, &m_pShadowRasterizerState);
    if (FAILED(hr))
        DXUT_ERR_MSGBOX("Failed to create rasterizer.", hr);

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
    hr = device->CreateInputLayout(layout, numElements, buffPtr, size, &m_pShadowLayout);
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Failed to create input layout.", hr);

    D3D11_BUFFER_DESC matrixBufferDesc;
    ZeroMemory(&matrixBufferDesc, sizeof(D3D11_BUFFER_DESC));
    matrixBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    matrixBufferDesc.ByteWidth = sizeof(XMMatrixCPerBuffer);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = 0;
    matrixBufferDesc.MiscFlags = 0;
    hr = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_constantBuffer);
    if (FAILED(hr))
        DXUT_ERR_MSGBOX("Failed to create matrix buffer.", hr);
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

    XMMatrixCPerBuffer view;
    const ViewProjectonData& viewLight = light->viewProjection();
    view.lightView = XMMatrixTranspose(viewLight.m_view * viewLight.m_projection);

    context->UpdateSubresource(m_constantBuffer, 0, nullptr, &view, 0, 0);
    context->VSSetConstantBuffers(0, 1, &m_constantBuffer);
}

void ShadowMap::Release() {
    if (m_shadowMapSRV) m_shadowMapSRV->Release();
    if (m_depthTexture) m_depthTexture->Release();
    if (m_shadowMapDepthStencilView) m_shadowMapDepthStencilView->Release();
}
