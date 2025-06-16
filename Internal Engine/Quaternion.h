#pragma once
#include "framework.h"

__declspec(align(16))
class Quaternion {
public:
    DirectX::XMVECTOR quat;

	Quaternion();
	explicit Quaternion(const DirectX::XMVECTOR& q);
	Quaternion(float x, float y, float z, float w);

    static Quaternion Identity();
    static Quaternion EulerAngles(float pitch, float yaw, float roll);
    static Quaternion AngleAxis(float angleDegrees, const DirectX::XMFLOAT3 axis);
    static float Angle(const Quaternion& a, const Quaternion& b);
    static float Dot(const Quaternion& a, const Quaternion& b);
    static Quaternion Lerp(const Quaternion& a, const Quaternion& b, float t);
    static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);
    static Quaternion LookRotation(const DirectX::XMFLOAT3& forward, const DirectX::XMFLOAT3& up = { 0, 1, 0 });

    XMFLOAT3 ToEulerAngles() const;
    Quaternion Normalized() const;
    void Normalize();
    Quaternion Inverse() const;
    void ToAngleAxis(float& angleDegrees, DirectX::XMFLOAT3& axis) const;
    void SetLookRotation(const DirectX::XMFLOAT3& forward, const DirectX::XMFLOAT3& up = { 0, 1, 0 });
    void RotateTowards(const Quaternion& target, float maxDegreesDelta);

    Quaternion& operator=(const Quaternion& other);
    Quaternion operator*(const Quaternion& rhs) const;
    bool operator==(const Quaternion& rhs) const;
    bool operator!=(const Quaternion& rhs) const;

    float X() const;
    float Y() const;
    float Z() const;
    float W() const;
    void Set(float x, float y, float z, float w);
    float operator[](int index) const;
};