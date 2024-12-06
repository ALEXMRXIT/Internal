#pragma once
#include "MeshComponent.h"

class Skybox {
private:
	VertexBuffer* m_vertexBuffer;
	IndexBuffer* m_indexBuffer;
	Shader* m_shader;

	ID3D11ShaderResourceView* m_sharedView;
	ID3D11DepthStencilState* m_depthState;
	ID3D11RasterizerState* m_cullMode;

	XMMATRIX m_pos;

	void CreateSphere(int lat, int longL);

public:
	Skybox(const Skybox&) = delete;
	Skybox& operator=(const Skybox&) = delete;

	void Init(ID3D11Device* device, ID3D11DeviceContext* context);
	void Update(float deltaTime);
	void Render(ID3D11DeviceContext* context);
};