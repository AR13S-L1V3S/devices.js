#ifndef _H_DEVICEJSIMPL_
#define _H_DEVICEJSIMPL_


class DeviceJSImpl
{
private:
	static DeviceJSImpl* instance;

public:
	static DeviceJSImpl* getInstance(void);

protected:
	DeviceJSImpl(void);
	DeviceJSImpl(DeviceJSImpl&) = delete;
	~DeviceJSImpl(void);

public:
	virtual bool enumerateDevices(nbind::cbFunction& callback) = 0;
	virtual void setOnDeviceChangeCallback(nbind::cbFunction& callback) = 0;

private:
	virtual void release(void) = 0;
};

#endif