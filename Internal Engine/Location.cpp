#include "Location.h"
#include "GameObject.h"
#include "MeshComponent.h"

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

    GameObject* obj = Instantiate(XMFLOAT3(-25.0f, 0.0f, 0.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
    MeshComponent* mesh = obj->AddComponent<MeshComponent>();
    mesh->setMatrix(obj->transform().getWorldMatrix());
    mesh->setGameObject(obj);
    m_loader->AddResourceToLoad("mesh\\m_had033.obj", mesh);

    GameObject* plane = Instantiate(XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(100.0f, 1.0f, 100.0f));
    MeshComponent* planeMesh = plane->AddComponent<MeshComponent>();
    planeMesh->setMatrix(plane->transform().getWorldMatrix());
    planeMesh->setGameObject(plane);
    m_loader->AddResourceToLoad("mesh\\plane.obj", planeMesh);

    GameObject* house = Instantiate(XMFLOAT3(60.0f, 0.0f, 50.0f),
        XMFLOAT3(0.0f, 180.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
    MeshComponent* houseMesh = house->AddComponent<MeshComponent>();
    houseMesh->setMatrix(house->transform().getWorldMatrix());
    houseMesh->setGameObject(house);
    m_loader->AddResourceToLoad("mesh\\m_hbf011.obj", houseMesh);
}

void Location::Update(float deltaTime) {

}

void Location::Insert(GameObject* obj) {
    m_objects.push_back(obj);
}
