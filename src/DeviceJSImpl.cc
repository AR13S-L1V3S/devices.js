#include "includes.h"

DeviceJSImpl* DeviceJSImpl::instance = nullptr;
DeviceJSImpl* DeviceJSImpl::getInstance()
{
	if (DeviceJSImpl::instance == nullptr)
		DeviceJSImpl::instance = devicejs_impl_t::getInstance();

	return DeviceJSImpl::instance;
}

DeviceJSImpl::DeviceJSImpl(void) {}
DeviceJSImpl::~DeviceJSImpl(void) {}