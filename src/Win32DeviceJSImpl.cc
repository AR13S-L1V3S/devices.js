#include "includes.h"

#include "Win32DeviceMonitor.h"
#include "Win32AudioMonitorTask.h"
#include "Win32DirectShowDeviceManager.h"

Win32DeviceJSImpl* Win32DeviceJSImpl::instance = nullptr;
Win32DeviceJSImpl* Win32DeviceJSImpl::getInstance(void)
{
	if (Win32DeviceJSImpl::instance == nullptr)
		Win32DeviceJSImpl::instance = new Win32DeviceJSImpl;

	return Win32DeviceJSImpl::instance;
}
bool Win32DeviceJSImpl::checkCompatible()
{
	if (win32::isWindows7OrHigher() == false) {
		NBIND_ERR("You must use Windows 7 or higher OS to use devices.js");
		return false;
	}

	return true;
}

Win32DeviceJSImpl::Win32DeviceJSImpl(void)
	: ondevicechange(nullptr), audioMonitorTask(nullptr)
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
}
Win32DeviceJSImpl::~Win32DeviceJSImpl(void)
{
	CoUninitialize();
}

bool Win32DeviceJSImpl::enumerateDevices(nbind::cbFunction& callback)
{
	IMFAttributes* attributes = nullptr;
	IMFActivate** activate = nullptr;

	std::vector<Win32Device*> devices;
#ifdef _DEVICEJS_USE_MEDIA_FOUNDATION_
	HRESULT hr = MFCreateAttributes(&attributes, 1);
	CERRORPR(FAILED(hr), false, "MFCreateAttributes(...) error occured with HRESULT 0x%08X", hr);

	hr = attributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
	CERRORPR(FAILED(hr), false, "IMFAttributes::SetGUID(...) error occured with HRESULT 0x%08X", hr);

	UINT count = 0;
	hr = MFEnumDeviceSources(attributes, &activate, &count);
	CERRORPR(FAILED(hr), false, "MFEnumDeviceSources(...) error occured with HRESULT 0x%08X", hr);

	for (UINT i = 0; i < count; ++i) {
		wchar_t* label = nullptr, *deviceId = nullptr;
		std::uint32_t labelLength = 0, deviceIdLength = 0;

		hr = activate[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &label, &labelLength);
		if (FAILED(hr))
			continue;

		hr = activate[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, &deviceId, &deviceIdLength);
		if (FAILED(hr))
			continue;

		std::wstring labelString(label, labelLength), deviceIdString(deviceId, deviceIdLength);
		devices.push_back(new Win32Device(deviceIdString.c_str(), labelString.c_str()));

		activate[i]->Release();
	}
#else
	Win32DirectShowDeviceManager::getAvailableVideoDevices(devices);
	Win32DirectShowDeviceManager::getAvailableAudioInputDevices(devices);
	Win32DirectShowDeviceManager::getAvailableAudioOutputDevices(devices);
#endif
	
	callback.call<void>(devices);
	return true;
}
void Win32DeviceJSImpl::setOnDeviceChangeCallback(nbind::cbFunction& callback)
{
	if (this->ondevicechange != nullptr) {
		delete this->ondevicechange;
	}

	this->ondevicechange = new Nan::Callback(callback.getJsFunction());

	if (this->audioMonitorTask == nullptr) {
		this->audioMonitorTask = new Win32AudioMonitorTask(this);
		Nan::AsyncQueueWorker(this->audioMonitorTask);
	} else {
		this->audioMonitorTask->swapCallback(callback);
	}
}

Nan::Callback* Win32DeviceJSImpl::getOnDeviceChangeCallback()
{
	return this->ondevicechange;
}

void Win32DeviceJSImpl::release()
{
	if (this->ondevicechange != nullptr)
		delete this->ondevicechange;
}
