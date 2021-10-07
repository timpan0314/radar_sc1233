/*
 * Copyright (c) 2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_ctl_fifo.h
 */

#ifndef __RS24G_CTL_FIFO_H__
#define __RS24G_CTL_FIFO_H__

#include <stdint.h>

#include "rs24g_types.h"

#include "rs24g_ctl_base.h"

/*
 * Type Definition
 */

/*
 * Constant Definition
 */
#define RS_MODE_FIFO							((rs_mode_t) (3))

/*
 * Struct Definition
 */
struct rs_fifo_info
{
	uint32_t status;
	uint32_t header;
	uint32_t end;
};

/*
 * Function Definition
 */
RS_IF rs_ret_t rs_get_fifo_size(rs_handle_t handle, rs_size_t *size);
RS_IF rs_ret_t rs_read_fifo(rs_handle_t handle, uint32_t timeout, uint8_t *data, rs_size_t size, struct rs_fifo_info *info);

#endif	/* __RS24G_CTL_FIFO_H__ */
