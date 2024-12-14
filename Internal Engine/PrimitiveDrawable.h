#pragma once
#include "MeshComponent.h"

struct VertexLine {
	XMFLOAT3 Position;
	XMFLOAT4 Color;
};

typedef struct _cbufferPrimitive {
	XMFLOAT4X4 WVP;
} cBuffePrimitive;

class PrimitiveDrawable {
private:
	ID3D11DeviceContext* m_context;

	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	ID3D11Buffer* m_preObjectBuffer;
	Shader* m_shader;
	ID3D11InputLayout* m_inputLayout;

	std::vector<VertexLine> m_vertices;
	std::vector<UINT> m_indices;
	cBuffePrimitive m_bufferWVP;

private:
	void UpdateVertexBuffer(const std::vector<VertexLine>& vertices);
	void UpdateIndexBuffer(const std::vector<UINT>& indices);

public:
	PrimitiveDrawable();

	PrimitiveDrawable(const PrimitiveDrawable&) = delete;
	PrimitiveDrawable& operator=(const PrimitiveDrawable&) = delete;

	void Init(ID3D11Device* device, ID3D11DeviceContext* context);
	void Release();

	void DrawLine(const XMFLOAT3& start, const XMFLOAT3& end, const XMFLOAT4& color);
	void Render();
};