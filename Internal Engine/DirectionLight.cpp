#include "DirectionLight.h"
#include "debug.h"
#include "ImGUI/imgui_internal.h"
#include "ViewProjectonData.h"

DirectionLight::DirectionLight() {
    m_transform = nullptr;
}

HRESULT DirectionLight::Init(ID3D11Device* device) {
    HRESULT hr{};
    const float dst = 250.0f;
    m_lightProjectionMatrix = XMMatrixOrthographicOffCenterLH(
        -dst, dst,
        -dst, dst,
        0.1f, 400.0f
    );

    m_device_loader = true;
    return hr;
}

void DirectionLight::Release() {

}

BufferDirectionLight DirectionLight::UpdateMatrixByDirectionLight(XMMATRIX worldPos) {
    XMFLOAT3 rotation = m_transform->rotation();
    XMVECTOR direction = XMLoadFloat3(&rotation);
    direction = XMVector3Equal(direction, XMVectorZero())
        ? XMVectorSet(0, 0, 1, 0)
        : XMVector3Normalize(direction);

    XMVECTOR lightPos = direction * 50.0f;
    XMMATRIX view = XMMatrixLookAtLH(lightPos, XMVectorZero(), XMVectorSet(0, 1, 0, 0));

    BufferDirectionLight buffer;
    buffer.direction = XMFLOAT4(XMVectorGetX(direction), XMVectorGetY(direction),
        XMVectorGetZ(direction), 1.0f);
    buffer.lightViewProj = XMMatrixTranspose(worldPos * view * m_lightProjectionMatrix);

    return buffer;
}

XMMATRIX DirectionLight::GetViewProjectionMatrix() {
    XMFLOAT3 rotation = m_transform->rotation();
    XMVECTOR direction = XMLoadFloat3(&rotation);
    direction = XMVector3Equal(direction, XMVectorZero())
        ? XMVectorSet(0, 0, 1, 0)
        : XMVector3Normalize(direction);

    XMVECTOR lightPos = direction * 50.0f;
    XMMATRIX view = XMMatrixLookAtLH(lightPos, XMVectorZero(), XMVectorSet(0, 1, 0, 0));

    return view * m_lightProjectionMatrix;
}

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
void DirectionLight::UpdateInterfaceInInspector(GameObject* gameObject) {

}
#endif

void DirectionLight::setMatrix(XMMATRIX& position) { }
