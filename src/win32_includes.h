#ifndef _H_WIN32_INCLUDES_
#define _H_WIN32_INCLUDES_

#include <windows.h>
#include <Lm.h>
#include <mfapi.h>
#include <mferror.h>
#include <mfidl.h>

#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")

#include "Win32.h"
#include "Win32Device.h"
#include "Win32DeviceJSImpl.h"

#define CERRORP(x, str, ...) if (x) { printf(str, __VA_ARGS__); }
#define CERRORPR(x, ret, str, ...) if (x) { printf(str, __VA_ARGS__); return ret; }
#define SAFE_RELEASE(x) { x->Release(); x = nullptr; }

#endif