/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * dllsearch.cpp
 */

#ifdef WIN32
#include <windows.h>
#include <tchar.h>
#include <vector>
#else
#include <dlfcn.h>   
#endif
#include <cstdint>   
#include <map>
#include <string>

#include "dllsearch.h"

//#define dllsearch_dprintf(...)		{printf(__VA_ARGS__);fflush(stdout);}
#define dllsearch_dprintf(...)



#ifdef WIN32
typedef HMODULE libhndl;
#else
typedef void *libhndl;
#endif


#ifdef WIN32
static inline std::wstring utf8towstr(const std::string &istr)
{
	const auto bufsize = ::MultiByteToWideChar(CP_UTF8, 0, istr.c_str() , -1, nullptr, 0);

	std::vector<wchar_t> ostr_buf(bufsize, 0);
	::MultiByteToWideChar(CP_UTF8, 0, istr.c_str(), -1, ostr_buf.data(), bufsize);
	std::wstring ostr(ostr_buf.data(), ostr_buf.data() + bufsize - 1);

	return ostr;
}
#endif

class dllsearch
{
private:
	std::map<std::string, libhndl> lib_list;
	std::map<std::string, void *> func_list;

public:
	dllsearch(void);
	~dllsearch(void);

private:
	template <typename T>
	bool find(std::map<std::string, T> &list, const std::string &str, T *val);

public:
	static std::string rename_libname(const char *libname);
	bool getfunc(const char *libname, const char *funcname, void **func);
};


dllsearch::dllsearch(void) :
	lib_list({}),
	func_list({})
{
}

dllsearch::~dllsearch(void)
{
	for(auto it=lib_list.begin(); it!=lib_list.end(); it++) {
#ifdef WIN32
		::FreeLibrary(it->second);
#else
		::dlclose(it->second);
#endif
	}
}

template <typename T>
bool dllsearch::find(std::map<std::string, T> &list, const std::string &str, T *val)
{
	*val = nullptr;

	if(list.find(str) != list.end()) {
		*val = list[str];
		return true;
	}

	return false;
}

std::string dllsearch::rename_libname(const char *libname)
{
#ifdef WIN32
#ifdef _WIN64
	const std::string libfullname = libname + std::string("_64.dll");
#else
	const std::string libfullname = libname + std::string(".dll");
#endif
#else
	const std::string libfullname = std::string("lib") + libname + std::string(".so");
#endif

	return std::move(libfullname);
}

bool dllsearch::getfunc(const char *libname, const char *funcname, void **func)
{
	const std::string fullname = libname + std::string("+") + funcname;

	if(find(func_list, fullname, func) == false) {
		libhndl handle = nullptr;
		if(find(lib_list, libname, &handle) == false) {
#ifdef WIN32
			handle = ::LoadLibrary(utf8towstr(libname).c_str());
#else
			handle = ::dlopen(libname, RTLD_LAZY);
#endif
			if(handle == nullptr){
				dllsearch_dprintf("dlopen error: %s\n", libname);
#ifdef WIN32
				dllsearch_dprintf("error val: %d\n", ::GetLastError());
#else
				dllsearch_dprintf("dlerror: %s\n", dlerror());
#endif
				return false;
			}
			dllsearch_dprintf("open lib: %s\n", libname);
			lib_list.insert(std::make_pair(libname, handle));
		} else {
			dllsearch_dprintf("find lib: %s\n", libname);
		}

#ifdef WIN32
		*func = ::GetProcAddress(handle, funcname);
#else
		*func = ::dlsym(handle, funcname);
#endif
		if(*func == nullptr){
			dllsearch_dprintf("dlsym error: %s %s\n", libname, funcname);
#ifdef WIN32
			dllsearch_dprintf("error val: %d\n", ::GetLastError());
#else
			dllsearch_dprintf("dlerror: %s\n", dlerror());
#endif
			return false;
		}
		dllsearch_dprintf("sym func: %s\n", funcname);

		func_list.insert(std::make_pair(fullname, *func));
	} else {
		dllsearch_dprintf("find func: %s\n", funcname);
	}

	return true;
}

static class dllsearch local_dllsearch;

int dllsearch_getfunc(const char *libname, const char *funcname, void **func)
{
	auto ret = local_dllsearch.getfunc(dllsearch::rename_libname(libname).c_str(), funcname, func);

	return ret == true ? 0 : -1;
}

int dllsearch_getfunc_nc(const char *libname, const char *funcname, void **func)
{
	auto ret = local_dllsearch.getfunc(libname, funcname, func);

	return ret == true ? 0 : -1;
}
