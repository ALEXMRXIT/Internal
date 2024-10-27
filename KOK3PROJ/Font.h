#pragma once
#include "framework.h"

class Font {
private:
	ID3D10Device1* m_device;
	IDXGIKeyedMutex* m_mutex1;
	IDXGIKeyedMutex* m_mutex2;
	ID2D1RenderTarget* m_renderTarget;
	ID2D1SolidColorBrush* m_brush;
	ID3D11Texture2D* m_backBuffer;
	ID3D11Texture2D* m_sharedTexture;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	ID3D11ShaderResourceView* m_shaderView;
	IDWriteFactory* m_factory;
	IDWriteTextFormat* m_textFormat;

public:
	Font();
	Font(const Font&) = delete;
	Font& operator=(const Font&) = delete;

	HRESULT InitFontDevice(IDXGIAdapter* adapter, ID3D11Device* device11);
	HRESULT InitScreen(ID3D11Device* device);
	void Render();

	void Release();
};