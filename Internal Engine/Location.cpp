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
    for (int i = 0; i < 5; ++i) {
        for (int y = 0; y < 5; ++y) {
            GameObject* obj = Instantiate(XMFLOAT3((float)i * 2, 0.0f, y * 2),
                XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));

            MeshComponent* mesh = obj->AddComponent<MeshComponent>();
            mesh->setMatrix(obj->transform().getWorldMatrix());
            engine.addMeshRenderer(mesh);

            Insert(obj);
        }
    }
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
