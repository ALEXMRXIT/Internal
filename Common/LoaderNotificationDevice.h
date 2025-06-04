#pragma once
class LoaderNotificationDevice {
public:
	bool m_device_loader;

public:
	LoaderNotificationDevice();
	virtual ~LoaderNotificationDevice() = default;

	void Reset();
};