#ifndef _H_DEVICE_
#define _H_DEVICE_

class Device
{
protected:
	Device(void);
	Device(Device&) = delete;
	~Device(void);

public:
	virtual string_t getDeviceId(void) = 0;
	virtual string_t getLabel(void) = 0;
	virtual void setDeviceId(std::string) = 0;
	virtual void setLabel(std::string) = 0;
};

#endif