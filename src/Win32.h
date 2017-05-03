#ifndef _H_WIN32HELPER
#define _H_WIN32HELPER

namespace win32 {
	inline bool isWindows7OrHigher(void)
	{
		static DWORD major = 0, minor = 0;
		if (major == 0) {
			LPWKSTA_INFO_100 info = nullptr;
			if (NetWkstaGetInfo(nullptr, 100, reinterpret_cast<BYTE**>(&info)) != NERR_Success)
				return false;

			major = info->wki100_ver_major;
			minor = info->wki100_ver_minor;
		}

		return (major >= 6 && minor >= 1) || (major >= 7 && minor >= 0);
	}
}

#endif