#include "DirectionLight.h"
#include "debug.h"
#include "ImGUI/imgui_internal.h"
#include "ViewProjectonData.h"
#include "GameObject.h"

DirectionLight::DirectionLight(GameObject* obj) : AbstractBaseComponent(obj) {
}

HRESULT DirectionLight::Init(ID3D11Device* device) {
    HRESULT hr{};
    const float dst = 250.0f;
    m_lightProjectionMatrix = XMMatrixOrthographicOffCenterLH(
        -dst, dst,
        -dst, dst,
        0.1f, 600.0f
    );

    m_device_loader = true;
    return hr;
}

void DirectionLight::Release() {

}

BufferDirectionLight DirectionLight::UpdateMatrixByDirectionLight(XMMATRIX worldPos) {
    Quaternion rotation = gameObject().GetComponentByType<Transform>()->rotation();
    XMVECTOR direction = XMVector3Rotate(
        XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
        rotation.m_quat
    );

    if (XMVector3Equal(direction, XMVectorZero()))
        direction = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    else
        direction = XMVector3Normalize(direction);

    XMVECTOR lightPos = direction * 50.0f;
    XMMATRIX view = XMMatrixLookAtLH(lightPos, XMVectorZero(), XMVectorSet(0, 1, 0, 0));

    BufferDirectionLight buffer;
    XMStoreFloat4(&buffer.direction, direction);
    buffer.lightViewProj = XMMatrixTranspose(worldPos * view * m_lightProjectionMatrix);
    return buffer;
}

XMMATRIX DirectionLight::GetViewProjectionMatrix() {
    Quaternion rotation = gameObject().GetComponentByType<Transform>()->rotation();
    XMVECTOR direction = XMVector3Rotate(
        XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
        rotation.m_quat
    );

    if (XMVector3Equal(direction, XMVectorZero()))
        direction = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    else
        direction = XMVector3Normalize(direction);

    XMVECTOR lightPos = direction * 50.0f;
    XMMATRIX view = XMMatrixLookAtLH(lightPos, XMVectorZero(), XMVectorSet(0, 1, 0, 0));

    return view * m_lightProjectionMatrix;
}

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
void DirectionLight::UpdateInterfaceInInspector(GameObject* gameObject) {

}
#endif