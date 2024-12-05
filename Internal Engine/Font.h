#pragma once
#include "framework.h"

class Shader;

typedef struct _cbufferFont {
	XMMATRIX WVP;
} cBuffer, *LPcBuffer;

class Font {
private:
	ID2D1RenderTarget* m_renderTarget;
	ID2D1SolidColorBrush* m_brush;
	IDWriteFactory* m_factory;
	IDWriteTextFormat* m_textFormat;
	ID3D11Texture2D* m_textureDesc;
	ID3D11ShaderResourceView* m_sharedResource;
	ID3D11RasterizerState* m_cWcullMode;
	Shader* m_fontShader;
	ID3D11SamplerState* m_textureSamplerState;
	ID3D11Buffer* m_preObjectBuffer;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;

	cBuffer m_bufferWVP;

public:
	Font();
	Font(const Font&) = delete;
	Font& operator=(const Font&) = delete;

	HRESULT Init(ID3D11Device* device, ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* deviceContext, const std::wstring text);

	void Release();
};