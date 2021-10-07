/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * setup_distance.h
 */

#ifndef __SETUP_DISTANCE_H__
#define __SETUP_DISTANCE_H__

#include <stdint.h>

#include "rs24g_types.h"
#include "rs24g_ctl_setup_base.h"
#include "rs24g_ctl_setup_distance.h"

#include "setup_common.h"


#define RS_DISTANCE_RX_GAIN_INIT				(60)
#define RS_DISTANCE_INTERVAL_INIT				(25)
#define RS_DISTANCE_BETA_INIT					(205)
#define RS_DISTANCE_LOWER_DISTANCE_INIT			(10)
#define RS_DISTANCE_UPPER_DISTANCE_INIT			(1000)
#define RS_DISTANCE_CHIRP_TIME_INIT				(1100)
#define RS_DISTANCE_DIGITAL_BEAM_SHAPER_INIT	(RS_DIGITAL_BEAM_SHAPER_WIDE)
#define RS_DISTANCE_HPF_INIT					(RS_HPF_FIRST_ORDER)
#define RS_DISTANCE_DISTANCE_STEP_INIT			(RS_DISTANCE_STEP_NORMAL)


extern rs_ret_t distance_init(struct rs_distance_param *lp);

extern rs_ret_t rs_setup_range_peak(rs_handle_t handle, uint32_t upper, uint32_t lower);
extern rs_ret_t rs_setup_beta(rs_handle_t handle, uint8_t beta);

extern rs_ret_t distance_update_param_in_ready(rs_handle_t handle, const struct rs_update_distance_param *lp);
extern rs_ret_t distance_update_param_check(rs_handle_t handle, const struct rs_update_distance_param *lp);

#endif	/* __SETUP_DISTANCE_H__ */
