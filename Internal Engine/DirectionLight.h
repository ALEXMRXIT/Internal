#pragma once
#include "framework.h"
#include "Transform.h"
#include "AbstractBaseComponent.h"
#include "LoaderNotificationDevice.h"
#include "ViewProjectonData.h"

struct BufferDirectionLight {
	XMFLOAT4 direction;
	float intensity;
	float padding[3];
	XMMATRIX lightViewProj;
};

class DirectionLight : public AbstractBaseComponent, public LoaderNotificationDevice {
private:
	ID3D11Buffer* m_constantLightBuffer;
	BufferDirectionLight m_bufferLight;
	XMMATRIX m_lightProjectionMatrix;
	ViewProjectonData* m_viewProjectionData;

private:
	void UpdateMatrix();

public:
	Transform* m_transform;

public:
	DirectionLight();

	HRESULT Init(ID3D11Device* device);

	void Update(float deltaTime);
	void Render(ID3D11DeviceContext* device_context);
	void Release();
	inline const ViewProjectonData& viewProjection() const { return *m_viewProjectionData; }

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
	void UpdateInterfaceInInspector(GameObject* gameObject) override;
#endif

	void setMatrix(XMMATRIX& position) override;
};