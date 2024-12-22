#pragma once
#include "MeshComponent.h"

class FileSharedBuffer {
public:
	char* material;
	virtual void LoadFile(const char* filePath, std::vector<Vertex>& vertices, std::vector<DWORD>& indices) = 0;
};

class MeshLoader : public FileSharedBuffer {
public:
	void LoadFile(const char* filePath, std::vector<Vertex>& vertices, std::vector<DWORD>& indices) override;
};