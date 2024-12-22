#pragma once
#include "framework.h"

struct _Vertex;

class FileSharedBuffer {
public:
	char* material;
	virtual void LoadFile(const char* filePath, std::vector<_Vertex>& vertices, std::vector<DWORD>& indices) = 0;
};

class MeshLoader : public FileSharedBuffer {
public:
	void LoadFile(const char* filePath, std::vector<_Vertex>& vertices, std::vector<DWORD>& indices) override;
};