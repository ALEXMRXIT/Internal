#include "Physics.h"
#include "MeshComponent.h"

bool Physics::IntersectRayWithTriangle(const XMVECTOR& rayOrigin, 
    const XMVECTOR& rayDirection, const Triangle& triangle, float& t)
{
    const float EPSILON = 0.0000001f;
    XMVECTOR edge1 = triangle.v1 - triangle.v0;
    XMVECTOR edge2 = triangle.v2 - triangle.v0;

    XMVECTOR h = XMVector3Cross(rayDirection, edge2);
    float a = XMVectorGetX(XMVector3Dot(edge1, h));
    if (a > -EPSILON && a < EPSILON) return false;
    float f = 1.0f / a;
    XMVECTOR s = rayOrigin - triangle.v0;
    float u = f * XMVectorGetX(XMVector3Dot(s, h));
    if (u < 0.0f || u > 1.0f) return false;
    XMVECTOR q = XMVector3Cross(s, edge1);
    float v = f * XMVectorGetX(XMVector3Dot(rayDirection, q));
    if (v < 0.0f || u + v > 1.0f) return false;
    t = f * XMVectorGetX(XMVector3Dot(edge2, q));
    return t > EPSILON;
}

Physics::Physics() {
    m_triangles.clear();
}

bool Physics::IntersectRayWithMesh(const XMVECTOR& rayOrigin, const XMVECTOR& rayDirection, MeshComponent* mesh) {
    for (const Triangle& triangle : m_triangles) {
        float t;
        if (IntersectRayWithTriangle(rayOrigin, rayDirection, triangle, t))
            return true;
    }
    return false;
}

void Physics::GenerateTriangles(const std::vector<_Vertex>& vertices, const std::vector<DWORD>& indices) {
    for (size_t i = 0; i < indices.size(); i += 3) {
        Triangle triangle{};
        triangle.v0 = XMLoadFloat3(&vertices[indices[i]].position);
        triangle.v1 = XMLoadFloat3(&vertices[indices[i + 1]].position);
        triangle.v2 = XMLoadFloat3(&vertices[indices[i + 2]].position);
        m_triangles.push_back(triangle);
    }
}
