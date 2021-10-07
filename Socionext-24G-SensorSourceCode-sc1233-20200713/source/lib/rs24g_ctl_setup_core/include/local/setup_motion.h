/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * setup_motion.h
 */

#ifndef __SETUP_MOTION_H__
#define __SETUP_MOTION_H__

#include <stdint.h>

#include "rs24g_types.h"
#include "rs24g_ctl_setup_base.h"
#include "rs24g_ctl_setup_motion.h"

#include "setup_common.h"


#define RS_MOTION_RX_GAIN_INIT					(60)
#define RS_MOTION_INTERVAL_INIT					(250)
#define RS_MOTION_ALPHA_INIT					(230)
#define RS_MOTION_MOTION_THRESHOLD_INIT			(200)
#define RS_MOTION_STABLE_COUNT_INIT				(20)
#define RS_MOTION_LOWER_DISTANCE_INIT			(10)
#define RS_MOTION_UPPER_DISTANCE_INIT			(1000)
#define RS_MOTION_CHIRP_TIME_INIT				(1100)
#define RS_MOTION_DIGITAL_BEAM_SHAPER_INIT		(RS_DIGITAL_BEAM_SHAPER_WIDE)
#define RS_MOTION_HPF_INIT						(RS_HPF_FIRST_ORDER)


extern rs_ret_t motion_init(struct rs_motion_param *lp);

extern rs_ret_t rs_setup_alpha(rs_handle_t handle, uint8_t alpha);
extern rs_ret_t rs_setup_motion_threshold(rs_handle_t handle, uint16_t motion_threshold);
extern rs_ret_t rs_setup_startup_count(rs_handle_t handle, uint8_t startup_count);
extern rs_ret_t rs_setup_range_motion(rs_handle_t handle, uint32_t upper, uint32_t lower);

extern rs_ret_t motion_update_param_in_ready(rs_handle_t handle, const struct rs_update_motion_param *lp);
extern rs_ret_t motion_update_param_check(rs_handle_t handle, const struct rs_update_motion_param *lp);

#endif	/* __SETUP_MOTION_H__ */
