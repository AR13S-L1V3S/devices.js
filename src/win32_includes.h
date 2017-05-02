#ifndef _H_WIN32_INCLUDES_
#define _H_WIN32_INCLUDES_

#include <windows.h>
#include <WinNls.h>
#include <Synchapi.h>
#include <atlbase.h>

#pragma comment(lib, "uuid.lib")

#include "Win32Device.h"
#include "Win32DeviceJSImpl.h"

#define COM_ERRORP(x, str, ...) if (FAILED(x)) { printf(str, __VA_ARGS__); }
#define COM_ERRORPR(x, ret, str, ...) if (FAILED(x)) { printf(str, __VA_ARGS__); return ret; }

#endif