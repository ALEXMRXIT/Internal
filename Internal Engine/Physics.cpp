#include "Physics.h"
#include "MeshComponent.h"

bool Physics::IntersectTriangle(XMVECTOR& triV1, XMVECTOR& triV2, XMVECTOR& triV3, XMVECTOR& point) {
    XMVECTOR cp1 = XMVector3Cross((triV3 - triV2), (point - triV2));
    XMVECTOR cp2 = XMVector3Cross((triV3 - triV2), (triV1 - triV2));
    if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0) {
        cp1 = XMVector3Cross((triV3 - triV1), (point - triV1));
        cp2 = XMVector3Cross((triV3 - triV1), (triV2 - triV1));
        if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0) {
            cp1 = XMVector3Cross((triV2 - triV1), (point - triV1));
            cp2 = XMVector3Cross((triV2 - triV1), (triV3 - triV1));
            return XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0;
        }
        else return false;
    }
    return false;
}

Physics::Physics() { 
    m_vertices.clear();
    m_indices.clear();
}

float Physics::IntersectRayWithMesh(const XMVECTOR& rayOrigin, const XMVECTOR& rayDirection, MeshComponent& mesh) {
    DWORD dwNumFaces = static_cast<DWORD>(m_indices.size()) / 3;
    
    for (int iterator = 0; iterator < dwNumFaces; iterator++) {
        XMVECTOR tri1V1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        XMVECTOR tri1V2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        XMVECTOR tri1V3 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

        Vertex tV1, tV2, tV3;

        tV1 = m_vertices[m_indices[(iterator * 3) + 0]];
        tV2 = m_vertices[m_indices[(iterator * 3) + 1]];
        tV3 = m_vertices[m_indices[(iterator * 3) + 2]];

        tri1V1 = XMVectorSet(tV1.position.x, tV1.position.y, tV1.position.z, 0.0f);
        tri1V2 = XMVectorSet(tV2.position.x, tV2.position.y, tV2.position.z, 0.0f);
        tri1V3 = XMVectorSet(tV3.position.x, tV3.position.y, tV3.position.z, 0.0f);

        const XMMATRIX meshWorldPosition = mesh.gameObject().transform().GetWorldMatrix();
        tri1V1 = XMVector3TransformCoord(tri1V1, meshWorldPosition);
        tri1V2 = XMVector3TransformCoord(tri1V2, meshWorldPosition);
        tri1V3 = XMVector3TransformCoord(tri1V3, meshWorldPosition);

        XMVECTOR U = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        XMVECTOR V = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        XMVECTOR faceNormal = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

        U = tri1V2 - tri1V1;
        V = tri1V3 - tri1V1;

        faceNormal = XMVector3Cross(U, V);
        faceNormal = XMVector3Normalize(faceNormal);

        XMVECTOR triPoint = tri1V1;
        float tri1A = XMVectorGetX(faceNormal);
        float tri1B = XMVectorGetY(faceNormal);
        float tri1C = XMVectorGetZ(faceNormal);
        float tri1D = (-tri1A * XMVectorGetX(triPoint) - tri1B * XMVectorGetY(triPoint) - tri1C * XMVectorGetZ(triPoint));

        float ep1, ep2, t = 0.0f;
        float planeIntersectX, planeIntersectY, planeIntersectZ = 0.0f;
        XMVECTOR pointInPlane = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

        ep1 = (XMVectorGetX(rayOrigin) * tri1A) + (XMVectorGetY(rayOrigin) * tri1B) + (XMVectorGetZ(rayOrigin) * tri1C);
        ep2 = (XMVectorGetX(rayDirection) * tri1A) + (XMVectorGetY(rayDirection) * tri1B) + (XMVectorGetZ(rayDirection) * tri1C);

        if (ep2 != 0.0f) t = -(ep1 + tri1D) / (ep2);

        if (t > 0.0f) {
            planeIntersectX = XMVectorGetX(rayOrigin) + XMVectorGetX(rayDirection) * t;
            planeIntersectY = XMVectorGetY(rayOrigin) + XMVectorGetY(rayDirection) * t;
            planeIntersectZ = XMVectorGetZ(rayOrigin) + XMVectorGetZ(rayDirection) * t;

            pointInPlane = XMVectorSet(planeIntersectX, planeIntersectY, planeIntersectZ, 0.0f);

            if (IntersectTriangle(tri1V1, tri1V2, tri1V3, pointInPlane))
                return t / 2.0f;
        }
    }
    return FLT_MAX;
}

void Physics::setVerticesPhysics(const std::vector<Vertex>& vertices) {
    m_vertices = vertices;
}

void Physics::setIndecesPhysics(const std::vector<DWORD>& indices) {
    m_indices = indices;
}