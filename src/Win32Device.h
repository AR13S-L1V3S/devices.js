#ifndef _H_WIN32DEVICE_
#define _H_WIN32DEVICE_

class Win32Device : public Device
{
public:
	Win32Device(wchar_t* deviceId, wchar_t* label);
	Win32Device(const wchar_t* deviceId, const wchar_t* label);
	Win32Device(Win32Device&) = delete;

protected:
	~Win32Device(void);

public:
	string_t getDeviceId(void) override;
	string_t getLabel(void) override;
	void setDeviceId(std::string) override;
	void setLabel(std::string) override;

private:
	std::wstring deviceId;
	std::wstring label;
};

#endif
