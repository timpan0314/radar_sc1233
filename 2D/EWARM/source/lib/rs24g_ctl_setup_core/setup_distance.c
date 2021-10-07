/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file setup_distance.c
 */

#include <stdint.h>
#include <math.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "defs/rs_dev_addr.h"
#include "rs24g_ctl_setup_base.h"
#include "rs24g_ctl_setup_local.h"
#include "rs24g_ctl_setup_distance.h"
#include "setup_common.h"
#include "setup_distance.h"
#include "rs24g_ctl_local.h"


rs_ret_t distance_init(struct rs_distance_param *lp)
{
	lp->rx_gain = RS_DISTANCE_RX_GAIN_INIT;
	lp->interval = RS_DISTANCE_INTERVAL_INIT;
	lp->beta = RS_DISTANCE_BETA_INIT;
	lp->lower_distance = RS_DISTANCE_LOWER_DISTANCE_INIT;
	lp->upper_distance = RS_DISTANCE_UPPER_DISTANCE_INIT;
	lp->chirp_time = RS_DISTANCE_CHIRP_TIME_INIT;
	lp->digital_beam_shaper = RS_DISTANCE_DIGITAL_BEAM_SHAPER_INIT;
	lp->high_pass_filter = RS_DISTANCE_HPF_INIT;
	lp->distance_step = RS_DISTANCE_DISTANCE_STEP_INIT;

	return RS_OK;
}

/**
 * Update smoothing factor register
 */
rs_ret_t rs_setup_beta(rs_handle_t handle, uint8_t beta)
{
	RET_ORG(rs_ret_t, read_modify_write_reg(handle, RS_CTL_REG_PRESDET_00, 0x0000ff00, ((uint32_t) beta) << 8), RS_OK);

	return RS_OK;
}

/**
 * Update distance measurement frequency index register
 */
rs_ret_t rs_setup_range_peak(rs_handle_t handle, uint32_t upper, uint32_t lower)
{
	int32_t conv_offset = 0;
	uint32_t val = 0;
	RET_ORG(rs_ret_t, rs_read_reg(handle, RS_CTL_REG_PRESDET_60, &val, 1), RS_OK);
	conv_offset = (((int32_t) (val)) << 12) >> 20;

	RET_ORG(rs_ret_t, rs_setup_range(handle, RS_CTL_REG_PRESDET_19, upper, lower, conv_offset), RS_OK);

	return RS_OK;
}

/*
 * update parameters in ready state for distance detection
 */
rs_ret_t distance_update_param_in_ready(rs_handle_t handle, const struct rs_update_distance_param *lp)
{
	RET_ORG(rs_ret_t, rs_setup_interval(handle, lp->interval), RS_OK);
	RET_ORG(rs_ret_t, rs_setup_beta(handle, lp->beta), RS_OK);
	RET_ORG(rs_ret_t, rs_setup_range_peak(handle, lp->upper_distance, lp->lower_distance), RS_OK);
	RET_ORG(rs_ret_t, rs_setup_hpf(handle, lp->high_pass_filter), RS_OK);

	return RS_OK;
}

/*
 * check update parameters in ready state for distance detection
 */
rs_ret_t distance_update_param_check(rs_handle_t handle, const struct rs_update_distance_param *lp)
{
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_INTERVAL, RS_MODE_DISTANCE_DETECT, lp->interval), RS_OK);
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_BETA, RS_MODE_DISTANCE_DETECT, lp->beta), RS_OK);
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_LOWER_DISTANCE, RS_MODE_DISTANCE_DETECT, lp->lower_distance), RS_OK);
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_UPPER_DISTANCE, RS_MODE_DISTANCE_DETECT, lp->upper_distance), RS_OK);
	RET_CHECK(lp->lower_distance < lp->upper_distance, RS_ERANGE_LOWER_DISTANCE);
	
	const uint32_t fft_point = 512;

	switch ((int32_t)point_per_chirp(handle, fft_point)){
	case 128:
		RET_ORG(rs_ret_t, rs_param_distance_range_check(RS_DISTANCE_STEP_NORMAL, lp->upper_distance, lp->lower_distance), RS_OK);
		break;
	case 64:
		RET_ORG(rs_ret_t, rs_param_distance_range_check(RS_DISTANCE_STEP_FINE, lp->upper_distance, lp->lower_distance), RS_OK);
		break;
	case 32:
		RET_ORG(rs_ret_t, rs_param_distance_range_check(RS_DISTANCE_STEP_XFINE, lp->upper_distance, lp->lower_distance), RS_OK);
		break;
	default:
		return RS_ERANGE_DISTANCE_STEP;
	}
	
	return RS_OK;
}
