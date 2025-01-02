#include "PrimitiveDrawable.h"
#include "Shader.h"
#include "debug.h"
#include "Config.h"
#include "Camera.h"

PrimitiveDrawable::PrimitiveDrawable() {
    m_context = nullptr;
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;
    m_shader = nullptr;
    m_inputLayout = nullptr;
    m_preObjectBuffer = nullptr;
}

void PrimitiveDrawable::Init(ID3D11Device* device, ID3D11DeviceContext* context) {
    HRESULT hr{};
    D3D11_BUFFER_DESC descVertices;
    ZeroMemory(&descVertices, sizeof(D3D11_BUFFER_DESC));
    descVertices.Usage = D3D11_USAGE_DYNAMIC;
    descVertices.ByteWidth = sizeof(VertexLine) * config.primitiveBufferSize;
    descVertices.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    descVertices.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = device->CreateBuffer(&descVertices, nullptr, &m_vertexBuffer);
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Error create vertex buffer", hr);

    D3D11_BUFFER_DESC descIndex;
    ZeroMemory(&descIndex, sizeof(D3D11_BUFFER_DESC));
    descIndex.Usage = D3D11_USAGE_DYNAMIC;
    descIndex.ByteWidth = sizeof(DWORD) * config.primitiveBufferSize;
    descIndex.BindFlags = D3D11_BIND_INDEX_BUFFER;
    descIndex.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = device->CreateBuffer(&descIndex, nullptr, &m_indexBuffer);
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Error create index buffer", hr);

    m_shader = new Shader();
    hr = m_shader->LoadVertexShader(device, "VS", "shaders\\primitive.fx");
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Error loaded vertex shader", hr);
    hr = m_shader->LoadPixelShader(device, "PS", "shaders\\primitive.fx");
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Error loaded pixel shader", hr);

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    UINT numElements = ARRAYSIZE(layout);

    LPVOID buffPtr = m_shader->getVertexBlob()->GetBufferPointer();
    SIZE_T size = m_shader->getVertexBlob()->GetBufferSize();
    hr = device->CreateInputLayout(layout, numElements, buffPtr, size, &m_inputLayout);
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Failed to create input layout.", hr);

    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(cBuffePrimitive);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    hr = device->CreateBuffer(&bufferDesc, NULL, &m_preObjectBuffer);
    if (FAILED(hr)) DXUT_ERR_MSGBOX("Failed to create buffer.", hr);

    m_context = context;
}

void PrimitiveDrawable::UpdateVertexBuffer(const std::vector<VertexLine>& vertices) {
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (FAILED(m_context->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
        DXUT_ERR_MSGBOX("Error vertex map memory", GetLastError());
    memcpy(mappedResource.pData, vertices.data(), sizeof(Vertex) * vertices.size());
    m_context->Unmap(m_vertexBuffer, 0);
}

void PrimitiveDrawable::UpdateIndexBuffer(const std::vector<UINT>& indices) {
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (FAILED(m_context->Map(m_indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
        DXUT_ERR_MSGBOX("Error index map memory", GetLastError());
    memcpy(mappedResource.pData, indices.data(), sizeof(UINT) * indices.size());
    m_context->Unmap(m_indexBuffer, 0);
}

void PrimitiveDrawable::DrawLine(const XMFLOAT3& start, const XMFLOAT3& end, const XMFLOAT4& color) {
    m_vertices.push_back({ start, color });
    m_vertices.push_back({ end, color });

    m_indices.push_back(m_indices.size());
    m_indices.push_back(m_indices.size());

    UpdateVertexBuffer(m_vertices);
    UpdateIndexBuffer(m_indices);
}

void PrimitiveDrawable::GizmosClear() {
    m_vertices.clear();
    m_indices.clear();

    UpdateVertexBuffer(m_vertices);
    UpdateIndexBuffer(m_indices);
}

void PrimitiveDrawable::Render() {
    if (!m_vertices.size())
        return;

    XMStoreFloat4x4(&m_bufferWVP.WVP, XMMatrixTranspose(camera.getView() * camera.getProjection()));
    m_context->UpdateSubresource(m_preObjectBuffer, 0, nullptr, &m_bufferWVP, 0, 0);
    m_context->VSSetConstantBuffers(0, 1, &m_preObjectBuffer);

    UINT stride = sizeof(VertexLine);
    UINT offset = 0;
    m_context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    m_context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    m_context->IASetInputLayout(m_inputLayout);

    m_shader->setVertexShader(m_context);
    m_shader->setPiexlShader(m_context);

    m_context->DrawIndexed(m_indices.size(), 0, 0);
}

void PrimitiveDrawable::Release() {
    if (m_vertexBuffer) m_vertexBuffer->Release();
    if (m_indexBuffer) m_indexBuffer->Release();
    if (m_shader) {
        m_shader->Release();
        delete m_shader;
    }
    if (m_inputLayout) m_inputLayout->Release();
    m_vertices.clear();
    m_indices.clear();
    if (m_preObjectBuffer) m_preObjectBuffer->Release();
}