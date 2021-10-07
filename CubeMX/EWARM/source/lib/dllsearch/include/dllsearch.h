/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * dllsearch.h
 */

#ifndef __DLLSEARCH_H__
#define __DLLSEARCH_H__

#ifdef WIN32
#include <windows.h>
#endif

/*
 * Type Definition
 */
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

	extern int dllsearch_getfunc(const char *libname, const char *funcname, void **func);
	extern int dllsearch_getfunc_nc(const char *libname, const char *funcname, void **func);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#ifdef __cplusplus

template <typename Tfunc, typename Tret, class... Args>
static inline bool dllsearch_call_inner(const char *libname, bool name_conv, const char *funcname, Tret *ret, Args... args)
{
	Tfunc *func = nullptr;

	if(name_conv == true) {
		if(dllsearch_getfunc_nc(libname, funcname, reinterpret_cast<void **>(&func)) != 0) {
			return false;
		}
	} else {
		if(dllsearch_getfunc(libname, funcname, reinterpret_cast<void **>(&func)) != 0) {
			return false;
		}
	}

	*ret = func(args ...);

	return true;
}

template <typename Tfunc, typename Tret, class... Args>
static inline Tret dllsearch_call(const char *libname, bool name_conv, const char *funcname, Tret err_ret, Args... args)
{
	Tret ret;
	if(!dllsearch_call_inner<Tfunc, Tret>(libname, name_conv, funcname, &ret, args ...)) {
		return err_ret;
	}

	return ret;
}

#endif  /* __cplusplus */

#endif	/* __DLLSEARCH_H__ */
