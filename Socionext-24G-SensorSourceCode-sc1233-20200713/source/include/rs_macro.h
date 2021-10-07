/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file rs_macro.h
 * call function and return depends on return value from the function.
 */

#ifndef __RS_MACRO_H__
#define __RS_MACRO_H__

#include <stdio.h>
#include <stdint.h>

//#define rs_macro_printf(fmt, ...)      fprintf(stdout, "%s:%d: " fmt, __FILE__, __LINE__, __VA_ARGS__); fflush(stdout)
#define rs_macro_printf(fmt, ...)


#if (__cplusplus >= 201103L || (defined(_MSC_VER) && defined(__cplusplus))) && !defined(__RS_C_RET_MACRO__)
#  define RS_MACRO_USE_AUTO_TYPE 1
#else
#  define RS_MACRO_USE_AUTO_TYPE 0
#endif

/**
 * call *api*, return *retval* if return value of *api* is not *refval*.
 *
 * @param[in] type type of return value (if available)
 * @param[in] api callee function
 * @param[in] refval expected return value from api
 * @param[in] retval return value if return value is not refval
 */
#if RS_MACRO_USE_AUTO_TYPE
#define RET_CONV(api, refval, retval)												\
{																					\
	const auto macroret = (api);													\
	if(macroret != refval){															\
		rs_macro_printf(															\
			"(%s) [error] ref=%x ret=%x\n",											\
			__FUNCTION__,															\
			(uint32_t) refval, (uint32_t) macroret									\
		);																			\
		return retval;																\
	}																				\
}
#else
#define RET_CONV(type, api, refval, retval)											\
{																					\
	const type macroret = (api);													\
	if(macroret != refval){															\
		rs_macro_printf(															\
			"(%s) [error] ref=%x ret=%x\n",											\
			__FUNCTION__,															\
			(uint32_t) refval, (uint32_t) macroret									\
		);																			\
		return retval;																\
	}																				\
}
#endif

/**
 * evaluate *api*, return *retval* if the evaluation value is false.
 *
 * @param[in] api expression to evaluate
 * @param[in] retval return vale on false
 */
#if RS_MACRO_USE_AUTO_TYPE
#define RET_CHECK(api, retval)		RET_CONV(api, true, retval)
#else
#define RET_CHECK(api, retval)		RET_CONV(int, api, !0, retval)
#endif

/**
 * call *api*, return return value of *api* if the return value is not *refval*.
 *
 * @param[in] type type of return value (if available)
 * @param[in] api callee function
 * @param[in] refval expected return value from api
 */
#if RS_MACRO_USE_AUTO_TYPE
#define RET_ORG(api, refval)														\
{																					\
	const auto macroret = (api);													\
	if(macroret != refval){															\
		rs_macro_printf(															\
			"(%s) [error] ref=%x ret=%x\n",											\
			__FUNCTION__,															\
			(uint32_t) refval, (uint32_t) macroret									\
		);																			\
		return macroret;															\
	}																				\
}
#else
#define RET_ORG(type, api, refval)													\
{																					\
	const type macroret = (api);													\
	if(macroret != refval){															\
		rs_macro_printf(															\
			"(%s) [error] ref=%x ret=%x\n",											\
			__FUNCTION__,															\
			(uint32_t) refval, (uint32_t) macroret									\
		);																			\
		return macroret;															\
	}																				\
}
#endif


/**
 * evaluate *api*, return *retval* if the evaluation value is false.
 *
 * @param[in] api expression to evaluate
 * @param[in] retval return vale on false
 */
#if defined(__cplusplus)
#  if RS_MACRO_USE_AUTO_TYPE
#    define RS_ASSERT(api, retval) RET_CONV(api, true, retval)
#  else
#    define RS_ASSERT(api, retval) RET_CONV(bool, api, true, retval)
#  endif
#else
#  define RS_ASSERT(api, retval) RET_CONV(int, api, !0, retval)
#endif

/**
 * call *api*, return return value of *api* if the return value is not RS_OK.
 *
 * @param[in] api callee function
 */
#if RS_MACRO_USE_AUTO_TYPE
#  define RS_CALL(api) RET_ORG(api, RS_OK)
#else
#  define RS_CALL(api) RET_ORG(rs_ret_t, api, RS_OK)
#endif

#undef RS_MACRO_USE_AUTO_TYPE

#endif	/* __RS_MACRO_H__ */
