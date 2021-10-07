/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_2d_sc1233_range.c
 */

#include <stdint.h>

#include "rs24g_types.h"
#include "rs24g_2d_types.h"
#include "rs_macro.h"
#include "rs24g_ctl_sc1233.h"

#include "rs24g_2d_sc1233.h"
#include "rs24g_2d_sc1233_local.h"
#include "param_2d.h"
#include "common_2d.h"


static const int32_t choice_mode[] = {
	RS_2D_MODE_RAW_ANGLE,
	RS_2D_MODE_MOVING_AVERAGE,
};
static const int32_t choice_sort_order[] = {
	RS_2D_SORT_ORDER_PEAK_LEVEL,
	RS_2D_SORT_ORDER_DISTANCE,
};
static const int32_t choice_rx_gain[] = {40, 50, 60};
static const int32_t choice_chirp_time[] = {220, 1100, 4400};
static const int32_t choice_high_pass_filter[] = {
	RS_HPF_FIRST_ORDER,
	RS_HPF_SECOND_ORDER,
};
static const int32_t choice_stationary[] = {
	RS_2D_STATIONARY_OFF,
	RS_2D_STATIONARY_ON,
};


RS_IF rs_ret_t rs_2d_param_get_range(rs_2d_param_t param, struct rs_2d_param_type_range *data)
{
	RET_CHECK(data != NULL, RS_EFAULT);

	data->min = 0;
	data->max = 0;

	switch(param) {
	case RS_2D_PARAM_NTAP_AVE_DISTANCE:
		data->min = RS_2D_NTAP_AVE_DISTANCE_MIN;
		data->max = RS_2D_NTAP_AVE_DISTANCE_MAX;
		break;
	case RS_2D_PARAM_NTAP_AVE_ANGLE:
		data->min = RS_2D_NTAP_AVE_ANGLE_MIN;
		data->max = RS_2D_NTAP_AVE_ANGLE_MAX;
		break;
	case RS_2D_PARAM_NTAP_AVE_PEAK_LEVEL:
		data->min = RS_2D_NTAP_AVE_PEAK_LEVEL_MIN;
		data->max = RS_2D_NTAP_AVE_PEAK_LEVEL_MAX;
		break;
	case RS_2D_PARAM_LOWER_ANGLE:
		data->min = RS_2D_LOWER_ANGLE_MIN;
		data->max = RS_2D_LOWER_ANGLE_MAX;
		break;
	case RS_2D_PARAM_UPPER_ANGLE:
		data->min = RS_2D_UPPER_ANGLE_MIN;
		data->max = RS_2D_UPPER_ANGLE_MAX;
		break;
	case RS_2D_PARAM_ANGLE_THRESHOLD:
		data->min = RS_2D_ANGLE_THRESHOLD_MIN;
		data->max = RS_2D_ANGLE_THRESHOLD_MAX;
		break;
	case RS_2D_PARAM_LOCAL_INTERVAL:
		data->min = RS_2D_LOCAL_INTERVAL_MIN;
		data->max = RS_2D_LOCAL_INTERVAL_MAX;
		break;

	case RS_2D_PARAM_MODE:
	case RS_2D_PARAM_SORT_ORDER:
	case RS_2D_PARAM_LOCAL_RX_GAIN:
	case RS_2D_PARAM_LOCAL_CHIRP_TIME:
	case RS_2D_PARAM_LOCAL_HPF:
	default:
		return RS_EINV;
	}

	return RS_OK;
}

