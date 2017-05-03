#include "includes.h"
#include "Win32DirectShowDeviceManager.h"

#include <MMDeviceAPI.h>
#include <mmsystem.h>
#include <Functiondiscoverykeys_devpkey.h>  // MMDeviceAPI.h must come first

#include <dshow.h>
#include <atlcomcli.h>

#pragma comment(lib, "strmiids")

Win32DirectShowDeviceManager::Win32DirectShowDeviceManager(void) {}
Win32DirectShowDeviceManager::~Win32DirectShowDeviceManager(void) {}

bool Win32DirectShowDeviceManager::getAvailableDevices(std::vector<Win32Device*>& devices, REFCLSID kind)
{
	HRESULT hr = S_OK;
	CComPtr<ICreateDevEnum> deviceEnumeration;
	DeviceType type = static_cast<DeviceType>(0);
	
	if (IsEqualGUID(kind, CLSID_VideoInputDeviceCategory) == TRUE)
		type = videoinput;
	else if (IsEqualGUID(kind, CLSID_AudioInputDeviceCategory) == TRUE)
		type = audioinput;

	hr = CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC, IID_PPV_ARGS(&deviceEnumeration));
	CERRORPR(FAILED(hr), false, "CoCreateInstance(CLSID_SystemDeviceEnum, ...) failed with HRESULT 0x%08X", hr);

	CComPtr<IEnumMoniker> enumerateMoniker;
	hr = deviceEnumeration->CreateClassEnumerator(kind, &enumerateMoniker, 0);
	CERRORPR(FAILED(hr), false, "IEnumMoniker::CreateClassEnumerator(CLSID_VideoInputDeviceCategory, ...) failed with HRESULT 0x%08X", hr);

	std::wregex regex(L"[0-9A-Fa-f]{8}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{12}");
	std::wsmatch match;

	while (true) {
		CComPtr<IMoniker> moniker;
		hr = enumerateMoniker->Next(1, &moniker, nullptr);
		if (hr == S_FALSE)
			break;

		CComPtr<IPropertyBag> propertyBag;
		hr = moniker->BindToStorage(nullptr, nullptr, IID_IPropertyBag, reinterpret_cast<void**>(&propertyBag));
		if (FAILED(hr)) {
			continue;
		}

		CComVariant variant;
		hr = propertyBag->Read(L"Description", &variant, nullptr);
		if (FAILED(hr))
			hr = propertyBag->Read(L"FriendlyName", &variant, nullptr);

		if (FAILED(hr)) {
			continue;
		}

		std::wstring label = variant.bstrVal;

		variant.Clear();
		hr = propertyBag->Read(L"DevicePath", &variant, nullptr);
		if (FAILED(hr)) {
			continue;
		}

		std::wstring deviceId = variant.bstrVal;
		std::regex_search(deviceId, match, regex);

		if (match.size() > 0)
			deviceId = match[0];

		for (auto& ch : deviceId)
			ch = std::toupper(ch, std::locale());

		deviceId.erase(std::remove(deviceId.begin(), deviceId.end(), L'-'), deviceId.end());
		devices.push_back(new Win32Device(deviceId.c_str(), label.c_str(), type));
	}

	return true;
}

bool Win32DirectShowDeviceManager::getAvailableVideoDevices(std::vector<Win32Device*>& devices)
{
	return Win32DirectShowDeviceManager::getAvailableDevices(devices, CLSID_VideoInputDeviceCategory);
}
bool Win32DirectShowDeviceManager::getAvailableAudioInputDevices(std::vector<Win32Device*>& devices)
{
	return Win32DirectShowDeviceManager::getAvailableDevices(devices, CLSID_AudioInputDeviceCategory);
}
bool Win32DirectShowDeviceManager::getAvailableAudioOutputDevices(std::vector<Win32Device*>& devices)
{
	HRESULT hr = S_OK;
	CComPtr<IMMDeviceEnumerator> deviceEnumerator;
	
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&deviceEnumerator));
	CERRORPR(FAILED(hr), false, "CoCreateInstance(__uuidof(MMDeviceEnumerator), ...) failed with HRESULT 0x%08X", hr);

	CComPtr<IMMDeviceCollection> collection;
	hr = deviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &collection);
	CERRORPR(FAILED(hr), false, "IMMDeviceEnumerator::EnumAudioEndpoints(...) failed with HRESULT 0x%08X", hr);

	UINT activeDeviceCount = 0;
	collection->GetCount(&activeDeviceCount);
	if (activeDeviceCount == 0)
		return false;

	std::wregex regex(L"[0-9A-Fa-f]{8}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{12}");
	std::wsmatch match;

	for (UINT i = 0; i < activeDeviceCount; ++i) {
		CComPtr<IMMDevice> device;
		hr = collection->Item(i, &device);
		if (FAILED(hr))
			continue;

		wchar_t* rawDeviceId = nullptr;
		device->GetId(&rawDeviceId);
		
		std::wstring deviceId = rawDeviceId;
		CComPtr<IPropertyStore> properties;
		hr = device->OpenPropertyStore(STGM_READ, &properties);
		if (FAILED(hr))
			continue;

		std::wstring label = L"";
		PROPVARIANT variant;
		PropVariantInit(&variant);
		{
			hr = properties->GetValue(PKEY_Device_FriendlyName, &variant);
			if (SUCCEEDED(hr) && variant.vt == VT_LPWSTR && variant.pwszVal)
				label = variant.pwszVal;
		}
		PropVariantClear(&variant);

		std::regex_search(deviceId, match, regex);

		if (match.size() > 0)
			deviceId = match[0];

		for (auto& ch : deviceId)
			ch = std::toupper(ch, std::locale());

		deviceId.erase(std::remove(deviceId.begin(), deviceId.end(), L'-'), deviceId.end());
		devices.push_back(new Win32Device(deviceId.c_str(), label.c_str(), audiooutput));
	}

	return true;
}
