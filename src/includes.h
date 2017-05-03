#ifndef _H_INCLUDES_
#define _H_INCLUDES_

#if defined(_MSC_VER)
#	pragma optimize("gty", on)
#	define _PLATFORMJS_WINDOWS_
#	define _WIN_VER 0x0A00
#	undef _WIN32_WINNT
#	define _WIN32_WINNT 0x0A00
#endif

#include <iostream>
#include <unordered_map>
#include <queue>
#include <memory>
#include <locale>
#include <regex>

#include "nbind/nbind.h"
#include "nan.h"
#include "v8.h"

#include "TypeHelper.h"
#include "nbindHelper.h"

#include "ScopedLock.h"
#include "Device.h"
#include "DeviceChangeEventArgs.h"
#include "DeviceJSImpl.h"
#include "DeviceJS.h"

#ifdef _PLATFORMJS_WINDOWS_
#	include "win32_includes.h"
#endif

#endif