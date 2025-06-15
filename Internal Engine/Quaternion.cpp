#include "Quaternion.h"

Quaternion::Quaternion() : quat(DirectX::XMQuaternionIdentity()) {}
Quaternion::Quaternion(float x, float y, float z, float w) : quat(DirectX::XMVectorSet(x, y, z, w)) {}
Quaternion::Quaternion(const DirectX::XMVECTOR& q) : quat(XMQuaternionNormalize(q)) {}

Quaternion Quaternion::Identity() { return Quaternion(); }

Quaternion Quaternion::EulerAngles(float pitch, float yaw, float roll) {
    return Quaternion(DirectX::XMQuaternionRotationRollPitchYaw(
        DirectX::XMConvertToRadians(pitch),
        DirectX::XMConvertToRadians(yaw),
        DirectX::XMConvertToRadians(roll)
    ));
}

Quaternion Quaternion::AngleAxis(float angleDegrees, const DirectX::XMFLOAT3& axis) {
    return Quaternion(DirectX::XMQuaternionRotationAxis(
        DirectX::XMLoadFloat3(&axis),
        DirectX::XMConvertToRadians(angleDegrees)
    ));
}

float Quaternion::Angle(const Quaternion& a, const Quaternion& b) {
    float dot = fminf(fabsf(Dot(a, b)), 1.0f);
    return DirectX::XMConvertToDegrees(2.0f * acosf(dot));
}

float Quaternion::Dot(const Quaternion& a, const Quaternion& b) {
    return DirectX::XMVectorGetX(DirectX::XMQuaternionDot(a.quat, b.quat));
}

Quaternion Quaternion::Lerp(const Quaternion& a, const Quaternion& b, float t) {
    return Quaternion(DirectX::XMQuaternionNormalize(
        DirectX::XMVectorLerp(a.quat, b.quat, t)
    ));
}

Quaternion Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float t) {
    return Quaternion(DirectX::XMQuaternionSlerp(a.quat, b.quat, t));
}

Quaternion Quaternion::LookRotation(const DirectX::XMFLOAT3& forward, const DirectX::XMFLOAT3& up) {
    DirectX::XMMATRIX mat = DirectX::XMMatrixLookToRH(
        DirectX::XMVectorZero(),
        DirectX::XMLoadFloat3(&forward),
        DirectX::XMLoadFloat3(&up)
    );
    return Quaternion(DirectX::XMQuaternionRotationMatrix(mat));
}

XMFLOAT3 Quaternion::ToEulerAngles() const {
    XMFLOAT3 euler;
    XMVECTOR q = quat;

    float sinp = 2.0f * (XMVectorGetW(q) * XMVectorGetY(q) - XMVectorGetZ(q) * XMVectorGetX(q));
    if (fabs(sinp) >= 0.9999f) {
        euler.x = XMConvertToDegrees(XM_PIDIV2 * sinp);
        euler.y = XMConvertToDegrees(atan2f(
            2.0f * (XMVectorGetX(q) * XMVectorGetY(q) + XMVectorGetW(q) * XMVectorGetZ(q)),
            XMVectorGetW(q) * XMVectorGetW(q) + XMVectorGetX(q) * XMVectorGetX(q) - XMVectorGetY(q) * XMVectorGetY(q) - XMVectorGetZ(q) * XMVectorGetZ(q)));
        euler.z = 0.0f;
    }
    else {
        euler.x = XMConvertToDegrees(asinf(sinp));
        euler.y = XMConvertToDegrees(atan2f(
            2.0f * (XMVectorGetW(q) * XMVectorGetZ(q) + XMVectorGetX(q) * XMVectorGetY(q)),
            1.0f - 2.0f * (XMVectorGetY(q) * XMVectorGetY(q) + XMVectorGetZ(q) * XMVectorGetZ(q))));
        euler.z = XMConvertToDegrees(atan2f(
            2.0f * (XMVectorGetW(q) * XMVectorGetX(q) + XMVectorGetY(q) * XMVectorGetZ(q)),
            1.0f - 2.0f * (XMVectorGetX(q) * XMVectorGetX(q) + XMVectorGetY(q) * XMVectorGetY(q))));
    }

    return euler;
}

Quaternion Quaternion::Normalized() const {
    return Quaternion(DirectX::XMQuaternionNormalize(quat));
}

void Quaternion::Normalize() {
    quat = DirectX::XMQuaternionNormalize(quat);
}

Quaternion Quaternion::Inverse() const {
    return Quaternion(DirectX::XMQuaternionInverse(quat));
}

void Quaternion::ToAngleAxis(float& angleDegrees, DirectX::XMFLOAT3& axis) const {
    DirectX::XMVECTOR axisVec;
    float angle;
    DirectX::XMQuaternionToAxisAngle(&axisVec, &angle, quat);
    angleDegrees = DirectX::XMConvertToDegrees(angle);
    DirectX::XMStoreFloat3(&axis, axisVec);
}

void Quaternion::SetLookRotation(const DirectX::XMFLOAT3& forward, const DirectX::XMFLOAT3& up) {
    *this = LookRotation(forward, up);
}

void Quaternion::RotateTowards(const Quaternion& target, float maxDegreesDelta) {
    float angle = Angle(*this, target);
    if (angle <= 0.001f) return;
    float t = fminf(1.0f, maxDegreesDelta / angle);
    *this = Slerp(*this, target, t);
}

XMFINLINE Quaternion& Quaternion::operator=(const Quaternion& other) {
    if (this != &other)
        quat = other.quat;
    return *this;
}

Quaternion Quaternion::operator*(const Quaternion& rhs) const {
    return Quaternion(DirectX::XMQuaternionMultiply(quat, rhs.quat));
}

bool Quaternion::operator==(const Quaternion& rhs) const {
    return DirectX::XMQuaternionEqual(quat, rhs.quat);
}

bool Quaternion::operator!=(const Quaternion& rhs) const {
    return !(*this == rhs);
}

float Quaternion::X() const { return DirectX::XMVectorGetX(quat); }
float Quaternion::Y() const { return DirectX::XMVectorGetY(quat); }
float Quaternion::Z() const { return DirectX::XMVectorGetZ(quat); }
float Quaternion::W() const { return DirectX::XMVectorGetW(quat); }

void Quaternion::Set(float x, float y, float z, float w) {
    quat = DirectX::XMVectorSet(x, y, z, w);
}

float Quaternion::operator[](int index) const {
    return DirectX::XMVectorGetByIndex(quat, index);
}