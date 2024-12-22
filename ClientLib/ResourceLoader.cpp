#include "ResourceLoader.h"
#include "FileSharedBuffer.h"

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

void ResourceLoader::AddResourceToLoad(const char* resourceName, MeshComponent* mesh) {
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
            LoadResource(resource.m_fileName, resource.m_mesh);
        else
            WaitForSingleObject(stopEvent, INFINITE);
    }
    return 0;
}

void ResourceLoader::LoadResource(const char* resourceName, MeshComponent* mesh) {
    std::vector<Vertex> vertices;
    std::vector<DWORD> indices;
    fileBuffer->LoadFile(resourceName, vertices, indices);

    mesh->CreateVertex(engine.device(), vertices, sizeof(Vertex), vertices.size());
    mesh->CreateIndex(engine.device(), indices, sizeof(DWORD), indices.size());
    mesh->setMaterial(fileBuffer->material, XMFLOAT2(1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f));

    mesh->Init(engine.device());
    engine.addMeshRenderer(mesh);
}
