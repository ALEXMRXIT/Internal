#pragma once
#include "framework.h"

class Vector3 {
public:
    union {
        struct { float x, y, z; };
        float data[3];
    };

    static __forceinline Vector3 back() { return Vector3(0, 0, -1); }
    static __forceinline Vector3 down() { return Vector3(0, -1, 0); }
    static __forceinline Vector3 forward() { return Vector3(0, 0, 1); }
    static __forceinline Vector3 left() { return Vector3(-1, 0, 0); }
    static __forceinline Vector3 one() { return Vector3(1, 1, 1); }
    static __forceinline Vector3 right() { return Vector3(1, 0, 0); }
    static __forceinline Vector3 up() { return Vector3(0, 1, 0); }
    static __forceinline Vector3 zero() { return Vector3(0, 0, 0); }
    static __forceinline Vector3 negativeInfinity() {
        return Vector3(-std::numeric_limits<float>::infinity(),
            -std::numeric_limits<float>::infinity(),
            -std::numeric_limits<float>::infinity());
    }
    static __forceinline Vector3 positiveInfinity() {
        return Vector3(std::numeric_limits<float>::infinity(),
            std::numeric_limits<float>::infinity(),
            std::numeric_limits<float>::infinity());
    }

    __forceinline Vector3() : x(0), y(0), z(0) {}
    __forceinline Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    __forceinline float operator[](int index) const { return data[index]; }
    __forceinline float& operator[](int index) { return data[index]; }

    __forceinline bool operator==(const Vector3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    __forceinline bool operator!=(const Vector3& other) const {
        return !(*this == other);
    }

    __forceinline Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    __forceinline Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    __forceinline Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    __forceinline Vector3 operator/(float scalar) const {
        float inv = 1.0f / scalar;
        return Vector3(x * inv, y * inv, z * inv);
    }

    __forceinline Vector3& operator+=(const Vector3& other) {
        x += other.x; y += other.y; z += other.z;
        return *this;
    }

    __forceinline Vector3& operator-=(const Vector3& other) {
        x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }

    __forceinline Vector3& operator*=(float scalar) {
        x *= scalar; y *= scalar; z *= scalar;
        return *this;
    }

    __forceinline Vector3& operator/=(float scalar) {
        float inv = 1.0f / scalar;
        x *= inv; y *= inv; z *= inv;
        return *this;
    }

    __forceinline float Magnitude() const {
        return sqrtf(x * x + y * y + z * z);
    }

    __forceinline float SqrMagnitude() const {
        return x * x + y * y + z * z;
    }

    __forceinline XMVECTOR ToXMVector() const {
        return XMVectorSet(x, y, z, 0.0f);
    }

    __forceinline static Vector3 FromXMVector(XMVECTOR v) {
        Vector3 result;
        XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&result), v);
        return result;
    }

    __forceinline Vector3 Normalized() const {
        float mag = Magnitude();
        return mag > 0 ? *this * (1.0f / mag) : Vector3::zero();
    }

    __forceinline void Normalize() {
        float mag = Magnitude();
        if (mag > 0) {
            float inv = 1.0f / mag;
            x *= inv; y *= inv; z *= inv;
        }
    }

    __forceinline static float Dot(const Vector3& a, const Vector3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    __forceinline static Vector3 Cross(const Vector3& a, const Vector3& b) {
        return Vector3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }

    __forceinline static float Distance(const Vector3& a, const Vector3& b) {
        return (a - b).Magnitude();
    }

    __forceinline static Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
        return Vector3(
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t
        );
    }

    __forceinline static Vector3 MoveTowards(const Vector3& current, const Vector3& target, float maxDistanceDelta) {
        Vector3 delta = target - current;
        float mag = delta.Magnitude();
        return mag <= maxDistanceDelta || mag == 0 ? target : current + delta * (maxDistanceDelta / mag);
    }
};

__forceinline Vector3 operator*(float scalar, const Vector3& vec) {
    return vec * scalar;
}