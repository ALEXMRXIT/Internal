#pragma once
#include "Engine.h"
#include "AbstractBaseComponent.h"
#include "Physics.h"
#include "GameObject.h"

class MeshMaterial;

typedef struct _Vertex {
	XMFLOAT3 position;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
	_Vertex() : position(0.f, 0.f, 0.f), texCoord(0.f, 0.f), normal(0.f, 0.f, 0.f) {}

	bool operator==(const _Vertex& other) const {
        return position.x == other.position.x && position.y == other.position.y && position.z == other.position.z &&
			normal.x == other.normal.x && normal.y == other.normal.y && normal.z == other.normal.z &&
			texCoord.x == other.texCoord.x && texCoord.y == other.texCoord.y;
    }
} Vertex, *LPVertex;

class VertexBuffer {
private:
	ID3D11Buffer* m_vertexBuffer;

public:
	VertexBuffer();
	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer& operator=(const VertexBuffer&) = delete;

	bool Create(ID3D11Device* device, void* pBuffer, uint32_t sizeType, uint32_t size);
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

	bool Create(ID3D11Device* device, void* pBuffer, uint32_t sizeType, uint32_t size);
	void Release();

	operator ID3D11Buffer*() const { return m_indexBuffer; }
};

typedef struct _worldViewProjectionBuffer {
	XMMATRIX WVP;
	XMMATRIX World;
	XMFLOAT2 texture_scale;
	XMFLOAT2 texture_offset;
} WorldViewProjection, * LPWorldViewProjection;

class MeshComponent : public AbstractBaseComponent, public Physics {
private:
	VertexBuffer* m_vertexBuffer;
	IndexBuffer* m_indexBuffer;
	ID3D11RasterizerState* m_cWcullMode;
	MeshMaterial* m_material;
	ID3D11InputLayout* m_layout;
	Shader* m_meshShader;
	WorldViewProjection m_bufferWVP;
	ID3D11Buffer* m_preObjectBuffer;
	XMMATRIX* m_position;
	uint32_t indices;
	GameObject* m_obj;

	void IASetVertexAndIndexBuffer(ID3D11DeviceContext* context);

public:
	MeshComponent();
	MeshComponent(const MeshComponent&) = delete;
	MeshComponent& operator=(const MeshComponent&) = delete;

	void Update(float deltaTime);
	void Render(ID3D11DeviceContext* context);

	void setMatrix(XMMATRIX& position);
	void setMaterial(const char* name, XMFLOAT2 scale, XMFLOAT2 offset);

	HRESULT Init(ID3D11Device* device, ID3D11DeviceContext* context);

	bool CreateVertex(ID3D11Device* device, void* pBuffer, uint32_t sizeType, uint32_t size);
	bool CreateIndex(ID3D11Device* device, void* pBuffer, uint32_t sizeType, uint32_t size);

	GameObject* gameObject() const { return m_obj; }
	void setGameObject(GameObject* obj) { m_obj = obj; }
	XMMATRIX& position() const { return *m_position; }

	void Release();
};