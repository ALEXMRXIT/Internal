#pragma once
#include "Engine.h"
#include <queue>

class SharedObject;
class FileSharedBuffer;

class ResourceLoader {
private:
	struct MeshContainer {
		SharedObject* m_mesh;
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
	void LoadResource(MeshContainer& container);

public:
	ResourceLoader(Engine& engine, FileSharedBuffer* fileBuffer);
	~ResourceLoader();

	ResourceLoader(const ResourceLoader&) = delete;
	ResourceLoader& operator=(const ResourceLoader&) = delete;

	void AddResourceToLoad(const char* resourceName, SharedObject* mesh);
};