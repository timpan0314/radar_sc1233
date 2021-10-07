/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_ctl_motion.h
 */

#ifndef __RS24G_CTL_MOTION_H__
#define __RS24G_CTL_MOTION_H__

#include <stdint.h>

#include "rs24g_types.h"

#include "rs24g_ctl_base.h"

/*
 * Type Definition
 */
typedef int32_t									rs_smoothedope_t;

/*
 * Constant Definition
 */
#define RS_MODE_MOTION_DETECT					((rs_mode_t) (0))

#define RS_SMOOTHED_LEVEL_UPDATE				((rs_smoothedope_t) (0))
#define RS_SMOOTHED_LEVEL_HOLD					((rs_smoothedope_t) (1))

/*
 * Struct Definition
 */

/*
 * Function Definition
 */
RS_IF rs_ret_t rs_get_motion(rs_handle_t handle, rs_bool_t *motion);
RS_IF rs_ret_t rs_wait_motion_change(rs_handle_t handle, uint32_t timeout, rs_bool_t *motion);

RS_IF rs_ret_t rs_setup_smoothed_level(rs_handle_t handle, rs_smoothedope_t smoothed_level_ope, uint32_t fft_point, uint16_t addr, const uint32_t *val, rs_size_t num);
RS_IF rs_ret_t rs_read_smoothed_level(rs_handle_t handle, uint32_t fft_point, uint16_t addr, uint32_t *val, rs_size_t num);

#endif	/* __RS24G_CTL_MOTION_H__ */
