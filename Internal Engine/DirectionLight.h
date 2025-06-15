#pragma once
#include "framework.h"
#include "Transform.h"
#include "AbstractBaseComponent.h"
#include "LoaderNotificationDevice.h"
#include "ViewProjectonData.h"

__declspec(align(16))
struct BufferDirectionLight {
	XMMATRIX lightViewProj;
	XMFLOAT4 direction;
};

class DirectionLight : public AbstractBaseComponent, public LoaderNotificationDevice {
private:
	BufferDirectionLight m_bufferLight;
	XMMATRIX m_lightProjectionMatrix;

public:
	Transform* m_transform;

public:
	DirectionLight();

	HRESULT Init(ID3D11Device* device);
	void Release();
	BufferDirectionLight UpdateMatrixByDirectionLight(XMMATRIX worldPos);
	XMMATRIX GetViewProjectionMatrix();

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
	void UpdateInterfaceInInspector(GameObject* gameObject) override;
#endif

	void setMatrix(XMMATRIX position) override {};
};