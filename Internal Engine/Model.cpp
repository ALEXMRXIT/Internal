#include "Model.h"
#include "Material.h"

Model::Model(MeshComponent& component) : m_component(component) {
	m_component.model = this;
	m_obj = nullptr;
}

void Model::Update(float deltaTime) {
	m_component.Update(deltaTime);
}

void Model::Render(ID3D11DeviceContext* context) {
	m_component.UpdateWVPMatrix(context);

	m_component.Render(context);
}

void Model::Release() {
	if (m_obj) delete m_obj;

	m_component.Release();
}

HRESULT Model::Init(ID3D11Device* device) {
	HRESULT hr{};
	hr = m_component.Init(device);
	engine.addMeshRenderer(this);
	return hr;
}

bool Model::CreateVertex(ID3D11Device* device, const std::vector<Vertex>& vertices, uint32_t sizeType, uint32_t size) {
	setVerticesPhysics(vertices);
	return m_component.CreateVertex(device, vertices, sizeType, size);
}

bool Model::CreateIndex(ID3D11Device* device, const std::vector<DWORD>& indices, uint32_t sizeType, uint32_t size) {
	setIndecesPhysics(indices);
	return m_component.CreateIndex(device, indices, sizeType, size);
}

void Model::setMaterial(const char* name, XMFLOAT2 scale, XMFLOAT2 offset) {
	m_component.setMaterial(name, scale, offset);
}
