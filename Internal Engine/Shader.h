#pragma once
#include "framework.h"

class Shader {
private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D10Blob* m_vertexBlob;

	HRESULT LoadShader(const char* entryPoint, const char* shaderName, const char* profile, ID3D10Blob** blob);

public:
	Shader();
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	HRESULT LoadVertexShader(ID3D11Device* device, const char* entry, const char* shaderName);
	HRESULT LoadPixelShader(ID3D11Device* device, const char* entry, const char* shaderName);

	void setVertexShader(ID3D11DeviceContext* context) const;
	void setPiexlShader(ID3D11DeviceContext* context) const;

	ID3D10Blob* getVertexBlob() const { return m_vertexBlob; }

	void Release();
};