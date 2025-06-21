#include "DirectionLight.h"
#include "debug.h"
#include "ImGUI/imgui_internal.h"
#include "ViewProjectonData.h"
#include "GameObject.h"

DirectionLight::DirectionLight(GameObject* obj) : AbstractBaseComponent(obj) {
    m_directionBuffer = nullptr;

    m_directionOption.AmbiendColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
    m_directionOption.DirectionOption = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
}

HRESULT DirectionLight::Init(ID3D11Device* device) {
    HRESULT hr{};

    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(BufferDirectionOption);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    hr = device->CreateBuffer(&bufferDesc, NULL, &m_directionBuffer);
    if (FAILED(hr)) {
        DXUT_ERR_MSGBOX("Failed to create buffer.", hr);
        return hr;
    }

    const float dst = 250.0f;
    m_lightProjectionMatrix = XMMatrixOrthographicOffCenterLH(
        -dst, dst,
        -dst, dst,
        0.1f, 600.0f
    );

    m_device_loader = true;
    return hr;
}

void DirectionLight::Render(ID3D11DeviceContext* context) {
    context->UpdateSubresource(m_directionBuffer, 0, nullptr, &m_directionOption, 0, 0);
    context->PSSetConstantBuffers(0, 1, &m_directionBuffer);
}

void DirectionLight::Release() {
    if (m_directionBuffer) m_directionBuffer->Release();
}

BufferDirectionLight DirectionLight::UpdateMatrixByDirectionLight(XMMATRIX worldPos) {
    BufferDirectionLight buffer;
    const Transform* transform = (const Transform*)gameObject().GetComponentByType<Transform>();
    Quaternion rotation = transform->rotation();
    XMVECTOR direction = XMVector3Normalize(rotation.m_quat);

    XMMATRIX view = XMMatrixLookAtLH(XMVectorZero(), direction, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0));

    XMStoreFloat4(&buffer.direction, direction);
    buffer.lightViewProj = XMMatrixTranspose(worldPos * view * m_lightProjectionMatrix);
    return buffer;
}

XMMATRIX DirectionLight::GetViewProjectionMatrix() {
    if (!gameObject().IsStatic()) {
        const Transform* transform = (const Transform*)gameObject().GetComponentByType<Transform>();
        Quaternion rotation = transform->rotation();
        XMVECTOR direction = XMVector3Normalize(rotation.m_quat);

        XMMATRIX view = XMMatrixLookAtLH(XMVectorZero(), direction, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0));
        m_viewProjectionCache = view * m_lightProjectionMatrix;
    }
    return m_viewProjectionCache;
}

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
void DirectionLight::UpdateInterfaceInInspector(GameObject* gameObject) {

}
#endif