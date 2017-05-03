#include "includes.h"

Win32Device::Win32Device(wchar_t* deviceId, wchar_t* label, DeviceType type) :
	deviceId(deviceId), label(label)
{
	switch (type) 
	{
	case audioinput:
		this->kind = L"audioinput";
		break;

	case audiooutput:
		this->kind = L"audiooutput";
		break;

	case videoinput:
		this->kind = L"videoinput";
		break;
	}
}
Win32Device::Win32Device(const wchar_t* deviceId, const wchar_t* label, DeviceType type) :
	deviceId(deviceId), label(label)
{
	switch (type)
	{
	case audioinput:
		this->kind = L"audioinput";
		break;

	case audiooutput:
		this->kind = L"audiooutput";
		break;

	case videoinput:
		this->kind = L"videoinput";
		break;
	}
}
Win32Device::~Win32Device(void) {}

string_t Win32Device::getDeviceId()
{
	return MAKE_STRING(this->deviceId.c_str());
}
string_t Win32Device::getLabel()
{
	return MAKE_STRING(this->label.c_str());
}
string_t Win32Device::getKind()
{
	return MAKE_STRING(this->kind.c_str());
}

void Win32Device::setDeviceId(std::string) {}
void Win32Device::setLabel(std::string) {}
void Win32Device::setKind(std::string) {}

#if defined(BUILDING_NODE_EXTENSION)
NBIND_CLASS(Win32Device)
{
	getset(getDeviceId, setDeviceId);
	getset(getLabel, setLabel);
	getset(getKind, setKind);
}
#endif