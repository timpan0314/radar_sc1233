/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file setup_2d.c
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "defs/rs_dev_addr.h"
#include "rs24g_ctl_local.h"
#include "rs24g_ctl_setup_base.h"
#include "rs24g_ctl_param_range.h"
#include "rs24g_ctl_setup_local.h"
#include "rs24g_ctl_setup_2d.h"
#include "setup_common.h"
#include "setup_distance.h"
#include "setup_2d.h"
#include "setup_selection.h"

#include "rs24g_initial_dataset.h"
#include "setup/evk/2d/all.h"


rs_ret_t mode_2d_init(struct rs_2rx_param *lp)
{
	lp->rx_gain = RS_2D_RX_GAIN_INIT;
	lp->interval = RS_2D_INTERVAL_INIT;
	lp->beta = RS_2D_BETA_INIT;
	lp->lower_distance = RS_2D_LOWER_DISTANCE_INIT;
	lp->upper_distance = RS_2D_UPPER_DISTANCE_INIT;
	lp->chirp_time = RS_2D_CHIRP_TIME_INIT;
	lp->high_pass_filter = RS_2D_HPF_INIT;
	lp->distance_step = RS_2D_DISTANCE_STEP_INIT;

	return RS_OK;
}

/**
 * prepare parameters for angle detection
 * @callergraph
 */
