#include "Physics.h"
#include "MeshComponent.h"

bool Physics::IntersectTriangle(const XMVECTOR& orig, const XMVECTOR& dir,
    const XMVECTOR& v0, const XMVECTOR& v1, const XMVECTOR& v2)
{
    XMVECTOR edge1 = v1 - v0;
    XMVECTOR edge2 = v2 - v0;
    XMVECTOR pvec;
    pvec = XMVector3Cross(dir, edge2);

    float det = XMVector3Dot(edge1, pvec).m128_f32[0];

    XMVECTOR tvec;
    if (det > 0) tvec = orig - v0;
    else {
        tvec = v0 - orig;
        det = -det;
    }

    if (det < 0.0001f) return false;
    float u = XMVector3Dot(tvec, pvec).m128_f32[0];
    if (u < 0.0f || u > det)  return false;

    XMVECTOR qvec;
    qvec = XMVector3Cross(tvec, edge1);
    float v = XMVector3Dot(dir, qvec).m128_f32[0];
    if (v < 0.0f || u + v > det) return false;

    return true;
}

Physics::Physics() { }

bool Physics::IntersectRayWithMesh(const XMVECTOR& rayOrigin, const XMVECTOR& rayDirection, MeshComponent* mesh) {
    DWORD dwNumFaces = static_cast<DWORD>(m_indices.size()) / 3;
    for (DWORD i = 0; i < dwNumFaces; i++) {
        XMVECTOR v0 = XMLoadFloat3(&m_vertices[m_indices[3 * i + 0]].position);
        XMVECTOR v1 = XMLoadFloat3(&m_vertices[m_indices[3 * i + 1]].position);
        XMVECTOR v2 = XMLoadFloat3(&m_vertices[m_indices[3 * i + 2]].position);
        if (IntersectTriangle(rayOrigin, rayDirection, v0, v1, v2))
            return true;
    }
    return false;
}

void Physics::SetDataPhysics(const std::vector<_Vertex>& vertices, const std::vector<DWORD>& indices) {
    m_vertices = vertices;
    m_indices = indices;
}