#include "Location.h"
#include "GameObject.h"
#include "MeshComponent.h"
#include "Model.h"

static GameObject* Instantiate(const XMFLOAT3 pos, const Quaternion quaternion) {
    GameObject* obj = new GameObject();

    Transform& transform = obj->transform();
    transform.position(pos);
    transform.rotation(quaternion);
    transform.scale(XMFLOAT3(1.0f, 1.0f, 1.0f));

    return obj;
}

Location::Location(ID3D11Device* device) {
    m_meshLoader = new MeshLoader();
    m_loader = new ResourceLoader(engine, m_meshLoader);

    GameObject* light = Instantiate(XMFLOAT3(0.0f, 0.0f, 0.0f), Quaternion::Identity());
    light->name = "Direction Light";
    m_directionLight = light->AddComponent<DirectionLight>();
    m_directionLight->m_transform = &light->transform();
    m_directionLight->Init(device);
    Insert(light);

    GameObject* objectSkybox = Instantiate(XMFLOAT3(0.0f, 0.0f, 0.0f), Quaternion::Identity());
    objectSkybox->name = "Skybox";
    objectSkybox->serialize = true;
    MeshComponent* meshSkybox = new MeshComponent();
    meshSkybox->setMatrix(objectSkybox->transform().GetWorldMatrix());
    meshSkybox->gameObject(objectSkybox);
    m_skybox = new Skybox(*meshSkybox);
    m_loader->AddResourceToLoad("mesh\\skybox.obj", m_skybox);
    Insert(objectSkybox);

    GameObject* tarrain = Instantiate(XMFLOAT3(0.0f, 0.0f, 0.0f), Quaternion::Identity());
    tarrain->name = "Tarrain";
    MeshComponent* meshTarrain = tarrain->AddComponent<MeshComponent>();
    Model* modelTarrain = new Model(*meshTarrain);
    tarrain->model = modelTarrain;
    meshTarrain->gameObject(tarrain);
    meshTarrain->setMatrix(tarrain->transform().GetWorldMatrix());
    m_loader->AddResourceToLoad("mesh\\terrain.obj", modelTarrain);
    Insert(tarrain);

    GameObject* home = Instantiate(XMFLOAT3(130.1f, 2.4f, -131.600f), Quaternion::Identity());
    home->name = "Bash";
    MeshComponent* meshBash = home->AddComponent<MeshComponent>();
    Model* modelBash = new Model(*meshBash);
    home->model = modelBash;
    meshBash->gameObject(home);
    meshBash->setMatrix(home->transform().GetWorldMatrix());
    m_loader->AddResourceToLoad("mesh\\m_had430000.obj", modelBash);
    Insert(home);
}

void Location::Update(float deltaTime) {

}

void Location::Insert(GameObject* obj) {
    m_objects.push_back(obj);
}