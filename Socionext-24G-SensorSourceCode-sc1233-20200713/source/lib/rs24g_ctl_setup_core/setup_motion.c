/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file setup_motion.c
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "defs/rs_dev_addr.h"
#include "rs24g_ctl_setup_base.h"
#include "rs24g_ctl_param_range.h"
#include "rs24g_ctl_setup_local.h"
#include "rs24g_ctl_setup_motion.h"
#include "setup_common.h"
#include "setup_motion.h"


rs_ret_t motion_init(struct rs_motion_param *lp)
{
	lp->rx_gain = RS_MOTION_RX_GAIN_INIT;
	lp->interval = RS_MOTION_INTERVAL_INIT;
	lp->alpha = RS_MOTION_ALPHA_INIT;
	lp->motion_threshold = RS_MOTION_MOTION_THRESHOLD_INIT;
	lp->startup_count = RS_MOTION_STABLE_COUNT_INIT;
	lp->lower_distance = RS_MOTION_LOWER_DISTANCE_INIT;
	lp->upper_distance = RS_MOTION_UPPER_DISTANCE_INIT;
	lp->chirp_time = RS_MOTION_CHIRP_TIME_INIT;
	lp->digital_beam_shaper = RS_MOTION_DIGITAL_BEAM_SHAPER_INIT;
	lp->high_pass_filter = RS_MOTION_HPF_INIT;

	return RS_OK;
}

/**
 * Update smoothing factor register
 */
rs_ret_t rs_setup_alpha(rs_handle_t handle, uint8_t alpha)
{
	RET_ORG(rs_ret_t, read_modify_write_reg(handle, RS_CTL_REG_PRESDET_00, 0x00ff0000, ((uint32_t) alpha) << 16), RS_OK);

	return RS_OK;
}

/**
 * Update entry motion detection threshold register
 */
rs_ret_t rs_setup_motion_threshold(rs_handle_t handle, uint16_t motion_threshold)
{
	RET_ORG(rs_ret_t, read_modify_write_reg(handle, RS_CTL_REG_PRESDET_01, 0x000fff00, motion_threshold << 8), RS_OK);

	return RS_OK;
}

/**
 * Update start-up counter register
 */
rs_ret_t rs_setup_startup_count(rs_handle_t handle, uint8_t startup_count)
{
	RET_ORG(rs_ret_t, read_modify_write_reg(handle, RS_CTL_REG_PRESDET_01, 0x000000ff, startup_count << 0), RS_OK);

	return RS_OK;
}

/**
 * Update entry motion detection frequency index register
 */
rs_ret_t rs_setup_range_motion(rs_handle_t handle, uint32_t upper, uint32_t lower)
{
	RET_ORG(rs_ret_t, rs_setup_range(handle, RS_CTL_REG_PRESDET_03, upper, lower, 0), RS_OK);

	return RS_OK;
}

/*
 * update parameters in ready state for motion detection
 */
rs_ret_t motion_update_param_in_ready(rs_handle_t handle, const struct rs_update_motion_param *lp)
{
	RET_ORG(rs_ret_t, rs_setup_interval(handle, lp->interval), RS_OK);
	RET_ORG(rs_ret_t, rs_setup_alpha(handle, lp->alpha), RS_OK);
	RET_ORG(rs_ret_t, rs_setup_motion_threshold(handle, lp->motion_threshold), RS_OK);
	RET_ORG(rs_ret_t, rs_setup_startup_count(handle, lp->startup_count), RS_OK);
	RET_ORG(rs_ret_t, rs_setup_range_motion(handle, lp->upper_distance, lp->lower_distance), RS_OK);
	RET_ORG(rs_ret_t, rs_setup_hpf(handle, lp->high_pass_filter), RS_OK);

	return RS_OK;
}

/*
 * check update parameters in ready state for motion detection
 */
rs_ret_t motion_update_param_check(rs_handle_t handle, const struct rs_update_motion_param *lp)
{
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_INTERVAL, RS_MODE_MOTION_DETECT, lp->interval), RS_OK);
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_ALPHA, RS_MODE_MOTION_DETECT, lp->alpha), RS_OK);
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_MOTION_THRESHOLD, RS_MODE_MOTION_DETECT, lp->motion_threshold), RS_OK);
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_STABLE_COUNT, RS_MODE_MOTION_DETECT, lp->startup_count), RS_OK);
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_LOWER_DISTANCE, RS_MODE_MOTION_DETECT, lp->lower_distance), RS_OK);
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_UPPER_DISTANCE, RS_MODE_MOTION_DETECT, lp->upper_distance), RS_OK);
	RET_CHECK(lp->lower_distance < lp->upper_distance, RS_ERANGE_LOWER_DISTANCE);

	return RS_OK;
}