rs_ret_t mode_2d_getcode(const struct rs_2rx_param *lp, rs_code_ref_t code)
{
	const struct initial_dataset *setup = NULL;

	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_INTERVAL, RS_MODE_DISTANCE_DETECT, lp->interval), RS_OK);
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_BETA, RS_MODE_DISTANCE_DETECT, lp->beta), RS_OK);
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_LOWER_DISTANCE, RS_MODE_DISTANCE_DETECT, lp->lower_distance), RS_OK);
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_UPPER_DISTANCE, RS_MODE_DISTANCE_DETECT, lp->upper_distance), RS_OK);
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_HIGH_PASS_FILTER, RS_MODE_DISTANCE_DETECT, lp->high_pass_filter), RS_OK);
	RET_CHECK(lp->lower_distance < lp->upper_distance, RS_ERANGE_LOWER_DISTANCE);
	RET_ORG(rs_ret_t, rs_param_distance_range_check(lp->distance_step, lp->upper_distance, lp->lower_distance), RS_OK);

	switch (lp->distance_step){
	case RS_DISTANCE_STEP_NORMAL:
		switch (lp->rx_gain) {
		case 40:
			switch (lp->chirp_time) {
#if RS_SETUP_MODE_2D_NORMAL_40DB_220US
			case 220:
				setup = &mode_2d_normal_40db_220us;
				break;
#endif
#if RS_SETUP_MODE_2D_NORMAL_40DB_1100US
			case 1100:
				setup = &mode_2d_normal_40db_1100us;
				break;
#endif
#if RS_SETUP_MODE_2D_NORMAL_40DB_4400US
			case 4400:
				setup = &mode_2d_normal_40db_4400us;
				break;
#endif
			default:
				return RS_ERANGE_CHIRP_TIME;
			}
			break;
		case 50:
			switch (lp->chirp_time) {
#if RS_SETUP_MODE_2D_NORMAL_50DB_220US
			case 220:
				setup = &mode_2d_normal_50db_220us;
				break;
#endif
#if RS_SETUP_MODE_2D_NORMAL_50DB_1100US
			case 1100:
				setup = &mode_2d_normal_50db_1100us;
				break;
#endif
#if RS_SETUP_MODE_2D_NORMAL_50DB_4400US
			case 4400:
				setup = &mode_2d_normal_50db_4400us;
				break;
#endif
			default:
				return RS_ERANGE_CHIRP_TIME;
			}
			break;
		case 60:
			switch (lp->chirp_time) {
#if RS_SETUP_MODE_2D_NORMAL_60DB_220US
			case 220:
				setup = &mode_2d_normal_60db_220us;
				break;
#endif
#if RS_SETUP_MODE_2D_NORMAL_60DB_1100US
			case 1100:
				setup = &mode_2d_normal_60db_1100us;
				break;
#endif
#if RS_SETUP_MODE_2D_NORMAL_60DB_4400US
			case 4400:
				setup = &mode_2d_normal_60db_4400us;
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
		switch (lp->rx_gain) {
		case 40:
			switch (lp->chirp_time) {
#if RS_SETUP_MODE_2D_FINE_40DB_220US
			case 220:
				setup = &mode_2d_fine_40db_220us;
				break;
#endif
#if RS_SETUP_MODE_2D_FINE_40DB_1100US
			case 1100:
				setup = &mode_2d_fine_40db_1100us;
				break;
#endif
#if RS_SETUP_MODE_2D_FINE_40DB_4400US
			case 4400:
				setup = &mode_2d_fine_40db_4400us;
				break;
#endif
			default:
				return RS_ERANGE_CHIRP_TIME;
			}
			break;
		case 50:
			switch (lp->chirp_time) {
#if RS_SETUP_MODE_2D_FINE_50DB_220US
			case 220:
				setup = &mode_2d_fine_50db_220us;
				break;
#endif
#if RS_SETUP_MODE_2D_FINE_50DB_1100US
			case 1100:
				setup = &mode_2d_fine_50db_1100us;
				break;
#endif
#if RS_SETUP_MODE_2D_FINE_50DB_4400US
			case 4400:
				setup = &mode_2d_fine_50db_4400us;
				break;
#endif
			default:
				return RS_ERANGE_CHIRP_TIME;
			}
			break;
		case 60:
			switch (lp->chirp_time) {
#if RS_SETUP_MODE_2D_FINE_60DB_220US
			case 220:
				setup = &mode_2d_fine_60db_220us;
				break;
#endif
#if RS_SETUP_MODE_2D_FINE_60DB_1100US
			case 1100:
				setup = &mode_2d_fine_60db_1100us;
				break;
#endif
#if RS_SETUP_MODE_2D_FINE_60DB_4400US
			case 4400:
				setup = &mode_2d_fine_60db_4400us;
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
#if RS_SETUP_MODE_2D_XFINE_40DB_220US
			case 220:
				setup = &mode_2d_xfine_40db_220us;
				break;
#endif
#if RS_SETUP_MODE_2D_XFINE_40DB_1100US
			case 1100:
				setup = &mode_2d_xfine_40db_1100us;
				break;
#endif
			default:
				return RS_ERANGE_CHIRP_TIME;
			}
			break;
		case 50:
			switch (lp->chirp_time) {
#if RS_SETUP_MODE_2D_XFINE_50DB_220US
			case 220:
				setup = &mode_2d_xfine_50db_220us;
				break;
#endif
#if RS_SETUP_MODE_2D_XFINE_50DB_1100US
			case 1100:
				setup = &mode_2d_xfine_50db_1100us;
				break;
#endif
			default:
				return RS_ERANGE_CHIRP_TIME;
			}
			break;
		case 60:
			switch (lp->chirp_time) {
#if RS_SETUP_MODE_2D_XFINE_60DB_220US
			case 220:
				setup = &mode_2d_xfine_60db_220us;
				break;
#endif
#if RS_SETUP_MODE_2D_XFINE_60DB_1100US
			case 1100:
				setup = &mode_2d_xfine_60db_1100us;
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
 * update parameters for angle detection
 * @callergraph
 */
rs_ret_t mode_2d_update_param(rs_handle_t handle, const struct rs_2rx_param *lp)
{
	RET_ORG(rs_ret_t, rs_setup_interval(handle, lp->interval), RS_OK);
	RET_ORG(rs_ret_t, rs_setup_beta(handle, lp->beta), RS_OK);
	RET_ORG(rs_ret_t, rs_setup_hpf(handle, lp->high_pass_filter), RS_OK);
	RET_ORG(rs_ret_t, rs_setup_range_peak(handle, lp->upper_distance, lp->lower_distance), RS_OK);

	return RS_OK;
}

/*
 * update parameters in ready for angle detection
 */
rs_ret_t mode_2d_update_param_in_ready(rs_handle_t handle, const struct rs_update_distance_param *lp)
{
	RET_ORG(rs_ret_t, distance_update_param_in_ready(handle, lp), RS_OK);

	return RS_OK;
}

/*
 * check update parameters in ready for angle detection
 */
rs_ret_t mode_2d_update_param_check(rs_handle_t handle, const struct rs_update_distance_param *lp)
{
	RET_ORG(rs_ret_t, distance_update_param_check(handle, lp), RS_OK);

	return RS_OK;
}
