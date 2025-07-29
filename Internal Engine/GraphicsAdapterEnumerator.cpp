#include "GraphicsAdapterEnumerator.h"

GraphicsEnumAdapterInfo::~GraphicsEnumAdapterInfo() {
    for (int i = 0; i < m_outputInfoList.size(); ++i) 
		delete m_outputInfoList[i];
    m_outputInfoList.clear();

    for (int i = 0; i < m_deviceInfoList.size(); ++i)
		delete m_deviceInfoList[i];
    m_deviceInfoList.clear();

    for (int i = 0; i < m_deviceSettingsComboList.size(); ++i)
        delete m_deviceSettingsComboList[i];
	m_deviceSettingsComboList.clear();

    if (m_pAdapter) m_pAdapter->Release();
    m_pAdapter = NULL;
}

GraphicsEnumOutputInfo::~GraphicsEnumOutputInfo() {
	if (m_pOutput) m_pOutput->Release();
    m_pOutput = NULL;
	m_displayModeList.clear();
}

std::vector<GraphicsEnumAdapterInfo*>* GraphicsAdapterEnumerator::GetAdapterInfoList() {
	return &m_adapterInfoList;
}

GraphicsEnumAdapterInfo* GraphicsAdapterEnumerator::GetAdapterInfo(uint32_t adapterOrdinal) {
    for (int adapterIter = 0; adapterIter < m_adapterInfoList.size(); ++adapterIter) {
		GraphicsEnumAdapterInfo* adapter = m_adapterInfoList[adapterIter];
		if (adapter->m_adapterOrdinal == adapterOrdinal)
			return adapter;
    }
    return NULL;
}

GraphicsEnumDeviceInfo* GraphicsAdapterEnumerator::GetDeviceInfo(
    uint32_t adapterOrdinal, D3D_DRIVER_TYPE deviceType)
{
	if (GraphicsEnumAdapterInfo* pAdapterInfo = GetAdapterInfo(adapterOrdinal)) {
		for (int deviceIter = 0; deviceIter < pAdapterInfo->m_deviceInfoList.size(); ++deviceIter) {
			GraphicsEnumDeviceInfo* pDeviceInfo = pAdapterInfo->m_deviceInfoList[deviceIter];
			if (pDeviceInfo->m_deviceType == deviceType)
				return pDeviceInfo;
		}
	}
	return NULL;
}

GraphicsEnumOutputInfo* GraphicsAdapterEnumerator::GetOutputInfo(
    uint32_t adapterOrdinal, uint32_t output)
{
    if (GraphicsEnumAdapterInfo* pAdapterInfo = GetAdapterInfo(adapterOrdinal))
        return pAdapterInfo->m_outputInfoList[output];
    return NULL;
}

GraphicsEnumDeviceSettingsCombo* GraphicsAdapterEnumerator::GetDeviceSettingsCombo(
    const GraphicsDeviceSettings* pDeviceSettings)
{
	if (!pDeviceSettings) return NULL;
    return GetDeviceSettingsCombo(
        pDeviceSettings->m_adapterOrdinal, pDeviceSettings->m_driverType, 
        pDeviceSettings->m_output, pDeviceSettings->m_swapChainDesc.BufferDesc.Format, 
        pDeviceSettings->m_swapChainDesc.Windowed);
}

GraphicsEnumDeviceSettingsCombo* GraphicsAdapterEnumerator::GetDeviceSettingsCombo(
    uint32_t adapterOrdinal, D3D_DRIVER_TYPE deviceType, uint32_t output, 
    DXGI_FORMAT backBufferFormat, BOOL windowed)
{
    if (GraphicsEnumAdapterInfo* pAdapterInfo = GetAdapterInfo(adapterOrdinal)) {
        for (int deviceIter = 0; deviceIter < pAdapterInfo->m_deviceSettingsComboList.size(); ++deviceIter) {
            GraphicsEnumDeviceSettingsCombo* pDeviceSettingsCombo = pAdapterInfo->m_deviceSettingsComboList[deviceIter];
            if (pDeviceSettingsCombo->m_backBufferFormat == backBufferFormat && 
                    pDeviceSettingsCombo->m_windowed == windowed)
                return pDeviceSettingsCombo;
        }
    }
    return NULL;
}
