#include "Location.h"
#include "GameObject.h"
#include "MeshComponent.h"
#include "Model.h"

SERVLIBFINLINE static SERVLIBCALL GameObject* Instantiate(const XMFLOAT3 pos) {
    return Instantiate(pos, Quaternion::Identity(), XMFLOAT3(1.0f, 1.0f, 1.0f));
}

SERVLIBFINLINE static SERVLIBCALL GameObject* Instantiate(const XMFLOAT3 pos, const Quaternion quaternion) {
    return Instantiate(pos, quaternion, XMFLOAT3(1.0f, 1.0f, 1.0f));
}

SERVLIBFINLINE static SERVLIBCALL GameObject* Instantiate(const XMFLOAT3 pos, const Quaternion quaternion, const XMFLOAT3 scale) {
    GameObject* obj = new GameObject();
    Transform& transform = *obj->AddComponent<Transform>(obj);
    transform.position(pos);
    transform.rotation(quaternion);
    transform.scale(scale);
    return obj;
}

Location::Location(ID3D11Device* device) {
    m_meshLoader = new MeshLoader();
    m_loader = new ResourceLoader(engine, m_meshLoader);

    GameObject* light = Instantiate(XMFLOAT3(0.0f, 0.0f, 0.0f), Quaternion(20.0f, -22.5f, -90.0f, 0.0f));
    light->name = "Direction Light";
    m_directionLight = light->AddComponent<DirectionLight>(light);
    m_directionLight->Init(device);
    Insert(light);

    GameObject* objectSkybox = Instantiate(XMFLOAT3(0.0f, 0.0f, 0.0f));
    objectSkybox->name = "Skybox";
    objectSkybox->serialize = true;
    MeshComponent* meshSkybox = new MeshComponent(objectSkybox);
    m_skybox = new Skybox(*meshSkybox);
    m_loader->AddResourceToLoad("mesh\\skybox.obj", m_skybox);
    Insert(objectSkybox);

    GameObject* tarrain = Instantiate(XMFLOAT3(0.0f, 0.0f, 0.0f));
    tarrain->name = "Tarrain";
    MeshComponent* meshTarrain = tarrain->AddComponent<MeshComponent>(tarrain);
    Model* modelTarrain = new Model(*meshTarrain);
    tarrain->model = modelTarrain;
    m_loader->AddResourceToLoad("mesh\\terrain.obj", modelTarrain);
    Insert(tarrain);

    GameObject* home = Instantiate(XMFLOAT3(130.1f, 2.4f, -131.600f), Quaternion::Identity(), XMFLOAT3(0.1f, 0.1f, 0.1f));
    home->name = "Bash";
    MeshComponent* meshBash = home->AddComponent<MeshComponent>(home);
    Model* modelBash = new Model(*meshBash);
    home->model = modelBash;
    m_loader->AddResourceToLoad("mesh\\m_had430000.obj", modelBash);
    Insert(home);
}

void Location::Update(float deltaTime) {

}

void Location::Insert(GameObject* obj) {
    m_objects.push_back(obj);
}