/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_ctl_setup_range.c
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "rs24g_ctl_base.h"
#include "rs24g_ctl_motion.h"
#include "rs24g_ctl_distance.h"
#include "rs24g_ctl_fifo.h"
#include "rs24g_ctl_local.h"

#include "rs24g_ctl_setup_base.h"
#include "rs24g_ctl_setup_distance.h"
#include "rs24g_ctl_setup_local.h"
#include "rs24g_ctl_param_range.h"


static const int32_t choice_rx_gain[] = {40, 50, 60};
static const int32_t choice_chirp_time[] = {220, 1100, 4400};
static const int32_t choice_high_pass_filter[] = {
	RS_HPF_FIRST_ORDER,
	RS_HPF_SECOND_ORDER,
};
static const int32_t choice_digital_beam_shaper[] = {
	RS_DIGITAL_BEAM_SHAPER_WIDE,
	RS_DIGITAL_BEAM_SHAPER_NARROW,
};
static const int32_t choice_distance_step[] = {
	RS_DISTANCE_STEP_NORMAL,
	RS_DISTANCE_STEP_FINE,
	RS_DISTANCE_STEP_XFINE
};

#define ARRAY_SIZE(a)							(sizeof(a) / sizeof((a)[0]))


RS_IF rs_ret_t rs_param_get_range(rs_param_t param, rs_mode_t mode, struct rs_param_type_range *data)
{
	RET_CHECK(data != NULL, RS_EFAULT);

	data->min = 10;
	data->max = 55;

	switch(mode) {
	case RS_MODE_MOTION_DETECT:
		switch(param) {
		case RS_PARAM_INTERVAL:
			data->min = RS_INTERVAL_MIN;
			data->max = RS_INTERVAL_MAX;
			break;
		case RS_PARAM_ALPHA:
			data->min = RS_ALPHA_MIN;
			data->max = RS_ALPHA_MAX;
			break;
		case RS_PARAM_MOTION_THRESHOLD:
			data->min = RS_MOTION_THRESHOLD_MIN;
			data->max = RS_MOTION_THRESHOLD_MAX;
			break;
		case RS_PARAM_STABLE_COUNT:
			data->min = RS_STABLE_COUNT_MIN;
			data->max = RS_STABLE_COUNT_MAX;
			break;
		case RS_PARAM_LOWER_DISTANCE:
			data->min = RS_DISTANCE_MIN_MODE_MOTION;
			data->max = RS_DISTANCE_MAX_MODE_MOTION;
			break;
		case RS_PARAM_UPPER_DISTANCE:
			data->min = RS_DISTANCE_MIN_MODE_MOTION;
			data->max = RS_DISTANCE_MAX_MODE_MOTION;
			break;

		default:
			return RS_EINV;
		}
		break;

	case RS_MODE_DISTANCE_DETECT:
		switch(param) {
		case RS_PARAM_INTERVAL:
			data->min = RS_INTERVAL_MIN;
			data->max = RS_INTERVAL_MAX;
			break;
		case RS_PARAM_BETA:
			data->min = RS_BETA_MIN;
			data->max = RS_BETA_MAX;
			break;
		case RS_PARAM_LOWER_DISTANCE:
			data->min = RS_DISTANCE_MIN_MODE_DISTANCE;
			data->max = RS_DISTANCE_MAX_MODE_DISTANCE;
			break;
		case RS_PARAM_UPPER_DISTANCE:
			data->min = RS_DISTANCE_MIN_MODE_DISTANCE;
			data->max = RS_DISTANCE_MAX_MODE_DISTANCE;
			break;

		default:
			return RS_EINV;
		}
		break;

	case RS_MODE_FIFO:
		switch(param) {
		case RS_PARAM_INTERVAL:
			data->min = RS_INTERVAL_MIN;
			data->max = RS_INTERVAL_MAX;
			break;

		default:
			return RS_EINV;
		}
		break;

	default:
		return RS_ERANGE_MODE_TYPE;
	}

	return RS_OK;
}

RS_IF rs_ret_t rs_param_get_choice(rs_param_t param, rs_mode_t mode, struct rs_param_type_choice *data)
{
	RET_CHECK(data != NULL, RS_EFAULT);

	switch(mode) {
	case RS_MODE_MOTION_DETECT:
	case RS_MODE_DISTANCE_DETECT:
		break;

	default:
		return RS_ERANGE_MODE_TYPE;
	}

	switch(param) {
	case RS_PARAM_RX_GAIN:
		data->num = ARRAY_SIZE(choice_rx_gain);
		data->choice = choice_rx_gain;
		break;

	case RS_PARAM_CHIRP_TIME:
		data->num = ARRAY_SIZE(choice_chirp_time);
		data->choice = choice_chirp_time;
		break;

	case RS_PARAM_HIGH_PASS_FILTER:
		data->num = ARRAY_SIZE(choice_high_pass_filter);
		data->choice = choice_high_pass_filter;
		break;

	case RS_PARAM_DIGITAL_BEAM_SHAPER:
		data->num = ARRAY_SIZE(choice_digital_beam_shaper);
		data->choice = choice_digital_beam_shaper;
		break;
	case RS_PARAM_DISTANCE_STEP:
		data->num = ARRAY_SIZE(choice_distance_step);
		data->choice = choice_distance_step;

	default:
		return RS_EINV;
	}

	return RS_OK;
}

