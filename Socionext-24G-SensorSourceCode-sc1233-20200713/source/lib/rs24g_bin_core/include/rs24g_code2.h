/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_code2.h
 */

#ifndef __RS24G_CODE2_H__
#define __RS24G_CODE2_H__

#include "rs24g_types.h"
#include "rs24g_bin.h"

/*
 * Type Definition
 */
typedef struct rs_code2_s *rs_code2_t;

/*
 * Constant Definition
 */

/*
 * Struct Definition
 */

/*
 * Function Definition
 */
RS_IF rs_ret_t rs_code2_alloc(rs_code2_t *pcode);
RS_IF rs_ret_t rs_code2_free(rs_code2_t *pcode);
RS_IF rs_ret_t rs_code2_load_minimum_from_bin(rs_code2_t code, rs_bin_t bin);

#endif /* __RS24G_CODE2_H__ */
