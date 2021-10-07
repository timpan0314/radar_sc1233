/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_bin_interface.h
 */

#ifndef __RS24G_BIN_INTERFACE_H__
#define __RS24G_BIN_INTERFACE_H__

#include "rs24g_bin_local.h"
#include "rs24g_code2.h"

/*
 * Type Definition
 */
typedef rs_ret_t (*rs_bin_alloc_t)(rs_bin_t *pbin);
typedef rs_ret_t (*rs_bin_free_t)(rs_bin_t bin);
typedef rs_ret_t (*rs_bin_load_t)(rs_bin_t bin, const void *buf, uint32_t bufsize);
typedef rs_ret_t (*rs_bin_save_t)(rs_bin_t bin, void *buf, uint32_t bufsize);
typedef rs_ret_t (*rs_bin_get_size_t)(rs_bin_t bin, uint32_t *size);
typedef rs_ret_t (*rs_code2_alloc_t)(rs_code2_t *pcode);
typedef rs_ret_t (*rs_code2_free_t)(rs_code2_t *pcode);
typedef rs_ret_t (*rs_code2_load_minimum_from_bin_t)(rs_code2_t code, rs_bin_t bin);

/*
 * Constant Definition
 */

/*
 * Struct Definition
 */
struct rs_bin_interface_s
{
	rs_bin_alloc_t rs_bin_alloc;
	rs_bin_free_t rs_bin_free;
	rs_bin_load_t rs_bin_load;
	rs_bin_save_t rs_bin_save;
	rs_bin_get_size_t rs_bin_get_size;
	rs_code2_alloc_t rs_code2_alloc;
	rs_code2_free_t rs_code2_free;
	rs_code2_load_minimum_from_bin_t rs_code2_load_minimum_from_bin;
};

/*
 * Function Definition
 */
RS_IF rs_ret_t rs_bin_interface_get(struct rs_bin_interface_s *pif);

#endif /* __RS24G_BIN_INTERFACE_H__ */
