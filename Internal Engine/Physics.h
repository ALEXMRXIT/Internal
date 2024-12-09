#pragma once
#include "framework.h"

struct Triangle {
	XMVECTOR v0, v1, v2;
};

class MeshComponent;
struct _Vertex;

class Physics {
private:
	std::vector<Triangle> m_triangles;

	__forceinline bool IntersectRayWithTriangle(const XMVECTOR& rayOrigin,
		const XMVECTOR& rayDirection, const Triangle& triangle, float& t);

public:
	Physics();

	Physics(const Physics&) = delete;
	Physics& operator=(const Physics&) = delete;

	bool IntersectRayWithMesh(const XMVECTOR& rayOrigin, const XMVECTOR& rayDirection, MeshComponent* mesh);

	void GenerateTriangles(const std::vector<_Vertex>& vertices, const std::vector<DWORD>& indices);
};