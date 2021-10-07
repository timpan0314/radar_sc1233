/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file setup_distance_wide.c
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "rs24g_ctl_setup_base.h"
#include "rs24g_ctl_param_range.h"
#include "rs24g_ctl_setup_local.h"
#include "rs24g_ctl_setup_distance.h"
#include "setup_common.h"
#include "setup_distance_wide.h"
#include "setup_selection.h"

#include "rs24g_initial_dataset.h"
#include "setup/evk/distance_wide/all.h"


/**
 * prepare parameters for distance detection
 * @callergraph
 */
rs_ret_t distance_wide_getcode(const struct rs_distance_param *lp, rs_code_ref_t code)
{
	const struct initial_dataset *setup = NULL;

	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_INTERVAL, RS_MODE_DISTANCE_DETECT, lp->interval), RS_OK);
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_BETA, RS_MODE_DISTANCE_DETECT, lp->beta), RS_OK);
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_LOWER_DISTANCE, RS_MODE_DISTANCE_DETECT, lp->lower_distance), RS_OK);
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_UPPER_DISTANCE, RS_MODE_DISTANCE_DETECT, lp->upper_distance), RS_OK);
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_HIGH_PASS_FILTER, RS_MODE_DISTANCE_DETECT, lp->high_pass_filter), RS_OK);
	RET_CHECK(lp->digital_beam_shaper == RS_DIGITAL_BEAM_SHAPER_WIDE, RS_ERANGE_DIGITAL_BEAM_SHAPER);
	RET_CHECK(lp->lower_distance < lp->upper_distance, RS_ERANGE_LOWER_DISTANCE);
	RET_ORG(rs_ret_t, rs_param_distance_range_check(lp->distance_step, lp->upper_distance, lp->lower_distance), RS_OK);

	switch (lp->distance_step){
	case RS_DISTANCE_STEP_NORMAL:
		switch (lp->rx_gain) {
		case 40:
			switch (lp->chirp_time) {
#if RS_SETUP_MODE_DISTANCE_WIDE_NORMAL_40DB_220US
			case 220:
				setup = &mode_distance_wide_normal_40db_220us;
				break;
#endif
#if RS_SETUP_MODE_DISTANCE_WIDE_NORMAL_40DB_1100US
			case 1100:
				setup = &mode_distance_wide_normal_40db_1100us;
				break;
#endif
#if RS_SETUP_MODE_DISTANCE_WIDE_NORMAL_40DB_4400US
			case 4400:
				setup = &mode_distance_wide_normal_40db_4400us;
				break;
#endif
			default:
				return RS_ERANGE_CHIRP_TIME;
			}
			break;
		case 50:
			switch (lp->chirp_time) {
#if RS_SETUP_MODE_DISTANCE_WIDE_NORMAL_50DB_220US
			case 220:
				setup = &mode_distance_wide_normal_50db_220us;
				break;
#endif
#if RS_SETUP_MODE_DISTANCE_WIDE_NORMAL_50DB_1100US
			case 1100:
				setup = &mode_distance_wide_normal_50db_1100us;
				break;
#endif
#if RS_SETUP_MODE_DISTANCE_WIDE_NORMAL_50DB_4400US
			case 4400:
				setup = &mode_distance_wide_normal_50db_4400us;
				break;
#endif
			default:
				return RS_ERANGE_CHIRP_TIME;
			}
			break;
		case 60:
			switch (lp->chirp_time) {
#if RS_SETUP_MODE_DISTANCE_WIDE_NORMAL_60DB_220US
			case 220:
				setup = &mode_distance_wide_normal_60db_220us;
				break;
#endif
#if RS_SETUP_MODE_DISTANCE_WIDE_NORMAL_60DB_1100US
			case 1100:
				setup = &mode_distance_wide_normal_60db_1100us;
				break;
#endif
#if RS_SETUP_MODE_DISTANCE_WIDE_NORMAL_60DB_4400US
			case 4400:
				setup = &mode_distance_wide_normal_60db_4400us;
				break;
#endif
			default:
				return RS_ERANGE_CHIRP_TIME;
			}
			break;
		default: 
			return RS_ERANGE_RX_GAIN;
		}
		break;
	case RS_DISTANCE_STEP_FINE:
		switch(lp->rx_gain) {
		case 40:
			switch (lp->chirp_time) {
#if RS_SETUP_MODE_DISTANCE_WIDE_FINE_40DB_220US
			case 220:
				setup = &mode_distance_wide_fine_40db_220us;
				break;
#endif
#if RS_SETUP_MODE_DISTANCE_WIDE_FINE_40DB_1100US
			case 1100:
				setup = &mode_distance_wide_fine_40db_1100us;
				break;
#endif
#if RS_SETUP_MODE_DISTANCE_WIDE_FINE_40DB_4400US
			case 4400:
				setup = &mode_distance_wide_fine_40db_4400us;
				break;
#endif
			default:
				return RS_ERANGE_CHIRP_TIME;
			}
			break;
		case 50:
			switch (lp->chirp_time) {
#if RS_SETUP_MODE_DISTANCE_WIDE_FINE_50DB_220US
			case 220:
				setup = &mode_distance_wide_fine_50db_220us;
				break;
#endif
#if RS_SETUP_MODE_DISTANCE_WIDE_FINE_50DB_1100US
			case 1100:
				setup = &mode_distance_wide_fine_50db_1100us;
				break;
#endif
#if RS_SETUP_MODE_DISTANCE_WIDE_FINE_50DB_4400US
			case 4400:
				setup = &mode_distance_wide_fine_50db_4400us;
				break;
#endif
			default:
				return RS_ERANGE_CHIRP_TIME;
			}
			break;
		case 60:
			switch (lp->chirp_time) {
#if RS_SETUP_MODE_DISTANCE_WIDE_FINE_60DB_220US
			case 220:
				setup = &mode_distance_wide_fine_60db_220us;
				break;
#endif
#if RS_SETUP_MODE_DISTANCE_WIDE_FINE_60DB_1100US
			case 1100:
				setup = &mode_distance_wide_fine_60db_1100us;
				break;
#endif
#if RS_SETUP_MODE_DISTANCE_WIDE_FINE_60DB_4400US
			case 4400:
				setup = &mode_distance_wide_fine_60db_4400us;
				break;
#endif
			default:
				return RS_ERANGE_CHIRP_TIME;
			}
			break;
		default:
			return RS_ERANGE_RX_GAIN;
		}
		break;
	case RS_DISTANCE_STEP_XFINE:
		switch (lp->rx_gain) {
		case 40:
			switch (lp->chirp_time) {
#if RS_SETUP_MODE_DISTANCE_WIDE_XFINE_40DB_220US
			case 220:
				setup = &mode_distance_wide_xfine_40db_220us;
				break;
#endif
#if RS_SETUP_MODE_DISTANCE_WIDE_XFINE_40DB_1100US
			case 1100:
				setup = &mode_distance_wide_xfine_40db_1100us;
				break;
#endif
			default:
				return RS_ERANGE_CHIRP_TIME;
			}
			break;
		case 50:
			switch (lp->chirp_time) {
#if RS_SETUP_MODE_DISTANCE_WIDE_XFINE_50DB_220US
			case 220:
				setup = &mode_distance_wide_xfine_50db_220us;
				break;
#endif
#if RS_SETUP_MODE_DISTANCE_WIDE_XFINE_50DB_1100US
			case 1100:
				setup = &mode_distance_wide_xfine_50db_1100us;
				break;
#endif
			default:
				return RS_ERANGE_CHIRP_TIME;
			}
			break;
		case 60:
			switch (lp->chirp_time) {
#if RS_SETUP_MODE_DISTANCE_WIDE_XFINE_60DB_220US
			case 220:
				setup = &mode_distance_wide_xfine_60db_220us;
				break;
#endif
#if RS_SETUP_MODE_DISTANCE_WIDE_XFINE_60DB_1100US
			case 1100:
				setup = &mode_distance_wide_xfine_60db_1100us;
				break;
#endif
			default:
				return RS_ERANGE_CHIRP_TIME;
			}
			break;
		default:
			return RS_ERANGE_RX_GAIN;
		}
		break;
	default:
		return RS_ERANGE_DISTANCE_STEP;
	}

	RET_ORG(rs_ret_t, rs_setup_getcode(code, setup), RS_OK);

	return RS_OK;
}

/**
 * update parameters for distance detection
 * @callergraph
 */
rs_ret_t distance_wide_update_param(rs_handle_t handle, const struct rs_distance_param *lp)
{
	RET_ORG(rs_ret_t, rs_setup_interval(handle, lp->interval), RS_OK);
	RET_ORG(rs_ret_t, rs_setup_beta(handle, lp->beta), RS_OK);
	RET_ORG(rs_ret_t, rs_setup_hpf(handle, lp->high_pass_filter), RS_OK);
	RET_ORG(rs_ret_t, rs_setup_range_peak(handle, lp->upper_distance, lp->lower_distance), RS_OK);

	return RS_OK;
}
