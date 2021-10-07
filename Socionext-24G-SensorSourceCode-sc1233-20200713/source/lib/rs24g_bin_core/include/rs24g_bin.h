/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_bin.h
 */

#ifndef __RS24G_BIN_H__
#define __RS24G_BIN_H__

#include "rs24g_types.h"

/*
 * Type Definition
 */
typedef struct rs_bin_s *rs_bin_t;

/*
 * Constant Definition
 */

/*
 * Struct Definition
 */

/*
 * Function Definition
 */

RS_IF rs_ret_t rs_bin_alloc(rs_bin_t *pbin);
RS_IF rs_ret_t rs_bin_free(rs_bin_t bin);
RS_IF rs_ret_t rs_bin_load(rs_bin_t bin, const void *buf, uint32_t bufsize);

#endif /* __RS24G_BIN_H__ */
