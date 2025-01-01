#pragma once
#include "MeshComponent.h"
#include "Physics.h"
#include "SharedObject.h"

class GameObject;

struct SelectableConstantBuffer {
	XMFLOAT4 selectable;
	XMFLOAT4 texture_color;
};

class Model : public SharedObject, public Physics {
private:
	SelectableConstantBuffer m_select;
	ID3D11Buffer* m_preObjectSelect;
	GameObject* m_obj;
	MeshComponent& m_component;

	bool m_selectable;
	float alpha;

public:
	Model(MeshComponent& component);

	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;

	void Update(float deltaTime);
	void Render(ID3D11DeviceContext* context);
	void Release();

	HRESULT Init(ID3D11Device* device) override;

	bool CreateVertex(ID3D11Device* device, const std::vector<Vertex>& vertices, uint32_t sizeType, uint32_t size) override;
	bool CreateIndex(ID3D11Device* device, const std::vector<DWORD>& indices, uint32_t sizeType, uint32_t size) override;

	void setMaterial(const char* name, XMFLOAT2 scale, XMFLOAT2 offset) override;

	GameObject* gameObject() const { return m_obj; }
	void setGameObject(GameObject* obj) { m_obj = obj; }
	void setSelectable(bool flag) { m_selectable = flag; }
	MeshComponent& mesh() const { return m_component; }
};