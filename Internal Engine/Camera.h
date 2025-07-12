#pragma once
#include "framework.h"
#include "AbstractBaseComponent.h"

class Transform;

class Camera : public AbstractBaseComponent {
private:
	XMMATRIX view;
	XMMATRIX projection;
	Transform* transform = NULL;

public:
	Camera(GameObject* obj);
	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;

	void Update();

	const XMMATRIX& getView() const { return view; }
	const XMMATRIX& getProjection() const { return projection; }
	const XMVECTOR& getPos() const;

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
	void UpdateInterfaceInInspector(GameObject* gameObject) override;
#endif

};