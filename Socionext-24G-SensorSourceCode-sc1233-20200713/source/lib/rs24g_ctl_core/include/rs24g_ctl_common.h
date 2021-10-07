/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_ctl_common.h
 */

#ifndef __RS24G_CTL_COMMON_H__
#define __RS24G_CTL_COMMON_H__

#include <stdint.h>

#include "rs24g_types.h"

#include "rs24g_ctl_distance.h"
#include "rs24g_ctl_fifo.h"

#define ARRAY_SIZE(a)							(sizeof(a) / sizeof((a)[0]))

#define RS_DISTANCE_INVALID						((int16_t) (0x7fff))
#define RS_PEAK_LEVEL_INVALID					((uint8_t) (0))


extern rs_ret_t get_devdata(rs_handle_t handle, uint32_t timeout, const uint16_t *reg_addr, rs_size_t reg_size, uint32_t *reg);
extern rs_ret_t get_devfifo(rs_handle_t handle, uint32_t timeout, rs_size_t fifo_size, uint8_t *fifo_data, struct rs_fifo_info *fifo_info);
extern rs_ret_t get_size_fifo(rs_handle_t handle, rs_size_t *size);
extern rs_ret_t get_distance_data(const uint32_t *regs, struct rs_distance_data *data, uint8_t peak_level_lower);

#endif	/* __RS24G_CTL_COMMON_H__ */
