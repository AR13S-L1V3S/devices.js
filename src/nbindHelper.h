#ifndef _H_NBIND_HELPER
#define _H_NBIND_HELPER

#if defined(BUILDING_NODE_EXTENSION)
#	include "nbind/api.h"
	using Nan::New;
	typedef std::string string_t;
#	define MAKE_STRING(x) DeviceJS::encode(std::vector<short>(x, x + wcslen(x)))
#else
#	define NBIND_ERR(x) printf(x);
	namespace nbind {
		class cbFunction
		{
		public:
			cbFunction(...) {};

			template <class T>
			T call(...) { return T(); };

			v8::Local<v8::Function> getJsFunction(void) const { return {}; };
		};

		class Buffer
		{
		public:
			Buffer(unsigned char *ptr = nullptr, size_t len = 0) {}
			unsigned char* data() const { return nullptr; }
			size_t length() const { return 0; }
		};
	}

	template <class T>
	v8::MaybeLocal<T> New(...) { return {}; };

	typedef const wchar_t* string_t;
#	define MAKE_STRING(x) x

#endif

#endif