#pragma once
#include "framework.h"
#include "servlib.h"
#include "Vector3.h"

__declspec(align(16))
class Quaternion {
public:
    DirectX::XMVECTOR m_quat;

	Quaternion();
	explicit Quaternion(const DirectX::XMVECTOR& q);
	Quaternion(float x, float y, float z, float w);

    static Quaternion Identity();
    static SERVLIBCALL Quaternion AngleAxis(float angleDegrees, const Vector3 axis);
    static SERVLIBCALL float Angle(const Quaternion& a, const Quaternion& b);
    static SERVLIBCALL float Dot(const Quaternion& a, const Quaternion& b);
    static SERVLIBCALL Quaternion Lerp(const Quaternion& a, const Quaternion& b, float t);
    static SERVLIBCALL Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);
    static SERVLIBCALL Quaternion LookRotation(const DirectX::XMFLOAT3& forward, const DirectX::XMFLOAT3& up = { 0, 1, 0 });
    static SERVLIBCALL XMFLOAT4 QuaternionToDirection(const Quaternion& quat);
    static SERVLIBCALL XMVECTOR QuaternionToDirectionVector(const Quaternion& quat);
    static SERVLIBCALL Quaternion CreateFromYawPitchRoll(float yaw, float pitch, float roll);

    Quaternion Normalized() const;
    void Normalize();
    XMFLOAT3 ToEuler() const;
    Quaternion Inverse() const;
    void SERVLIBCALL ToAngleAxis(float& angleDegrees, DirectX::XMFLOAT3& axis) const;
    void SERVLIBCALL SetLookRotation(const DirectX::XMFLOAT3& forward, const DirectX::XMFLOAT3& up = { 0, 1, 0 });
    void SERVLIBCALL RotateTowards(const Quaternion& target, float maxDegreesDelta);

    Quaternion& operator=(const Quaternion& other);
    Quaternion operator*(const Quaternion& rhs) const;
    bool operator==(const Quaternion& rhs) const;
    bool operator!=(const Quaternion& rhs) const;
    Quaternion operator^(const Quaternion& quat) const;

    float X() const;
    float Y() const;
    float Z() const;
    float W() const;
    void Set(float x, float y, float z, float w);
    float operator[](int index) const;

    __forceinline Vector3 operator*(const Vector3& vec) const {
        DirectX::XMVECTOR v = vec.ToXMVector();

        DirectX::XMVECTOR result = DirectX::XMVector3Rotate(v, m_quat);
        return Vector3::FromXMVector(result);
    }

    __forceinline DirectX::XMVECTOR ToXMVector() const {
        return m_quat;
    }

    __forceinline static Quaternion FromXMVector(DirectX::XMVECTOR v) {
        Quaternion result;
        DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&result), v);
        return result;
    }
};