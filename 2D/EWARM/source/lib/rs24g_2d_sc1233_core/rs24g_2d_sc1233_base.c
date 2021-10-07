/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_2d_sc1233_base.c
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "rs24g_ctl_sc1233.h"

#include "rs24g_2d_sc1233.h"
#include "rs24g_2d_sc1233_local.h"
#include "common_2d.h"
#include "location.h"
#include "param_2d.h"
#include "rs_2d_handle_t.h"


RS_IF rs_ret_t rs_2d_init(rs_2d_handle_t *handle, const struct rs_2rx_param *param)
{
	rs_ret_t ret = RS_OK;
	const struct noise_floor_table *nf = NULL;

	RET_CHECK(handle != NULL, RS_EFAULT);
	RET_CHECK(param != NULL, RS_EFAULT);

	RET_ORG(rs_ret_t, rs_2d_param_check(RS_2D_PARAM_LOCAL_RX_GAIN, param->rx_gain), RS_OK);
	RET_ORG(rs_ret_t, rs_2d_param_check(RS_2D_PARAM_LOCAL_INTERVAL, param->interval), RS_OK);
	RET_ORG(rs_ret_t, rs_2d_param_check(RS_2D_PARAM_LOCAL_CHIRP_TIME, param->chirp_time), RS_OK);
	RET_ORG(rs_ret_t, rs_2d_param_check(RS_2D_PARAM_LOCAL_HPF, param->high_pass_filter), RS_OK);

	switch(param->chirp_time) {
	case 220:
		switch(param->high_pass_filter) {
		case RS_HPF_FIRST_ORDER:
			nf = &nf_220us_hpf1;
			break;
		case RS_HPF_SECOND_ORDER:
			nf = &nf_220us_hpf2;
			break;
		default:
			break;
		}
		break;
	case 1100:
		switch (param->high_pass_filter) {
		case RS_HPF_FIRST_ORDER:
			nf = &nf_1100us_hpf1;
			break;
		case RS_HPF_SECOND_ORDER:
			nf = &nf_1100us_hpf2;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	*handle = (rs_2d_handle_t *) calloc(1, sizeof(struct rs_2d_handle));
	if(*handle == NULL) {
		ret = RS_ENOMEM;
	}

	if(ret == RS_OK) {
		struct rs_2d_handle *lh = (struct rs_2d_handle *) *handle;
		int i;

		ret = rs_list_init(&(lh->data.loc_est.list), MAX_LOC_EST, lh->data.loc_est.mem, sizeof(*(lh->data.loc_est.mem)), lh->data.loc_est.list_mem, sizeof(lh->data.loc_est.list_mem));

		for(i=0; i<MAX_LOC_EST; i++) {
			if(ret == RS_OK) {
				struct loc_est *obj = &(lh->data.loc_est.mem[i]);
				ret = rs_list_init(&(obj->loc_dev.list), MAX_LOC_DEV, obj->loc_dev.mem, sizeof(*(obj->loc_dev.mem)), obj->loc_dev.list_mem, sizeof(obj->loc_dev.list_mem));
			}

			//rs24g2d("obj[%2d]: %08llx", i, (long long unsigned int) &(lh->data.loc_est.mem[i]));
		}

		if(ret == RS_OK) {
			memcpy(&(lh->init_param), param, sizeof(lh->init_param));

			lh->state = RS_2D_STATE_INIT;

			lh->noise_floor = nf;
			lh->nc = RS_MAX(1, (rs_size_t)(125 / lh->init_param.interval));
			lh->dmax = RS_MAX(5, (rs_size_t)(500 / lh->init_param.interval));
			lh->dref = RS_2D_DREF_INIT;
			lh->la = RS_2D_LA_INIT;
			lh->target_threshold_prob = (rs_2d_fp_t) (RS_2D_TARGET_THRESHOLD_PROB);
			lh->noise_threshold_prob = (rs_2d_fp_t) (RS_2D_NOISE_THRESHOLD_PROB);
			//rs24g2d("lower_distance: %d", lh->init_param.lower_distance);

			return ret;
		}

		free(*handle);
	}

	return ret;
}

RS_IF rs_ret_t rs_2d_deinit(rs_2d_handle_t handle)
{
	RET_CHECK(handle != NULL, RS_EINV);

	free(handle);

	return RS_OK;
}

RS_IF rs_ret_t rs_2d_get_init_algo_param(struct rs_2d_algo_param *param)
{
	RET_CHECK(param != NULL, RS_EFAULT);

	param->mode = RS_2D_MODE_INIT;
	param->ntap_ave_distance = RS_2D_NTAP_AVE_DISTANCE_INIT;
	param->ntap_ave_angle = RS_2D_NTAP_AVE_ANGLE_INIT;
	param->ntap_ave_peak_level = RS_2D_NTAP_AVE_PEAK_LEVEL_INIT;
	param->sort_order = RS_2D_SORT_ORDER_INIT;
	param->lower_angle = RS_2D_LOWER_ANGLE_INIT;
	param->upper_angle = RS_2D_UPPER_ANGLE_INIT;
	param->angle_threshold = RS_2D_ANGLE_THRESHOLD_INIT;
	param->stationary = RS_2D_STATIONARY_INIT;

	return RS_OK;
}

RS_IF rs_ret_t rs_2d_set_algo_param(rs_2d_handle_t handle, const struct rs_2d_algo_param *param)
{
	struct rs_2d_handle *lh = (struct rs_2d_handle *) handle;

	RET_CHECK(handle != NULL, RS_EINV);
	RET_CHECK(param != NULL, RS_EFAULT);
	RET_CHECK(lh->state == RS_2D_STATE_INIT, RS_ESTATE);

	RET_ORG(rs_ret_t, rs_2d_param_check(RS_2D_PARAM_MODE, param->mode), RS_OK);
	RET_ORG(rs_ret_t, rs_2d_param_check(RS_2D_PARAM_NTAP_AVE_DISTANCE, param->ntap_ave_distance), RS_OK);
	RET_ORG(rs_ret_t, rs_2d_param_check(RS_2D_PARAM_NTAP_AVE_ANGLE, param->ntap_ave_angle), RS_OK);
	RET_ORG(rs_ret_t, rs_2d_param_check(RS_2D_PARAM_NTAP_AVE_PEAK_LEVEL, param->ntap_ave_peak_level), RS_OK);
	RET_ORG(rs_ret_t, rs_2d_param_check(RS_2D_PARAM_SORT_ORDER, param->sort_order), RS_OK);
	RET_ORG(rs_ret_t, rs_2d_param_check(RS_2D_PARAM_LOWER_ANGLE, param->lower_angle), RS_OK);
	RET_ORG(rs_ret_t, rs_2d_param_check(RS_2D_PARAM_UPPER_ANGLE, param->upper_angle), RS_OK);
	RET_ORG(rs_ret_t, rs_2d_param_check(RS_2D_PARAM_ANGLE_THRESHOLD, param->angle_threshold), RS_OK);
	RET_ORG(rs_ret_t, rs_2d_param_check(RS_2D_PARAM_STATIONARY, param->stationary), RS_OK);

	RET_CHECK(param->lower_angle < param->upper_angle, RS_2D_ERANGE_LOWER_ANGLE);

	memcpy(&(lh->algo_param), param, sizeof(lh->algo_param));
	
	if (param->stationary){
		lh->nc = 10;
		lh->dmax = 20;
	}

	lh->state = RS_2D_STATE_EXECUTABLE;

	return RS_OK;
}

RS_IF rs_ret_t rs_2d_exec(rs_2d_handle_t handle, const struct rs_2rx_peak_data *data)
{
	struct rs_2d_handle *lh = (struct rs_2d_handle *) handle;

	RET_CHECK(handle != NULL, RS_EINV);
	RET_CHECK(data != NULL, RS_EFAULT);
	RET_CHECK(lh->state == RS_2D_STATE_EXECUTABLE, RS_ESTATE);

	memcpy(&(lh->last_data), data, sizeof(lh->last_data));

	return loc_input(handle, data);
}


RS_IF rs_ret_t rs_2d_get_location(rs_2d_handle_t handle, struct rs_2d_location_data *data)
{
	struct rs_2d_handle *lh = (struct rs_2d_handle *) handle;

	RET_CHECK(handle != NULL, RS_EINV);
	RET_CHECK(data != NULL, RS_EFAULT);
	RET_CHECK(lh->state == RS_2D_STATE_EXECUTABLE, RS_ESTATE);

	if(lh->algo_param.mode == RS_2D_MODE_RAW_ANGLE) {
		RET_ORG(rs_ret_t, angle_get(handle, 5, data), RS_OK);
	} else {
		RET_ORG(rs_ret_t, loc_get(handle, 5, data), RS_OK);
	}

	return RS_OK;
}
