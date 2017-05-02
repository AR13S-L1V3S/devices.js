#ifndef _H_TYPEHELPER_
#define _H_TYPEHELPER_

#ifdef _PLATFORMJS_WINDOWS_
class Win32DeviceJSImpl;
class Win32Device;
typedef Win32DeviceJSImpl devicejs_impl_t;
typedef Win32Device device_t;
#endif

#endif