#pragma once
#include "framework.h"
#include "servlib.h"

__declspec(align(16))
class Quaternion {
public:
    DirectX::XMVECTOR m_quat;

	Quaternion();
	explicit Quaternion(const DirectX::XMVECTOR& q);
	Quaternion(float x, float y, float z, float w);

    static Quaternion Identity();
    static SERVLIBCALL Quaternion AngleAxis(float angleDegrees, const DirectX::XMFLOAT3 axis);
    static SERVLIBCALL float Angle(const Quaternion& a, const Quaternion& b);
    static SERVLIBCALL float Dot(const Quaternion& a, const Quaternion& b);
    static SERVLIBCALL Quaternion Lerp(const Quaternion& a, const Quaternion& b, float t);
    static SERVLIBCALL Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);
    static SERVLIBCALL Quaternion LookRotation(const DirectX::XMFLOAT3& forward, const DirectX::XMFLOAT3& up = { 0, 1, 0 });

    Quaternion Normalized() const;
    void Normalize();
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
};