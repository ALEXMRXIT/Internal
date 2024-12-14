#include "Physics.h"
#include "MeshComponent.h"

bool Physics::IntersectTriangle(const XMVECTOR& orig, const XMVECTOR& dir,
    const XMVECTOR& v0, const XMVECTOR& v1, const XMVECTOR& v2, float& t)
{
    // Вычисляем ребра треугольника
    XMVECTOR edge1 = v1 - v0;
    XMVECTOR edge2 = v2 - v0;

    // Вычисляем вектор pvec
    XMVECTOR pvec = XMVector3Cross(dir, edge2);

    // Вычисляем детерминант
    float det = XMVectorGetX(XMVector3Dot(edge1, pvec));

    // Если детерминант близок к нулю, луч параллелен треугольнику
    if (fabs(det) < 1e-6f) return false;

    // Вычисляем обратный детерминант
    float invDet = 1.0f / det;

    // Вычисляем tvec
    XMVECTOR tvec = orig - v0;

    // Вычисляем параметр u
    float u = XMVectorGetX(XMVector3Dot(tvec, pvec)) * invDet;
    if (u < 0.0f || u > 1.0f) return false;

    // Вычисляем qvec
    XMVECTOR qvec = XMVector3Cross(tvec, edge1);

    // Вычисляем параметр v
    float v = XMVectorGetX(XMVector3Dot(dir, qvec)) * invDet;
    if (v < 0.0f || u + v > 1.0f) return false;

    // Вычисляем параметр t (расстояние до точки пересечения)
    t = XMVectorGetX(XMVector3Dot(edge2, qvec)) * invDet;

    // Проверяем, что t положительно (пересечение впереди луча)
    return t >= 0.0f;
}

Physics::Physics() { }

bool Physics::IntersectRayWithMesh(const XMVECTOR& rayOrigin, const XMVECTOR& rayDirection, MeshComponent* mesh, float& closestDistance) {
    DWORD dwNumFaces = static_cast<DWORD>(m_indices.size()) / 3;
    bool intersected = false;
    closestDistance = FLT_MAX;

    for (DWORD i = 0; i < dwNumFaces; i++) {
        XMVECTOR v0 = XMLoadFloat3(&m_vertices[m_indices[3 * i + 0]].position);
        XMVECTOR v1 = XMLoadFloat3(&m_vertices[m_indices[3 * i + 1]].position);
        XMVECTOR v2 = XMLoadFloat3(&m_vertices[m_indices[3 * i + 2]].position);

        float t = 0.0f;
        if (IntersectTriangle(rayOrigin, rayDirection, v0, v1, v2, t)) {
            if (t < closestDistance) {
                closestDistance = t;
                intersected = true;
            }
        }
    }

    return intersected;
}

void Physics::SetDataPhysics(const std::vector<_Vertex>& vertices, const std::vector<DWORD>& indices) {
    m_vertices = vertices;
    m_indices = indices;
}