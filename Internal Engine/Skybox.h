#pragma once
#include "SharedObject.h"
#include "MeshComponent.h"
#include "LoaderNotificationDevice.h"

typedef struct _viewProject {
	XMMATRIX WVP;
} ViewProject, *LPViewProject;

class Skybox : public SharedObject, public LoaderNotificationDevice {
private:
	ID3D11DepthStencilState* m_depthState;
	ID3D11RasterizerState* m_cullMode;
	ID3D11Buffer* m_preObjectBuffer;
	ID3D11InputLayout* m_layout;
	MeshComponent& m_component;
	Shader* m_shader;

	XMMATRIX m_pos;
	ViewProject m_wvp;

public:
	Skybox(MeshComponent& component);

	Skybox(const Skybox&) = delete;
	Skybox& operator=(const Skybox&) = delete;

	HRESULT Init(ID3D11Device* device) override;
	void Update(float deltaTime);
	void Render(ID3D11DeviceContext* context);

	bool CreateVertex(ID3D11Device* device, const std::vector<Vertex>& vertices, uint32_t sizeType, uint32_t size) override;
	bool CreateIndex(ID3D11Device* device, const std::vector<DWORD>& indices, uint32_t sizeType, uint32_t size) override;

	void setMaterial(const char* name, XMFLOAT2 scale, XMFLOAT2 offset) override;

	void Release();
};