static rs_bool_t param_check_range_local(rs_param_t param, rs_mode_t mode, int32_t val)
{
	struct rs_param_type_range range;

	RET_CHECK(rs_param_get_range(param, mode, &range) == RS_OK, RS_FALSE);
	RET_CHECK(range.min <= val, RS_FALSE);
	RET_CHECK(range.max >= val, RS_FALSE);

	return RS_TRUE;
}

static rs_bool_t param_check_choice_local(rs_param_t param, rs_mode_t mode, int32_t val)
{
	struct rs_param_type_choice choice;

	RET_CHECK(rs_param_get_choice(param, mode, &choice) == RS_OK, RS_FALSE);

	{
		rs_size_t i;
		for(i=0; i<choice.num; i++) {
			if(choice.choice[i] == val) {
				return RS_TRUE;
			}
		}
	}

	return RS_FALSE;
}

RS_IF rs_ret_t rs_param_check(rs_param_t param, rs_mode_t mode, int32_t val)
{
	switch(param) {
	case RS_PARAM_INTERVAL:
		RET_CHECK(param_check_range_local(param, mode, val), RS_ERANGE_INTERVAL);
		break;
	case RS_PARAM_ALPHA:
		RET_CHECK(param_check_range_local(param, mode, val), RS_ERANGE_ALPHA);
		break;
	case RS_PARAM_MOTION_THRESHOLD:
		RET_CHECK(param_check_range_local(param, mode, val), RS_ERANGE_MOTION_THRESHOLD);
		break;
	case RS_PARAM_STABLE_COUNT:
		RET_CHECK(param_check_range_local(param, mode, val), RS_ERANGE_STARTUP_COUNT);
		break;
	case RS_PARAM_LOWER_DISTANCE:
		RET_CHECK(param_check_range_local(param, mode, val), RS_ERANGE_LOWER_DISTANCE);
		break;
	case RS_PARAM_UPPER_DISTANCE:
		RET_CHECK(param_check_range_local(param, mode, val), RS_ERANGE_UPPER_DISTANCE);
		break;
	case RS_PARAM_BETA:
		RET_CHECK(param_check_range_local(param, mode, val), RS_ERANGE_BETA);
		break;

	case RS_PARAM_RX_GAIN:
		RET_CHECK(param_check_choice_local(param, mode, val), RS_ERANGE_RX_GAIN);
		break;
	case RS_PARAM_CHIRP_TIME:
		RET_CHECK(param_check_choice_local(param, mode, val), RS_ERANGE_CHIRP_TIME);
		break;
	case RS_PARAM_HIGH_PASS_FILTER:
		RET_CHECK(param_check_choice_local(param, mode, val), RS_ERANGE_HPF);
		break;
	case RS_PARAM_DIGITAL_BEAM_SHAPER:
		RET_CHECK(param_check_choice_local(param, mode, val), RS_ERANGE_DIGITAL_BEAM_SHAPER);
		break;
	case RS_PARAM_DISTANCE_STEP:
		RET_CHECK(param_check_choice_local(param, mode, val), RS_ERANGE_DISTANCE_STEP);
		break;

	default:
		return RS_EINV;
	}

	return RS_OK;
}


/*
 * Check distance range with distance step for distance and 2d detection.
 */
RS_IF rs_ret_t rs_param_distance_range_check(rs_step_t dist_step, uint32_t upper, uint32_t lower)
{
	switch (dist_step){
	case RS_DISTANCE_STEP_NORMAL:
		RET_CHECK(upper <= RS_DISTANCE_MAX_NORMAL, RS_ERANGE_UPPER_DISTANCE);
		RET_CHECK(lower >= RS_DISTANCE_MIN_NORMAL, RS_ERANGE_LOWER_DISTANCE);
		break;
	case RS_DISTANCE_STEP_FINE:
		RET_CHECK(upper <= RS_DISTANCE_MAX_FINE, RS_ERANGE_UPPER_DISTANCE);
		RET_CHECK(lower >= RS_DISTANCE_MIN_FINE, RS_ERANGE_LOWER_DISTANCE);
		break;
	case RS_DISTANCE_STEP_XFINE:
		RET_CHECK(upper <= RS_DISTANCE_MAX_XFINE, RS_ERANGE_UPPER_DISTANCE);
		RET_CHECK(lower >= RS_DISTANCE_MIN_XFINE, RS_ERANGE_LOWER_DISTANCE);
		break;

	default:
		return RS_ERANGE_DISTANCE_STEP;
	}

	return RS_OK;
}
