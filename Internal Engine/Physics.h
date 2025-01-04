#pragma once
#include "framework.h"

class MeshComponent;
struct Vertex;

class Physics {
private:
	std::vector<Vertex> m_vertices;
	std::vector<DWORD> m_indices;

	__forceinline bool IntersectTriangle(XMVECTOR& triV1, XMVECTOR& triV2, XMVECTOR& triV3, XMVECTOR& point);

public:
	Physics();

	Physics(const Physics&) = delete;
	Physics& operator=(const Physics&) = delete;

	float IntersectRayWithMesh(const XMVECTOR& rayOrigin, const XMVECTOR& rayDirection, MeshComponent& mesh);

	void setVerticesPhysics(const std::vector<Vertex>& vertices);
	void setIndecesPhysics(const std::vector<DWORD>& indices);
};