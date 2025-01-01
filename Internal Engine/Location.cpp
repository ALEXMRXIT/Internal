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

Location::Location() {
    m_meshLoader = new MeshLoader();
    m_loader = new ResourceLoader(engine, m_meshLoader);

    GameObject* objectSkybox = Instantiate(XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
    objectSkybox->name = "Skybox";
    MeshComponent* meshSkybox = new MeshComponent();
    meshSkybox->setMatrix(objectSkybox->transform().getWorldMatrix());
    m_skybox = new Skybox(*meshSkybox);
    m_loader->AddResourceToLoad("mesh\\skybox.obj", m_skybox);
    Insert(objectSkybox);
    
    GameObject* obj = Instantiate(XMFLOAT3(-25.0f, 0.0f, 0.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
    obj->name = "Cart";
    MeshComponent* mesh = obj->AddComponent<MeshComponent>();
    Model* model = new Model(*mesh);
    obj->model = model;
    model->setGameObject(obj);
    mesh->setMatrix(obj->transform().getWorldMatrix());
    m_loader->AddResourceToLoad("mesh\\m_had033.obj", model);
    Insert(obj);
    
    GameObject* plane = Instantiate(XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(100.0f, 1.0f, 100.0f));
    plane->name = "Ground";
    MeshComponent* planeMesh = plane->AddComponent<MeshComponent>();
    Model* modelPlane = new Model(*planeMesh);
    plane->model = modelPlane;
    modelPlane->setGameObject(plane);
    planeMesh->setMatrix(plane->transform().getWorldMatrix());
    m_loader->AddResourceToLoad("mesh\\plane.obj", modelPlane);
    Insert(plane);
    
    GameObject* house = Instantiate(XMFLOAT3(60.0f, 0.0f, 50.0f),
        XMFLOAT3(0.0f, 180.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
    house->name = "House";
    MeshComponent* houseMesh = house->AddComponent<MeshComponent>();
    Model* modelHouse = new Model(*houseMesh);
    house->model = modelHouse;
    modelHouse->setGameObject(house);
    houseMesh->setMatrix(house->transform().getWorldMatrix());
    m_loader->AddResourceToLoad("mesh\\m_hbf011.obj", modelHouse);
    Insert(house);
}

void Location::Update(float deltaTime) {

}

void Location::Insert(GameObject* obj) {
    m_objects.push_back(obj);
}
