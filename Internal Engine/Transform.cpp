#include "Transform.h"

Transform::Transform() {
    Position = XMMatrixIdentity();
    Rotation = XMMatrixIdentity();
    Scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
}

Transform::Transform(const Transform& other) {
    *this = other;
}

Transform& Transform::operator=(const Transform& other) {
    if (this != &other) {
        Position = other.Position;
        Rotation = other.Rotation;
        Scale = other.Scale;
    }
    return *this;
}