#include "includes.h"

DeviceJS* DeviceJS::instance = nullptr;

DeviceJS* DeviceJS::getInstance(nbind::cbFunction& encodeFunction)
{
	if (DeviceJS::instance == nullptr)
		DeviceJS::instance = new DeviceJS(encodeFunction);

	return DeviceJS::instance;
}

std::string DeviceJS::encode(std::vector<short>& string)
{
	return DeviceJS::instance->encodeFunction.call<std::string>(string);
}

DeviceJS::DeviceJS(nbind::cbFunction& encodeFunction)
	: impl(devicejs_impl_t::getInstance()), encodeFunction(encodeFunction)
{
}
DeviceJS::~DeviceJS(void) {}

bool DeviceJS::enumerateDevices(nbind::cbFunction& callback)
{
	return impl->enumerateDevices(callback);
}
void DeviceJS::setOnDeviceChangeCallback(nbind::cbFunction& callback)
{
	impl->setOnDeviceChangeCallback(callback);
}

#if defined(BUILDING_NODE_EXTENSION)
NBIND_CLASS(DeviceJS)
{
	method(getInstance);
	method(enumerateDevices);
	method(setOnDeviceChangeCallback);
}
#endif