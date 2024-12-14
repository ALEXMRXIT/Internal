#pragma once
#include "framework.h"

class MeshComponent;
struct _Vertex;

class Physics {
private:
	std::vector<_Vertex> m_vertices;
	std::vector<DWORD> m_indices;

	__forceinline bool IntersectTriangle(const XMVECTOR& orig, const XMVECTOR& dir, 
		const XMVECTOR& v0, const XMVECTOR& v1, const XMVECTOR& v2, float& t);

public:
	Physics();

	Physics(const Physics&) = delete;
	Physics& operator=(const Physics&) = delete;

	bool IntersectRayWithMesh(const XMVECTOR& rayOrigin, const XMVECTOR& rayDirection, MeshComponent* mesh, float& closestDistance);

	void SetDataPhysics(const std::vector<_Vertex>& vertices, const std::vector<DWORD>& indices);
};