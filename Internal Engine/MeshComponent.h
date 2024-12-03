#pragma once
#include "framework.h"
#include "Engine.h"
#include "AbstractBaseComponent.h"

class MeshMaterial;

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

	bool Create(ID3D11Device* device, void* pBuffer, unsigned int size);
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

	bool Create(ID3D11Device* device, void* pBuffer, unsigned int size);
	void Release();

	operator ID3D11Buffer*() const { return m_indexBuffer; }
};

typedef struct _worldViewProjectionBuffer {
	XMMATRIX WVP;
} WorldViewProjection, * LPWorldViewProjection;

class MeshComponent : public AbstractBaseComponent {
private:
	VertexBuffer* m_vertexBuffer;
	IndexBuffer* m_indexBuffer;
	ID3D11RasterizerState* m_cWcullMode;
	MeshMaterial* m_material;
	ID3D11InputLayout* m_layout;
	Shader* m_meshShader;
	WorldViewProjection m_bufferWVP;
	ID3D11Buffer* m_preObjectBuffer;
	XMMATRIX m_position;

	void IASetVertexAndIndexBuffer(ID3D11DeviceContext* context);

public:
	MeshComponent();
	MeshComponent(const MeshComponent&) = delete;
	MeshComponent& operator=(const MeshComponent&) = delete;

	void Update(float deltaTime);
	void Render(ID3D11DeviceContext* context);

	void setPostion(XMMATRIX& position);

	HRESULT Init(ID3D11Device* device, ID3D11DeviceContext* context);

	bool CreateVertex(ID3D11Device* device, void* pBuffer, unsigned int size);
	bool CreateIndex(ID3D11Device* device, void* pBuffer, unsigned int size);

	void Release();
};