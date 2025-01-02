#include "Model.h"
#include "Material.h"

Model::Model(MeshComponent& component) : m_component(component) {
	m_component.model = this;
	m_preObjectSelect = nullptr;
	m_obj = nullptr;
	m_select.selectable = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	alpha = 0.0f;
}

void Model::Update(float deltaTime) {
	if (alpha < 1.0f) {
		alpha += deltaTime * 5.0f;
		alpha = min(alpha, 1.0f);
	}
}

void Model::Render(ID3D11DeviceContext* context) {
	m_component.UpdateWVPMatrix(context);

	m_select.selectable = XMFLOAT4(0.0f, alpha, 0.0f, 0.0f);
	m_select.texture_color = m_component.material()->color();
	context->UpdateSubresource(m_preObjectSelect, 0, NULL, &m_select, 0, 0);
	context->PSSetConstantBuffers(2, 1, &m_preObjectSelect);

	m_component.Render(context);
}

void Model::Release() {
	if (m_preObjectSelect) m_preObjectSelect->Release();
	if (m_obj) delete m_obj;

	m_component.Release();
}

HRESULT Model::Init(ID3D11Device* device) {
	HRESULT hr{};
	D3D11_BUFFER_DESC bufferDescSelectable;
	ZeroMemory(&bufferDescSelectable, sizeof(D3D11_BUFFER_DESC));
	bufferDescSelectable.Usage = D3D11_USAGE_DEFAULT;
	bufferDescSelectable.ByteWidth = sizeof(SelectableConstantBuffer);
	bufferDescSelectable.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescSelectable.CPUAccessFlags = 0;
	bufferDescSelectable.MiscFlags = 0;
	hr = device->CreateBuffer(&bufferDescSelectable, NULL, &m_preObjectSelect);
	if (FAILED(hr)) {
		DXUT_ERR_MSGBOX("Failed to create buffer.", hr);
		return hr;
	}
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
