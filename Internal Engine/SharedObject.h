#pragma once
#include "framework.h"
#include "MeshComponent.h"

class SharedObject {
private:
	char* m_materialName;

public:
	virtual HRESULT Init(ID3D11Device* device) = 0;
	virtual bool CreateVertex(ID3D11Device* device, const std::vector<Vertex>& vertices, uint32_t sizeType, uint32_t size) = 0;
	virtual bool CreateIndex(ID3D11Device* device, const std::vector<DWORD>& indices, uint32_t sizeType, uint32_t size) = 0;

	__forceinline void SetMaterialName(const char* name) { m_materialName = (char*)name; };
	__forceinline char* MaterialName() const { return m_materialName; }
};