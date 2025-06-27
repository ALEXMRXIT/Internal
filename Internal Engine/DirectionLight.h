#pragma once
#include "framework.h"
#include "Transform.h"
#include "AbstractBaseComponent.h"
#include "LoaderNotificationDevice.h"
#include "servlib.h"

__declspec(align(16))
struct BufferDirectionLight {
	XMMATRIX lightViewProj;
	XMFLOAT4 direction;
};

__declspec(align(16))
struct BufferDirectionOption {
	XMFLOAT4 LightDirection;
	XMFLOAT4 LightColor;
	XMFLOAT4 AmbiendColor;
	float baked;
	float diffuseIntensity;
	float shadowIntensity;
	float bias;
	XMFLOAT2 shadowSize;
	float pcfSize;
	XMFLOAT3 cameraPos;
	float padding[2];
};

class DirectionLight : public AbstractBaseComponent, public LoaderNotificationDevice {
private:
	ID3D11Buffer* m_directionBuffer;
	XMMATRIX m_lightProjectionMatrix;
	XMMATRIX m_viewProjectionCache;
	int shadowMapSize;

	ID3D11Device* m_device; // for recreate shadow map

	BufferDirectionOption m_directionOption;

public:
	static const int m_presetValues[5];

	int directionType;
	int shadowType;

public:
	DirectionLight(GameObject* obj);

	HRESULT Init(ID3D11Device* device);

	void Render(ID3D11DeviceContext* context);
	void Release();

	BufferDirectionLight UpdateMatrixByDirectionLight(XMMATRIX worldPos);
	ALWAYS_INLINE XMFLOAT4& LightDirection() { return m_directionOption.LightDirection; }
	ALWAYS_INLINE float& GetDirectionLightBaked() { return m_directionOption.baked; }
	ALWAYS_INLINE XMFLOAT2 GetShadowSize() const { return m_directionOption.shadowSize; }
	XMMATRIX GetViewProjectionMatrix();

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
	void UpdateInterfaceInInspector(GameObject* gameObject) override;
#endif
};