#ifndef _H_DEVICEJS_
#define _H_DEVICEJS_

class DeviceJS
{
private:
	static DeviceJS* instance;

public:
	static DeviceJS* getInstance(nbind::cbFunction& encodeFunction);
	static bool checkCompatible(void);
	static std::string encode(std::vector<short>& string);

private:
	DeviceJS(nbind::cbFunction&);
	DeviceJS(DeviceJS&) = delete;
	~DeviceJS(void);

public:
	bool enumerateDevices(nbind::cbFunction& callback);
	void setOnDeviceChangeCallback(nbind::cbFunction& callback);

private:
	DeviceJSImpl* impl;
	nbind::cbFunction encodeFunction;
};

#endif