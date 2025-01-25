#include "Location.h"
#include "GameObject.h"
#include "MeshComponent.h"
#include "Model.h"

GameObject* Instantiate(const XMFLOAT3& pos, const XMFLOAT3& rot, const XMFLOAT3& scale) {
    GameObject* obj = new GameObject();

    obj->setPosition(pos);
    obj->setRotation(rot);
    obj->setScale(scale);

    return obj;
}

Location::Location(ID3D11Device* device) {
    m_meshLoader = new MeshLoader();
    m_loader = new ResourceLoader(engine, m_meshLoader);

    GameObject* light = Instantiate(XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(-0.9f, -6.4f, 5.2f), XMFLOAT3(1.0f, 1.0f, 1.0f));
    light->name = "Direction Light";
    m_directionLight = light->AddComponent<DirectionLight>();
    m_directionLight->m_transform = &light->transform();
    m_directionLight->Init(device);
    Insert(light);

    GameObject* objectSkybox = Instantiate(XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
    objectSkybox->name = "Skybox";
    objectSkybox->serialize = true;
    MeshComponent* meshSkybox = new MeshComponent();
    meshSkybox->setMatrix(objectSkybox->transform().getWorldMatrix());
    m_skybox = new Skybox(*meshSkybox);
    m_loader->AddResourceToLoad("mesh\\skybox.obj", m_skybox);
    Insert(objectSkybox);

    GameObject* tarrain = Instantiate(XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
    tarrain->name = "Tarrain";
    MeshComponent* meshTarrain = tarrain->AddComponent<MeshComponent>();
    Model* modelTarrain = new Model(*meshTarrain);
    tarrain->model = modelTarrain;
    modelTarrain->setGameObject(tarrain);
    meshTarrain->setMatrix(tarrain->transform().getWorldMatrix());
    m_loader->AddResourceToLoad("mesh\\terrain.obj", modelTarrain);
    Insert(tarrain);

    GameObject* home = Instantiate(XMFLOAT3(130.1f, 2.4f, -131.600f),
        XMFLOAT3(0.0f, -0.500f, 0.0f), XMFLOAT3(0.1f, 0.1f, 0.1f));
    home->name = "Bash";
    MeshComponent* meshBash = home->AddComponent<MeshComponent>();
    Model* modelBash = new Model(*meshBash);
    home->model = modelBash;
    modelBash->setGameObject(home);
    meshBash->setMatrix(home->transform().getWorldMatrix());
    m_loader->AddResourceToLoad("mesh\\m_had430000.obj", modelBash);
    Insert(home);
}

void Location::Update(float deltaTime) {

}

void Location::Insert(GameObject* obj) {
    m_objects.push_back(obj);
}