RS_IF rs_ret_t rs_2d_param_get_choice(rs_2d_param_t param, struct rs_2d_param_type_choice *data)
{
	RET_CHECK(data != NULL, RS_EFAULT);

	switch(param) {
	case RS_2D_PARAM_MODE:
		data->num = RS_2D_ARRAY_SIZE(choice_mode);
		data->choice = choice_mode;
		break;
	case RS_2D_PARAM_SORT_ORDER:
		data->num = RS_2D_ARRAY_SIZE(choice_sort_order);
		data->choice = choice_sort_order;
		break;
	case RS_2D_PARAM_LOCAL_RX_GAIN:
		data->num = RS_2D_ARRAY_SIZE(choice_rx_gain);
		data->choice = choice_rx_gain;
		break;
	case RS_2D_PARAM_LOCAL_CHIRP_TIME:
		data->num = RS_2D_ARRAY_SIZE(choice_chirp_time);
		data->choice = choice_chirp_time;
		break;
	case RS_2D_PARAM_LOCAL_HPF:
		data->num = RS_2D_ARRAY_SIZE(choice_high_pass_filter);
		data->choice = choice_high_pass_filter;
		break;
	case RS_2D_PARAM_STATIONARY:
		data->num = RS_2D_ARRAY_SIZE(choice_stationary);
		data->choice = choice_stationary;
		break;

	case RS_2D_PARAM_NTAP_AVE_DISTANCE:
	case RS_2D_PARAM_NTAP_AVE_ANGLE:
	case RS_2D_PARAM_NTAP_AVE_PEAK_LEVEL:
	case RS_2D_PARAM_LOWER_ANGLE:
	case RS_2D_PARAM_UPPER_ANGLE:
	case RS_2D_PARAM_ANGLE_THRESHOLD:
	case RS_2D_PARAM_LOCAL_INTERVAL:
	default:
		return RS_EINV;
	}

	return RS_OK;
}

static rs_bool_t param_check_range_local(rs_2d_param_t param, int32_t val)
{
	struct rs_2d_param_type_range range;

	RET_CHECK(rs_2d_param_get_range(param, &range) == RS_OK, RS_FALSE);
//	rs24g2d("val:%d (%d ~ %d)", val, range.min, range.max);

	RET_CHECK(range.min <= val, RS_FALSE);
	RET_CHECK(range.max >= val, RS_FALSE);

	return RS_TRUE;
}

static rs_bool_t param_check_choice_local(rs_2d_param_t param, int32_t val)
{
	struct rs_2d_param_type_choice choice;

	RET_CHECK(rs_2d_param_get_choice(param, &choice) == RS_OK, RS_FALSE);

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

RS_IF rs_ret_t rs_2d_param_check(rs_2d_param_t param, int32_t val)
{
	switch(param) {
	case RS_2D_PARAM_NTAP_AVE_DISTANCE:
		RET_CHECK(param_check_range_local(param, val), RS_2D_ERANGE_NTAP_AVE_DISTANCE);
		break;
	case RS_2D_PARAM_NTAP_AVE_ANGLE:
		RET_CHECK(param_check_range_local(param, val), RS_2D_ERANGE_NTAP_AVE_ANGLE);
		break;
	case RS_2D_PARAM_NTAP_AVE_PEAK_LEVEL:
		RET_CHECK(param_check_range_local(param, val), RS_2D_ERANGE_NTAP_AVE_PEAK_LEVEL);
		break;
	case RS_2D_PARAM_LOWER_ANGLE:
		RET_CHECK(param_check_range_local(param, val), RS_2D_ERANGE_LOWER_ANGLE);
		break;
	case RS_2D_PARAM_UPPER_ANGLE:
		RET_CHECK(param_check_range_local(param, val), RS_2D_ERANGE_UPPER_ANGLE);
		break;
	case RS_2D_PARAM_ANGLE_THRESHOLD:
		RET_CHECK(param_check_range_local(param, val), RS_2D_ERANGE_ANGLE_THRESHOLD);
		break;
	case RS_2D_PARAM_LOCAL_INTERVAL:
		RET_CHECK(param_check_range_local(param, val), RS_ERANGE_INTERVAL);
		break;

	case RS_2D_PARAM_MODE:
		RET_CHECK(param_check_choice_local(param, val), RS_2D_ERANGE_MODE);
		break;
	case RS_2D_PARAM_SORT_ORDER:
		RET_CHECK(param_check_choice_local(param, val), RS_2D_ERANGE_SORT_ORDER);
		break;
	case RS_2D_PARAM_LOCAL_RX_GAIN:
		RET_CHECK(param_check_choice_local(param, val), RS_ERANGE_RX_GAIN);
		break;
	case RS_2D_PARAM_LOCAL_CHIRP_TIME:
		RET_CHECK(param_check_choice_local(param, val), RS_ERANGE_CHIRP_TIME);
		break;
	case RS_2D_PARAM_LOCAL_HPF:
		RET_CHECK(param_check_choice_local(param, val), RS_ERANGE_HPF);
		break;
	case RS_2D_PARAM_STATIONARY:
		RET_CHECK(param_check_choice_local(param, val), RS_2D_ERANGE_STATIONARY);
		break;

	default:
		return RS_EINV;
	}

	return RS_OK;
}
