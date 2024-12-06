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
    GameObject* obj = Instantiate(XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(10.0f, 0.0f, 10.0f));

    MeshComponent* mesh = obj->AddComponent<MeshComponent>();
    mesh->setMatrix(obj->transform().getWorldMatrix());
    mesh->setMaterial("grass.jpg", XMFLOAT2(10.f, 10.f), XMFLOAT2(0.f, 0.f));
    engine.addMeshRenderer(mesh);

    Insert(obj);
}

void Location::Update(float deltaTime) {
    //static float rot = 0.0f;
    //rot += deltaTime * 5.0f;
    //
    //for (GameObject* obj : m_objects) {
    //    obj->setRotation(XMFLOAT3(rot, rot, rot));
    //}
}

void Location::Insert(GameObject* obj) {
    m_objects.push_back(obj);
}
