#pragma once
#include "MeshComponent.h"

typedef struct _viewProject {
	XMMATRIX WVP;
	XMMATRIX World;
} ViewProject, *LPViewProject;

class Skybox {
private:
	VertexBuffer* m_vertexBuffer;
	IndexBuffer* m_indexBuffer;
	Shader* m_shader;

	ID3D11ShaderResourceView* m_sharedView;
	ID3D11DepthStencilState* m_depthState;
	ID3D11RasterizerState* m_cullMode;
	ID3D11Buffer* m_preObjectBuffer;
	ID3D11SamplerState* m_textureSamplerState;

	int verticesNum;
	int indexesNum;

	XMMATRIX m_pos;
	ViewProject m_wvp;

	void CreateSphere(ID3D11Device* device, int llines, int longlines);

public:
	Skybox();

	Skybox(const Skybox&) = delete;
	Skybox& operator=(const Skybox&) = delete;

	void Init(ID3D11Device* device);
	void Update(float deltaTime);
	void Render(ID3D11DeviceContext* context);

	void IASetVertexAndIndexBuffer(ID3D11DeviceContext* context);
	bool CreateVertex(ID3D11Device* device, void* pBuffer, uint32_t sizeType, uint32_t size);
	bool CreateIndex(ID3D11Device* device, void* pBuffer, uint32_t sizeType, uint32_t size);

	void Release();
};