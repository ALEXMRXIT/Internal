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
        XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));

    MeshComponent* mesh = obj->AddComponent<MeshComponent>();
    mesh->setMatrix(obj->transform().getWorldMatrix());
    mesh->setMaterial("mesh\\m_had0330000.png", XMFLOAT2(1.f, 1.f), XMFLOAT2(0.f, 0.f));
    engine.addMeshRenderer(mesh, "mesh\\m_had033.obj");

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
