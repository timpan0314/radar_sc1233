/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file setup_motion_wide.c
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
#include "setup_selection.h"

#include "rs24g_initial_dataset.h"
#include "setup/evk/motion_wide/all.h"


/**
 * prepare parameters for motion detection
 * @callergraph
 */
rs_ret_t motion_wide_getcode(const struct rs_motion_param *lp, rs_code_ref_t code)
{
	const struct initial_dataset *setup = NULL;

	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_INTERVAL, RS_MODE_MOTION_DETECT, lp->interval), RS_OK);
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_ALPHA, RS_MODE_MOTION_DETECT, lp->alpha), RS_OK);
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_MOTION_THRESHOLD, RS_MODE_MOTION_DETECT, lp->motion_threshold), RS_OK);
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_STABLE_COUNT, RS_MODE_MOTION_DETECT, lp->startup_count), RS_OK);
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_LOWER_DISTANCE, RS_MODE_MOTION_DETECT, lp->lower_distance), RS_OK);
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_UPPER_DISTANCE, RS_MODE_MOTION_DETECT, lp->upper_distance), RS_OK);
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_HIGH_PASS_FILTER, RS_MODE_MOTION_DETECT, lp->high_pass_filter), RS_OK);
	RET_CHECK(lp->digital_beam_shaper == RS_DIGITAL_BEAM_SHAPER_WIDE, RS_ERANGE_DIGITAL_BEAM_SHAPER);
	RET_CHECK(lp->lower_distance < lp->upper_distance, RS_ERANGE_LOWER_DISTANCE);

	switch(lp->rx_gain) {
	case 40:
		switch(lp->chirp_time) {
#if RS_SETUP_MODE_MOTION_WIDE_40DB_220US
		case 220:		setup = &mode_motion_wide_40db_220us;		break;
#endif
#if RS_SETUP_MODE_MOTION_WIDE_40DB_1100US
		case 1100:		setup = &mode_motion_wide_40db_1100us;		break;
#endif
#if RS_SETUP_MODE_MOTION_WIDE_40DB_4400US
		case 4400:		setup = &mode_motion_wide_40db_4400us;		break;
#endif
		default:		return RS_ERANGE_CHIRP_TIME;
		}
		break;
	case 50:
		switch(lp->chirp_time) {
#if RS_SETUP_MODE_MOTION_WIDE_50DB_220US
		case 220:		setup = &mode_motion_wide_50db_220us;		break;
#endif
#if RS_SETUP_MODE_MOTION_WIDE_50DB_1100US
		case 1100:		setup = &mode_motion_wide_50db_1100us;		break;
#endif
#if RS_SETUP_MODE_MOTION_WIDE_50DB_4400US
		case 4400:		setup = &mode_motion_wide_50db_4400us;		break;
#endif
		default:		return RS_ERANGE_CHIRP_TIME;
		}
		break;
	case 60:
		switch(lp->chirp_time) {
#if RS_SETUP_MODE_MOTION_WIDE_60DB_220US
		case 220:		setup = &mode_motion_wide_60db_220us;		break;
#endif
#if RS_SETUP_MODE_MOTION_WIDE_60DB_1100US
		case 1100:		setup = &mode_motion_wide_60db_1100us;		break;
#endif
#if RS_SETUP_MODE_MOTION_WIDE_60DB_4400US
		case 4400:		setup = &mode_motion_wide_60db_4400us;		break;
#endif
		default:		return RS_ERANGE_CHIRP_TIME;
		}
		break;
	default:
		return RS_ERANGE_RX_GAIN;
	}

	RET_ORG(rs_ret_t, rs_setup_getcode(code, setup), RS_OK);

	return RS_OK;
}

/**
 * update parameters for motion detection
 * @callergraph
 */
rs_ret_t motion_wide_update_param(rs_handle_t handle, const struct rs_motion_param *lp) {
	RET_ORG(rs_ret_t, rs_setup_interval(handle, lp->interval), RS_OK);
	RET_ORG(rs_ret_t, rs_setup_alpha(handle, lp->alpha), RS_OK);
	RET_ORG(rs_ret_t, rs_setup_motion_threshold(handle, lp->motion_threshold), RS_OK);
	RET_ORG(rs_ret_t, rs_setup_startup_count(handle, lp->startup_count), RS_OK);
	RET_ORG(rs_ret_t, rs_setup_range_motion(handle, lp->upper_distance, lp->lower_distance), RS_OK);
	RET_ORG(rs_ret_t, rs_setup_hpf(handle, lp->high_pass_filter), RS_OK);

	return RS_OK;
}
