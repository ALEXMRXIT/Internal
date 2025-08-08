#include "Camera.h"
#include "Engine.h"
#include "Quaternion.h"
#include "Component.h"
#include "GameObject.h"
#include "Vector3.h"
#include "Transform.h"

Camera::Camera(GameObject* obj) : AbstractBaseComponent(obj) {
	transform = gameObject().GetComponentByType<Transform>();

    const WindowDescription* wndDesc = engine.getWindowDesc();
    float screenAspect = wndDesc->width / wndDesc->height;
	projection = XMMatrixPerspectiveFovLH(D3DX_PI / 3.0f, screenAspect, 2.0f, 2000.0f);
}

void Camera::Update() {
    Vector3 pos = transform->position();
    Quaternion rot = transform->rotation();

    Vector3 forward = rot * Vector3::forward();
    Vector3 right = rot * Vector3::right();
    Vector3 up = Vector3::Cross(forward, right).Normalized();

    XMVECTOR positionVec = pos.ToXMVector();
    XMVECTOR targetVec = positionVec + forward.ToXMVector();
    view = XMMatrixLookAtLH(positionVec, targetVec, up.ToXMVector());
}

const XMVECTOR& Camera::getPos() const { return transform->position().ToXMVector(); }

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
void Camera::UpdateInterfaceInInspector(GameObject* gameObject) {

}
#endif
