#include "Quaternion.h"

Quaternion::Quaternion() : m_quat(DirectX::XMQuaternionIdentity()) {}
Quaternion::Quaternion(float x, float y, float z, float w) : m_quat(DirectX::XMVectorSet(
    XMConvertToRadians(x), 
    XMConvertToRadians(y),
    XMConvertToRadians(z),
    XMConvertToRadians(w)
)) {  }
Quaternion::Quaternion(const DirectX::XMVECTOR& q) : m_quat(XMQuaternionNormalize(q)) {}

Quaternion Quaternion::Identity() { return Quaternion(); }

Quaternion SERVLIBCALL Quaternion::AngleAxis(float angleDegrees, const DirectX::XMFLOAT3 axis) {
    return Quaternion(DirectX::XMQuaternionRotationAxis(
        DirectX::XMLoadFloat3(&axis),
        DirectX::XMConvertToRadians(angleDegrees)
    ));
}

float SERVLIBCALL Quaternion::Angle(const Quaternion& a, const Quaternion& b) {
    float dot = fminf(fabsf(Dot(a, b)), 1.0f);
    return DirectX::XMConvertToDegrees(2.0f * acosf(dot));
}

float SERVLIBCALL Quaternion::Dot(const Quaternion& a, const Quaternion& b) {
    return DirectX::XMVectorGetX(DirectX::XMQuaternionDot(a.m_quat, b.m_quat));
}

Quaternion SERVLIBCALL Quaternion::Lerp(const Quaternion& a, const Quaternion& b, float t) {
    return Quaternion(DirectX::XMQuaternionNormalize(
        DirectX::XMVectorLerp(a.m_quat, b.m_quat, t)
    ));
}

Quaternion SERVLIBCALL Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float t) {
    return Quaternion(DirectX::XMQuaternionSlerp(a.m_quat, b.m_quat, t));
}

Quaternion SERVLIBCALL Quaternion::LookRotation(const DirectX::XMFLOAT3& forward, const DirectX::XMFLOAT3& up) {
    DirectX::XMMATRIX mat = DirectX::XMMatrixLookToRH(
        DirectX::XMVectorZero(),
        DirectX::XMLoadFloat3(&forward),
        DirectX::XMLoadFloat3(&up)
    );
    return Quaternion(DirectX::XMQuaternionRotationMatrix(mat));
}

SERVLIBCALL XMFLOAT4 Quaternion::QuaternionToDirection(const Quaternion& quat) {
    XMVECTOR quatNorm = XMQuaternionNormalize(quat.m_quat);
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR direction = XMVector3Rotate(up, quatNorm);

    XMFLOAT4 lightDirection;
    XMStoreFloat4(&lightDirection, direction);
    return lightDirection;
}

Quaternion Quaternion::Normalized() const {
    return Quaternion(DirectX::XMQuaternionNormalize(m_quat));
}

void Quaternion::Normalize() {
    m_quat = DirectX::XMQuaternionNormalize(m_quat);
}

Quaternion Quaternion::Inverse() const {
    return Quaternion(DirectX::XMQuaternionInverse(m_quat));
}

void SERVLIBCALL Quaternion::ToAngleAxis(float& angleDegrees, DirectX::XMFLOAT3& axis) const {
    DirectX::XMVECTOR axisVec;
    float angle;
    DirectX::XMQuaternionToAxisAngle(&axisVec, &angle, m_quat);
    angleDegrees = DirectX::XMConvertToDegrees(angle);
    DirectX::XMStoreFloat3(&axis, axisVec);
}

void SERVLIBCALL Quaternion::SetLookRotation(const DirectX::XMFLOAT3& forward, const DirectX::XMFLOAT3& up) {
    *this = LookRotation(forward, up);
}

void SERVLIBCALL Quaternion::RotateTowards(const Quaternion& target, float maxDegreesDelta) {
    float angle = Angle(*this, target);
    if (angle <= 0.001f) return;
    float t = fminf(1.0f, maxDegreesDelta / angle);
    *this = Slerp(*this, target, t);
}

Quaternion& Quaternion::operator=(const Quaternion& other) {
    if (this != &other)
        m_quat = other.m_quat;
    return *this;
}

Quaternion Quaternion::operator*(const Quaternion& rhs) const {
    return Quaternion(DirectX::XMQuaternionMultiply(m_quat, rhs.m_quat));
}

bool Quaternion::operator==(const Quaternion& rhs) const {
    return DirectX::XMQuaternionEqual(m_quat, rhs.m_quat);
}

bool Quaternion::operator!=(const Quaternion& rhs) const {
    return !(*this == rhs);
}

Quaternion Quaternion::operator^(const Quaternion& quat) const {
    return Quaternion(XMQuaternionMultiply(this->m_quat, quat.m_quat));
}

float Quaternion::X() const { return DirectX::XMVectorGetX(m_quat); }
float Quaternion::Y() const { return DirectX::XMVectorGetY(m_quat); }
float Quaternion::Z() const { return DirectX::XMVectorGetZ(m_quat); }
float Quaternion::W() const { return DirectX::XMVectorGetW(m_quat); }

void Quaternion::Set(float x, float y, float z, float w) {
    m_quat = DirectX::XMVectorSet(x, y, z, w);
}

float Quaternion::operator[](int index) const {
    return DirectX::XMVectorGetByIndex(m_quat, index);
}