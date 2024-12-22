#pragma once
#include "framework.h"
#include "MeshComponent.h"
#include <queue>

class MeshComponent;
class FileSharedBuffer;

class ResourceLoader {
private:
	struct MeshContainer {
		MeshComponent* m_mesh;
		const char* m_fileName;
	};

private:
	Engine& engine;
	FileSharedBuffer* fileBuffer;
	HANDLE workerThread;
	HANDLE stopEvent;
	std::queue<MeshContainer> resourceQueue;
	CRITICAL_SECTION queueMutex;
	bool stop;

private:
	static DWORD WINAPI WorkerFunction(LPVOID param);

	DWORD ThreadLoop();
	void LoadResource(const char* resourceName, MeshComponent* mesh);

public:
	ResourceLoader(Engine& engine, FileSharedBuffer* fileBuffer);
	~ResourceLoader();

	ResourceLoader(const ResourceLoader&) = delete;
	ResourceLoader& operator=(const ResourceLoader&) = delete;

	void AddResourceToLoad(const char* resourceName, MeshComponent* mesh);
};