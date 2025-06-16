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
    XMFLOAT4 com;
    XMStoreFloat4(&com, quat);

    float sinr_cosp = 2.0f * (com.w * com.x + com.y * com.z);
    float cosr_cosp = 1.0f - 2.0f * (com.x * com.x + com.y * com.y);
    euler.x = std::atan2(sinr_cosp, cosr_cosp);

    float sinp = 2.0f * (com.w * com.y - com.z * com.x);
    if (std::abs(sinp) >= 1.0f)
        euler.y = std::copysign(XM_PI / 2.0f, sinp);
    else
        euler.y = std::asin(sinp);

    float siny_cosp = 2.0f * (com.w * com.z + com.x * com.y);
    float cosy_cosp = 1.0f - 2.0f * (com.y * com.y + com.z * com.z);
    euler.z = std::atan2(siny_cosp, cosy_cosp);

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