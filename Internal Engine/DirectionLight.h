#pragma once
#include "framework.h"
#include "Transform.h"
#include "AbstractBaseComponent.h"
#include "LoaderNotificationDevice.h"
#include "ViewProjectonData.h"
#include "servlib.h"

__declspec(align(16))
struct BufferDirectionLight {
	XMMATRIX lightViewProj;
	XMFLOAT4 direction;
};

__declspec(align(16))
struct BufferDirectionOption {
	XMFLOAT4 LightDirection;
	XMFLOAT4 AmbiendColor;
	float baked;
	float diffuseIntensity;
	float shadowIntensity;
	float bias;
	float minShadowBrightness;
	float shadowDiffuseMix;
	float padding[2];
};

class DirectionLight : public AbstractBaseComponent, public LoaderNotificationDevice {
private:
	ID3D11Buffer* m_directionBuffer;
	XMMATRIX m_lightProjectionMatrix;
	XMMATRIX m_viewProjectionCache;

	BufferDirectionOption m_directionOption;

public:
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
	XMMATRIX GetViewProjectionMatrix();

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
	void UpdateInterfaceInInspector(GameObject* gameObject) override;
#endif
};