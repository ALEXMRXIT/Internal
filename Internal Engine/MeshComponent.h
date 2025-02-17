#pragma once
#include "Engine.h"
#include "AbstractBaseComponent.h"
#include "GameObject.h"
#include "LoaderNotificationDevice.h"

class MeshMaterial;
class ViewProjectonData;

struct Vertex {
	XMFLOAT3 position;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
	Vertex() : position(0.f, 0.f, 0.f), texCoord(0.f, 0.f), normal(0.f, 0.f, 0.f) {}

	bool operator==(const Vertex& other) const {
        return position.x == other.position.x && position.y == other.position.y && position.z == other.position.z &&
			normal.x == other.normal.x && normal.y == other.normal.y && normal.z == other.normal.z &&
			texCoord.x == other.texCoord.x && texCoord.y == other.texCoord.y;
    }
};

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

__declspec(align(16))
struct WorldViewProjection {
	XMMATRIX WVP;
	XMMATRIX World;
	XMFLOAT2 texture_scale;
	XMFLOAT2 texture_offset;
};

__declspec(align(16))
struct AdditionalColored {
	XMFLOAT4 texture_color;
	float alpha;
};

class MeshComponent : public AbstractBaseComponent, public LoaderNotificationDevice {
private:
	VertexBuffer* m_vertexBuffer;
	IndexBuffer* m_indexBuffer;
	MeshMaterial* m_material;
	WorldViewProjection m_bufferWVP;
	ID3D11Buffer* m_preObjectBuffer;
	AdditionalColored m_additionalColor;
	ID3D11Buffer* m_preObjectSelect;
	XMMATRIX* m_position;
	uint32_t m_indices;

	bool m_spawned;

private:
	void IASetVertexAndIndexBuffer(ID3D11DeviceContext* context);

public:
	Model* model;

public:
	MeshComponent();
	MeshComponent(const MeshComponent&) = delete;
	MeshComponent& operator=(const MeshComponent&) = delete;

	void Update(float deltaTime);
	void UpdateWVPMatrix(ID3D11DeviceContext* context, const ViewProjectonData& viewProjection);
	void Render(ID3D11DeviceContext* context);
	void RenderShadow(ID3D11DeviceContext* context, const ViewProjectonData& viewProjection);

	void setMatrix(XMMATRIX& position) override;
	void setMaterial(const char* name, XMFLOAT2 scale, XMFLOAT2 offset);

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
	void UpdateInterfaceInInspector(GameObject* gameObject) override;
#endif

	HRESULT Init(ID3D11Device* device);

	bool CreateVertex(ID3D11Device* device, const std::vector<Vertex>& vertices, uint32_t sizeType, uint32_t size);
	bool CreateIndex(ID3D11Device* device, const std::vector<DWORD>& indices, uint32_t sizeType, uint32_t size);

	MeshMaterial* material() const { return m_material; }
	XMMATRIX& position() const { return *m_position; }
	float alpha() const { return m_additionalColor.alpha; }
	void setlAlpha(float value) { m_additionalColor.alpha = value; }
	void clearAlpha() { m_additionalColor.alpha = 0.0f; }

	void Release();
};