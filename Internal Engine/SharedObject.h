#pragma once
#include "framework.h"
#include "MeshComponent.h"

class SharedObject {
public:
	virtual HRESULT Init(ID3D11Device* device) = 0;
	virtual bool CreateVertex(ID3D11Device* device, const std::vector<Vertex>& vertices, uint32_t sizeType, uint32_t size) = 0;
	virtual bool CreateIndex(ID3D11Device* device, const std::vector<DWORD>& indices, uint32_t sizeType, uint32_t size) = 0;
	virtual void setMaterial(const char* name, XMFLOAT2 scale, XMFLOAT2 offset) = 0;
};