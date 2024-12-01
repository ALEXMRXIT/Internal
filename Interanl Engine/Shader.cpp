#include "Shader.h"
#include "debug.h"

Shader::Shader() {
    m_vertexShader = nullptr;
    m_pixelShader = nullptr;
    m_vertexBlob = nullptr;
}

HRESULT Shader::LoadShader(const char* entryPoint, const char* shaderName, const char* profile, ID3D10Blob** blob) {
    HRESULT hr;
    ID3D10Blob* errorBlob;
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    flags |= D3DCOMPILE_DEBUG;
    flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    hr = D3DX11CompileFromFile(shaderName, NULL,
        NULL, entryPoint, profile, flags, 0, 0, blob, &errorBlob, 0);
    if (FAILED(hr)) {
        if (errorBlob) {
            DXUT_ERR_MSGBOX("Error compiling shader %s", hr, (char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        else DXUT_ERR_MSGBOX("Error compiling shader", hr);
    }
    return hr;
}

HRESULT Shader::LoadVertexShader(ID3D11Device* device, ID3D11DeviceContext* context, const char* shaderName) {
    ID3D10Blob* vertexShader = nullptr;
    HRESULT hr = LoadShader("VS", shaderName, "vs_4_0", &vertexShader);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Error compiling vertex shader", hr);
        return hr;
    }
    LPVOID buffPtr = vertexShader->GetBufferPointer();
    SIZE_T size = vertexShader->GetBufferSize();
    device->CreateVertexShader(buffPtr, size, NULL, &m_vertexShader);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create vertex shader.", hr);
        return hr;
    }
    context->VSSetShader(m_vertexShader, 0, 0);
    m_vertexBlob = vertexShader;
    return hr;
}

HRESULT Shader::LoadPixelShader(ID3D11Device* device, ID3D11DeviceContext* context, const char* shaderName) {
    ID3D10Blob* pixelShader = nullptr;
    HRESULT hr = LoadShader("PS", shaderName, "ps_4_0", &pixelShader);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Error compiling pixel shader", hr);
        return hr;
    }
    LPVOID buffPtr = pixelShader->GetBufferPointer();
    SIZE_T size = pixelShader->GetBufferSize();
    device->CreatePixelShader(buffPtr, size, NULL, &m_pixelShader);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create pixel shader.", hr);
        return hr;
    }
    context->PSSetShader(m_pixelShader, 0, 0);
    pixelShader->Release();
    return hr;
}

void Shader::Release() {
    if (m_vertexShader) m_vertexShader->Release();
    if (m_pixelShader) m_pixelShader->Release();
    if (m_vertexBlob) m_vertexBlob->Release();
}