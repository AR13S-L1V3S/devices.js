#ifndef _H_WIN32DIRECTSHOWDEVICEMANAGER_
#define _H_WIN32DIRECTSHOWDEVICEMANAGER_

class Win32DirectShowDeviceManager
{
private:
	Win32DirectShowDeviceManager(void);
	Win32DirectShowDeviceManager(Win32DirectShowDeviceManager&) = delete;
	~Win32DirectShowDeviceManager(void);

private:
	static bool getAvailableDevices(std::vector<Win32Device*>& devices, REFCLSID kind);

public:
	static bool getAvailableVideoDevices(std::vector<Win32Device*>& devices);
	static bool getAvailableAudioInputDevices(std::vector<Win32Device*>& devices);
	static bool getAvailableAudioOutputDevices(std::vector<Win32Device*>& devices);
};

#endif
