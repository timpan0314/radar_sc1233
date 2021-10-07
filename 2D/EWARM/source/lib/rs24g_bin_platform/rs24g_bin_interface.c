/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_bin_interface.c
 */

#include <string.h>
#include "rs24g_bin_interface.h"
#include "dllsearch.h"

#if defined(__MBED__)
#define LOAD_DLL 0
#else
#define LOAD_DLL 0  //Tim
#endif

#if LOAD_DLL
static rs_ret_t error_bin_alloc(rs_bin_t *pbin)
{
	(void) pbin;
	return RS_ENOSUPPORT;
}

static rs_ret_t error_bin_free(rs_bin_t bin)
{
	(void) bin;
	return RS_ENOSUPPORT;
}

static rs_ret_t error_bin_load(rs_bin_t bin, const void *buf, uint32_t bufsize)
{
	(void) bin;
	(void) buf;
	(void) bufsize;
	return RS_ENOSUPPORT;
}

static rs_ret_t error_bin_save(rs_bin_t bin, void *buf, uint32_t bufsize)
{
	(void) bin;
	(void) buf;
	(void) bufsize;
	return RS_ENOSUPPORT;
}

static rs_ret_t error_bin_get_size(rs_bin_t bin, uint32_t *size)
{
	(void) bin;
	(void) size;
	return RS_ENOSUPPORT;
}

static rs_ret_t error_code2_alloc(rs_code2_t *pcode)
{
	(void) pcode;
	return RS_ENOSUPPORT;
}

static rs_ret_t error_code2_free(rs_code2_t *pcode)
{
	(void) pcode;
	return RS_ENOSUPPORT;
}

static rs_ret_t error_code2_load_minimum_from_bin(rs_code2_t code, rs_bin_t bin)
{
	(void) code;
	(void) bin;
	return RS_ENOSUPPORT;
}
#endif

RS_IF rs_ret_t rs_bin_interface_get(struct rs_bin_interface_s *pif)
{
#if LOAD_DLL
#define GETFUNC(x,api,err) { void *f; if (dllsearch_getfunc("rs24g_bin", #api, &f) == 0) { x = (api ## _t)f; } else { x = err; } }
#else
#define GETFUNC(x,api,err) { x = api; }
#endif
	GETFUNC(pif->rs_bin_alloc, rs_bin_alloc, error_bin_alloc)
	GETFUNC(pif->rs_bin_free, rs_bin_free, error_bin_free)
	GETFUNC(pif->rs_bin_load, rs_bin_load, error_bin_load)
	GETFUNC(pif->rs_bin_save, rs_bin_save, error_bin_save)
	GETFUNC(pif->rs_bin_get_size, rs_bin_get_size, error_bin_get_size)
	GETFUNC(pif->rs_code2_alloc, rs_code2_alloc, error_code2_alloc)
	GETFUNC(pif->rs_code2_free, rs_code2_free, error_code2_free)
	GETFUNC(pif->rs_code2_load_minimum_from_bin, rs_code2_load_minimum_from_bin, error_code2_load_minimum_from_bin)
#undef GETFUNC
	return RS_OK;
}
