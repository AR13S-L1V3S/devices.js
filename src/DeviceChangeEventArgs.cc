#include "includes.h"

DeviceChangeEventArgs::DeviceChangeEventArgs(device_t* target, std::wstring which)
	: target(target), which(which) {}

DeviceChangeEventArgs::~DeviceChangeEventArgs(void) {}

device_t* DeviceChangeEventArgs::getTarget(void)
{
	return this->target;
}
string_t DeviceChangeEventArgs::getWhich()
{
	return MAKE_STRING(this->which.c_str());
}

void DeviceChangeEventArgs::setTarget(device_t* device)
{
	this->target = device;
}
void DeviceChangeEventArgs::setWhich(std::string)
{
}

#if defined(BUILDING_NODE_EXTENSION)
NBIND_CLASS(DeviceChangeEventArgs)
{
	getset(getTarget, setTarget);
	getset(getWhich, setWhich);
}
#endif