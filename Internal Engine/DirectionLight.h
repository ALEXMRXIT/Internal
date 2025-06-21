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
	XMFLOAT4 AmbiendColor;
	XMFLOAT4 DirectionOption;
};

class DirectionLight : public AbstractBaseComponent, public LoaderNotificationDevice {
private:
	ID3D11Buffer* m_directionBuffer;
	XMMATRIX m_lightProjectionMatrix;
	XMMATRIX m_viewProjectionCache;

	BufferDirectionOption m_directionOption;

public:
	DirectionLight(GameObject* obj);

	HRESULT Init(ID3D11Device* device);

	void Render(ID3D11DeviceContext* context);
	void Release();

	BufferDirectionLight UpdateMatrixByDirectionLight(XMMATRIX worldPos);
	ALWAYS_INLINE XMFLOAT4& GetDirectionLightOption() { return m_directionOption.DirectionOption; }
	XMMATRIX GetViewProjectionMatrix();

#ifdef INTERNAL_ENGINE_GUI_INTERFACE
	void UpdateInterfaceInInspector(GameObject* gameObject) override;
#endif
};