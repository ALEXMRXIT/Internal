#include "ResourceLoader.h"
#include "FileSharedBuffer.h"
#include "Model.h"
#include "SharedObject.h"

#include <intrin.h>

ResourceLoader::ResourceLoader(Engine& engine, FileSharedBuffer* fileBuffer) : 
        engine(engine), fileBuffer(fileBuffer), stop(false) {
	stopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    InitializeCriticalSection(&queueMutex);
    workerThread = CreateThread(NULL, 0, WorkerFunction, this, 0, NULL);
}

ResourceLoader::~ResourceLoader() {
    EnterCriticalSection(&queueMutex);
    stop = true;
    LeaveCriticalSection(&queueMutex);
    SetEvent(stopEvent);
    WaitForSingleObject(workerThread, INFINITE);
    CloseHandle(workerThread);
    CloseHandle(stopEvent);
    DeleteCriticalSection(&queueMutex);
}

void ResourceLoader::AddResourceToLoad(const char* resourceName, SharedObject* mesh) {
    EnterCriticalSection(&queueMutex);
    resourceQueue.emplace(MeshContainer{ mesh, resourceName });
    LeaveCriticalSection(&queueMutex);
    SetEvent(stopEvent);
}

DWORD WINAPI ResourceLoader::WorkerFunction(LPVOID param) {
    ResourceLoader* loader = static_cast<ResourceLoader*>(param);
    return loader->ThreadLoop();
}

DWORD ResourceLoader::ThreadLoop() {
    while (true) {
        MeshContainer resource;
        resource.m_fileName = nullptr;
        EnterCriticalSection(&queueMutex);
        if (stop && resourceQueue.empty()) {
            LeaveCriticalSection(&queueMutex);
            break;
        }
        if (!resourceQueue.empty()) {
            resource = resourceQueue.front();
            resourceQueue.pop();
        }
        LeaveCriticalSection(&queueMutex);
        if (resource.m_fileName)
            LoadResource(resource);
        else
            WaitForSingleObject(stopEvent, INFINITE);
    }
    return 0;
}

void ResourceLoader::LoadResource(MeshContainer& container) {
    std::vector<Vertex> vertices;
    std::vector<DWORD> indices;
    fileBuffer->LoadFile(container.m_fileName, vertices, indices);

    container.m_mesh->CreateVertex(engine.device(), vertices, sizeof(Vertex), vertices.size());
    container.m_mesh->CreateIndex(engine.device(), indices, sizeof(DWORD), indices.size());
    container.m_mesh->SetMaterialName(fileBuffer->material);
    container.m_mesh->Init(engine.device());
}
