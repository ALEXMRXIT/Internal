#pragma once
#include "framework.h"
#include "Transform.h"
#include "AbstractBaseComponent.h"
#include "LoaderNotificationDevice.h"

__declspec(align(16))
struct BufferDirectionLight {
	XMFLOAT4 direction;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	float intensity;
	float darkness;
};

class DirectionLight : public AbstractBaseComponent, public LoaderNotificationDevice {
private:
	ID3D11Buffer* m_constantLightBuffer;
	BufferDirectionLight m_bufferLight;

public:
	Transform* m_transform;

public:
	DirectionLight();

	HRESULT Init(ID3D11Device* device);

	void Update(float deltaTime);
	void Render(ID3D11DeviceContext* device_context);
	void Release();

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
	void UpdateInterfaceInInspector(GameObject* gameObject) override;
#endif

	void setMatrix(XMMATRIX& position) override;
};