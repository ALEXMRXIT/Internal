#pragma once
#include "framework.h"
#include "Engine.h"

typedef struct _Vertex {
	XMFLOAT3 position;
	XMFLOAT2 texCoord;
	_Vertex(float x, float y, float z, float tx, float ty) :
		position(x, y, z), texCoord(tx, ty) {}
} Vertex, * LPVertex;

class VertexBuffer {
private:
	ID3D11Buffer* m_vertexBuffer;

public:
	VertexBuffer();
	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer& operator=(const VertexBuffer&) = delete;

	bool Create(ID3D11Device* device, ID3D11DeviceContext* context, void* pBuffer, unsigned int size);
	void Release();

	operator ID3D11Buffer*() const { return m_vertexBuffer; }
};

class IndexBuffer {
private:
	ID3D11Buffer* m_indexBuffer;

public:
	IndexBuffer();
	IndexBuffer(const IndexBuffer&) = delete;
	IndexBuffer& operator=(const IndexBuffer&) = delete;

	bool Create(ID3D11Device* device, ID3D11DeviceContext* context, void* pBuffer, unsigned int size);
	void Release();

	operator ID3D11Buffer* () const { return m_indexBuffer; }
};

class Mesh : public RenderObject {
private:
	VertexBuffer* m_vertexBuffer;
	IndexBuffer* m_indexBuffer;

	XMMATRIX Rotation;
	XMMATRIX Scale;
	XMMATRIX Translation;
	float rot;

public:
	Mesh();
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	void Render(MeshRenderData* renderData) override;

	bool CreateVertex(ID3D11Device* device, ID3D11DeviceContext* context, void* pBuffer, unsigned int size);
	bool CreateIndex(ID3D11Device* device, ID3D11DeviceContext* context, void* pBuffer, unsigned int size);

	void Release() override;
};