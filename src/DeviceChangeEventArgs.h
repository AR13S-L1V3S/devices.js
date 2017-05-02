#ifndef _H_DEVICECHANGEEVENTARGS_
#define _H_DEVICECHANGEEVENTARGS_

class DeviceChangeEventArgs
{
public:
	DeviceChangeEventArgs(device_t* target, std::wstring which);
	DeviceChangeEventArgs(DeviceChangeEventArgs&) = delete;
	~DeviceChangeEventArgs(void);

public:
	device_t* getTarget(void);
	string_t getWhich(void);

public:
	void setTarget(device_t* device);
	void setWhich(std::string);

private:
	device_t* target;
	std::wstring which;
};

#endif