#include "includes.h"

Win32Device::Win32Device(wchar_t* deviceId, wchar_t* label) :
	deviceId(deviceId), label(label)
{
}
Win32Device::Win32Device(const wchar_t* deviceId, const wchar_t* label) :
	deviceId(deviceId), label(label)
{
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

void Win32Device::setDeviceId(std::string) { }
void Win32Device::setLabel(std::string) {}

#if defined(BUILDING_NODE_EXTENSION)
NBIND_CLASS(Win32Device)
{
	getset(getDeviceId, setDeviceId);
	getset(getLabel, setLabel);
}
#endif