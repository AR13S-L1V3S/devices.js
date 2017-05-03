#ifndef _H_WIN32DEVICEMONITOR_
#define _H_WIN32DEVICEMONITOR_

#include <combaseapi.h>
#include <MMDeviceAPI.h>

class Win32AudioMonitorTask;
class Win32DeviceMonitor : public IMMNotificationClient
{
public:
	Win32DeviceMonitor(Win32AudioMonitorTask*);
	virtual ~Win32DeviceMonitor(void) = default;

private:
	bool saveNewDeviceInformation(const std::wstring& deviceId);

public:
	STDMETHOD(QueryInterface)(const IID& riid, void** ppvObject) override;
	STDMETHOD_(ULONG, AddRef)(void) override;
	STDMETHOD_(ULONG, Release)(void) override;

private:
	STDMETHOD(OnDeviceStateChanged)(LPCWSTR pwstrDeviceId, DWORD dwNewState) override;
	STDMETHOD(OnDeviceAdded)(LPCWSTR pwstrDeviceId) override;
	STDMETHOD(OnDeviceRemoved)(LPCWSTR pwstrDeviceId) override;
	STDMETHOD(OnDefaultDeviceChanged)(EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDeviceId) override;
	STDMETHOD(OnPropertyValueChanged)(LPCWSTR pwstrDeviceId, const PROPERTYKEY key) override;

public:
	std::wregex regex;
	std::unordered_map<std::wstring, device_t*> devices;
	Win32AudioMonitorTask* task;
};

#endif
