#ifndef _H_WIN32DEVICEJSIMPL_
#define _H_WIN32DEVICEJSIMPL_

class Win32AudioMonitorTask;
class Win32DeviceJSImpl : public DeviceJSImpl
{
private:
	static Win32DeviceJSImpl* instance;

public:
	static Win32DeviceJSImpl* getInstance(void);
	static bool checkCompatible(void);

private:
	Win32DeviceJSImpl(void);
	Win32DeviceJSImpl(Win32DeviceJSImpl&) = delete;
	~Win32DeviceJSImpl(void);

public:
	virtual bool enumerateDevices(nbind::cbFunction& callback) override;
	void setOnDeviceChangeCallback(nbind::cbFunction& callback) override;
	
public:
	Nan::Callback* getOnDeviceChangeCallback(void);

public:
	virtual void release(void) override;

private:
	Nan::Callback* ondevicechange;
	Win32AudioMonitorTask* audioMonitorTask;
};

#endif
