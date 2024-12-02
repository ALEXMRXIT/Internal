#pragma once
#include "framework.h"

class Shader;

class Font {
private:
	ID3D10Device1* m_device;
	ID2D1Factory* m_d2dFactory;
	ID2D1RenderTarget* m_renderTarget;
	ID2D1SolidColorBrush* m_brush;
	IDWriteFactory* m_factory;
	IDWriteTextFormat* m_textFormat;
	ID3D11Texture2D* m_textureDesc;
	ID3D11ShaderResourceView* m_sharedResource;
	ID3D11RasterizerState* m_cWcullMode;
	Shader* m_fontShader;

public:
	Font();
	Font(const Font&) = delete;
	Font& operator=(const Font&) = delete;

	HRESULT Init(ID3D11Device* device, ID3D11DeviceContext* context, IDXGIAdapter1* adapter);
	void Render(ID3D11DeviceContext* deviceContext, const std::wstring text);

	void Release();
};