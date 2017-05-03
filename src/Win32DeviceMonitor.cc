#include "includes.h"

#include <atlcomcli.h>
#include <Functiondiscoverykeys_devpkey.h>

#include "Win32DeviceMonitor.h"
#include "Win32AudioMonitorTask.h"

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

Win32DeviceMonitor::Win32DeviceMonitor(Win32AudioMonitorTask* task) 
	: task(task), regex(L"[0-9A-Fa-f]{8}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{12}")
{
}

bool Win32DeviceMonitor::saveNewDeviceInformation(const std::wstring& deviceId)
{
	CComPtr<IMMDeviceEnumerator> deviceEnumerator;
	HRESULT hr = CoCreateInstance(CLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL, IID_IMMDeviceEnumerator, reinterpret_cast<void**>(&deviceEnumerator));
	CERRORPR(FAILED(hr), false, "CoCreateInstance(CLSID_MMDeviceEnumerator, ...) failed with HRESULT (0x%08X)", hr);

	CComPtr<IMMDevice> device;
	hr = deviceEnumerator->GetDevice(deviceId.c_str(), &device);
	CERRORPR(FAILED(hr), false, "IMMDeviceEnumerator::GetDevice(...) failed with HRESULT (0x%08X)", hr);

	CComPtr<IMMEndpoint> endpoint;
	hr = device->QueryInterface(&endpoint);
	CERRORPR(FAILED(hr), false, "IMMEndpoint::QueryInterface(...) failed with HRESULT (0x%08X)", hr);

	EDataFlow dataFlow;
	hr = endpoint->GetDataFlow(&dataFlow);
	CERRORPR(FAILED(hr), false, "EDataFlow::GetDataFlow(...) failed with HRESULT (0x%08X)", hr);

	DeviceType type = static_cast<DeviceType>(0);
	switch (dataFlow)
	{
	case eCapture:
		type = audioinput;
		break;

	case eRender:
		type = audiooutput;
		break;

	default:
		CERRORPR(true, false, "Unknown data flow value (0x%08X)", dataFlow);
	}

	CComPtr<IPropertyStore> properties;
	hr = device->OpenPropertyStore(STGM_READ, &properties);
	CERRORPR(FAILED(hr), false, "IMMDevice::OpenPropertyStore(...) failed with HRESULT (0x%08X)", hr);

	std::wstring label = L"";
	PROPVARIANT variant;
	PropVariantInit(&variant);
	{
		hr = properties->GetValue(PKEY_Device_FriendlyName, &variant);
		if (SUCCEEDED(hr) && variant.vt == VT_LPWSTR && variant.pwszVal)
			label = variant.pwszVal;
	}
	PropVariantClear(&variant);

	std::wsmatch match;
	std::regex_search(deviceId, match, regex);

	std::wstring normalizedDeviceId = deviceId;
	if (match.size() > 0)
		normalizedDeviceId = match[0];

	for (auto& ch : normalizedDeviceId)
		ch = std::toupper(ch, std::locale());

	normalizedDeviceId.erase(std::remove(normalizedDeviceId.begin(), normalizedDeviceId.end(), L'-'), normalizedDeviceId.end());
	this->devices[deviceId] = new device_t(normalizedDeviceId.c_str(), label.c_str(), type);
	return true;
}

HRESULT Win32DeviceMonitor::QueryInterface(const IID& riid, void** ppvObject)
{
	if (riid == IID_IUnknown || riid == __uuidof(IMMNotificationClient))
	{
		*ppvObject = static_cast<IMMNotificationClient*>(this);
		return S_OK;
	}

	*ppvObject = nullptr;
	return E_NOINTERFACE;
}
ULONG Win32DeviceMonitor::AddRef(void)
{
	return 1;
}
ULONG Win32DeviceMonitor::Release(void)
{
	return 1;
}

HRESULT Win32DeviceMonitor::OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState)
{
	std::wstring deviceId = pwstrDeviceId;
	if (this->devices.find(deviceId) == this->devices.end())
		this->saveNewDeviceInformation(pwstrDeviceId);

	AudioMonitorEvent event(devices[deviceId], StateChanged, dwNewState);
	this->task->pushEvent(event);
	return S_OK;
}
HRESULT Win32DeviceMonitor::OnDeviceAdded(LPCWSTR pwstrDeviceId)
{
	std::wstring deviceId = pwstrDeviceId;
	if (this->devices.find(deviceId) == this->devices.end())
		this->saveNewDeviceInformation(pwstrDeviceId);

	AudioMonitorEvent event(devices[deviceId], Added);
	this->task->pushEvent(event);
	return S_OK;
}
HRESULT Win32DeviceMonitor::OnDeviceRemoved(LPCWSTR pwstrDeviceId)
{
	std::wstring deviceId = pwstrDeviceId;
	if (this->devices.find(deviceId) == this->devices.end())
		this->saveNewDeviceInformation(pwstrDeviceId);

	AudioMonitorEvent event(devices[deviceId], Removed);
	this->task->pushEvent(event);
	return S_OK;
}
HRESULT Win32DeviceMonitor::OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDeviceId)
{
	if ((role != eConsole && role != eCommunications) || (flow != eRender && flow != eCapture) || pwstrDefaultDeviceId == NULL)
		return S_OK;

	std::wstring deviceId = pwstrDefaultDeviceId;
	if (this->devices.find(deviceId) == this->devices.end())
		this->saveNewDeviceInformation(pwstrDefaultDeviceId);

	AudioMonitorEvent event(devices[deviceId], DefaultDeviceChanged);
	this->task->pushEvent(event);
	return S_OK;
}
HRESULT Win32DeviceMonitor::OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key)
{
	return S_OK;
}