#pragma once
#include "framework.h"

#define MAX_DEVICE_IDENTIFIER_STRING 128

struct GraphicsEnumOutputInfo;
struct GraphicsEnumDeviceInfo;
struct GraphicsEnumDeviceSettingsCombo;

struct GraphicsDeviceSettings {
    uint32_t m_adapterOrdinal;
    D3D_DRIVER_TYPE m_driverType;
    uint32_t m_output;
    DXGI_SWAP_CHAIN_DESC m_swapChainDesc;
    uint32_t m_reateFlags;
    uint32_t m_syncInterval;
    DWORD m_presentFlags;
    BOOL m_autoCreateDepthStencil;
    DXGI_FORMAT m_autoDepthStencilFormat;
    D3D_FEATURE_LEVEL m_deviceFeatureLevel;
};

struct GraphicsEnumAdapterInfo {
public:
    ~GraphicsEnumAdapterInfo();

    uint32_t m_adapterOrdinal;
    DXGI_ADAPTER_DESC m_adapterDesc;
    WCHAR m_szUniqueDescription[MAX_DEVICE_IDENTIFIER_STRING];
    IDXGIAdapter* m_pAdapter;
    bool m_bAdapterUnavailable;

    std::vector<GraphicsEnumOutputInfo*> m_outputInfoList;
    std::vector<GraphicsEnumDeviceInfo*> m_deviceInfoList;
    std::vector<GraphicsEnumDeviceSettingsCombo*> m_deviceSettingsComboList;
};

struct GraphicsEnumDeviceInfo {
public:
    uint32_t m_adapterOrdinal;
    D3D_DRIVER_TYPE m_deviceType;
    D3D_FEATURE_LEVEL m_selectedLevel;
    D3D_FEATURE_LEVEL m_maxLevel;
    BOOL m_computeShaderDispatcher;
};

struct GraphicsEnumOutputInfo {
public:
	~GraphicsEnumOutputInfo();

	uint32_t m_adapterOrdinal;
	uint32_t m_outputOrdinal;
	DXGI_OUTPUT_DESC m_outputDesc;
	IDXGIOutput* m_pOutput;

	std::vector<DXGI_MODE_DESC> m_displayModeList;
};

struct GraphicsEnumDeviceSettingsCombo {
public:
	uint32_t m_adapterOrdinal;
	D3D_DRIVER_TYPE m_deviceType;
	DXGI_FORMAT m_backBufferFormat;
	BOOL m_windowed;
	uint32_t m_output;

	std::vector<uint32_t> m_multiSampleCountList;
	std::vector<uint32_t> m_multiSampleQualityList;

    GraphicsEnumAdapterInfo* m_pAdapterInfo;
    GraphicsEnumDeviceInfo* m_pDeviceInfo;
	GraphicsEnumOutputInfo* m_pOutputInfo;
};

class GraphicsAdapterEnumerator {
private:
    std::vector<GraphicsEnumAdapterInfo*> m_adapterInfoList;

public:
    std::vector<GraphicsEnumAdapterInfo*>* GetAdapterInfoList();
    GraphicsEnumAdapterInfo* GetAdapterInfo(uint32_t adapterOrdinal);
    GraphicsEnumDeviceInfo* GetDeviceInfo(uint32_t adapterOrdinal,
        D3D_DRIVER_TYPE deviceType
    );
    GraphicsEnumOutputInfo* GetOutputInfo(uint32_t adapterOrdinal, uint32_t output);
	GraphicsEnumDeviceSettingsCombo* GetDeviceSettingsCombo(
		const GraphicsDeviceSettings* pDeviceSettings
    );
	GraphicsEnumDeviceSettingsCombo* GetDeviceSettingsCombo(
        uint32_t adapterOrdinal, D3D_DRIVER_TYPE deviceType, 
        uint32_t output, DXGI_FORMAT backBufferFormat, BOOL windowed
    );
};