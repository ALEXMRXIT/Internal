#include "Shader.h"
#include "debug.h"

Shader::Shader() {
    m_vertexShader = nullptr;
    m_pixelShader = nullptr;
    m_vertexBlob = nullptr;
}

HRESULT Shader::LoadShader(const char* entryPoint, const char* shaderName, const char* profile, ID3D10Blob** blob) {
    HRESULT handle{};
    ID3D10Blob* errorBlob;
    handle = D3DX11CompileFromFile(shaderName, NULL,
        NULL, entryPoint, profile, 0, 0, 0, blob, &errorBlob, 0);
    if (FAILED(handle)) {
        ERROR_MSG("Error loading shader: %s", (char*)errorBlob->GetBufferPointer());
        errorBlob->Release();
    }
    return handle;
}

HRESULT Shader::LoadVertexShader(ID3D11Device* device, ID3D11DeviceContext* context, const char* shaderName) {
    ID3D10Blob* vertexShader = nullptr;
    if (FAILED(LoadShader("VS", shaderName, "vs_4_0", &vertexShader))) {
        ERROR_MSG("Error compiling vertex shader");
        return E_FAIL;
    }
    LPVOID buffPtr = vertexShader->GetBufferPointer();
    SIZE_T size = vertexShader->GetBufferSize();
    HRESULT handle = device->CreateVertexShader(buffPtr, size, NULL, &m_vertexShader);
    if (FAILED(handle)) {
        ERROR_MSG("Failed to create vertex shader. %d error code.", handle);
        return handle;
    }
    context->VSSetShader(m_vertexShader, 0, 0);
    m_vertexBlob = vertexShader;
    return handle;
}

HRESULT Shader::LoadPixelShader(ID3D11Device* device, ID3D11DeviceContext* context, const char* shaderName) {
    ID3D10Blob* pixelShader = nullptr;
    if (FAILED(LoadShader("PS", shaderName, "ps_4_0", &pixelShader))) {
        ERROR_MSG("Error compiling pixel shader");
        return E_FAIL;
    }
    LPVOID buffPtr = pixelShader->GetBufferPointer();
    SIZE_T size = pixelShader->GetBufferSize();
    HRESULT handle = device->CreatePixelShader(buffPtr, size, NULL, &m_pixelShader);
    if (FAILED(handle)) {
        ERROR_MSG("Failed to create pixel shader. %d error code.", handle);
        return handle;
    }
    context->PSSetShader(m_pixelShader, 0, 0);
    pixelShader->Release();
    return handle;
}

void Shader::Release() {
    if (m_vertexShader) m_vertexShader->Release();
    if (m_pixelShader) m_pixelShader->Release();
    if (m_vertexBlob) m_vertexBlob->Release();
}