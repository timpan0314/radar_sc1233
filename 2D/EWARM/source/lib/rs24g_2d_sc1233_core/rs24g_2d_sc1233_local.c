/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_2d_sc1233_local.c
 */

#include <stdint.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "rs24g_ctl_sc1233.h"

#include "rs24g_2d_sc1233.h"
#include "rs24g_2d_sc1233_local.h"
#include "common_2d.h"
#include "location.h"
#include "rs_2d_handle_t.h"


RS_IF rs_ret_t rs_2d_get_location_all(rs_2d_handle_t handle, struct rs_2d_location_data *data)
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

RS_IF rs_ret_t rs_2d_get_internal_data(rs_2d_handle_t handle, struct rs_2d_internal_data *data)
{
	struct rs_2d_handle *lh = (struct rs_2d_handle *) handle;
	rs_size_t i;
	rs_size_t j;
	struct loc_est *eloc = NULL;
	struct loc_dev *dloc = NULL;

	data->order_size = rs_list_get_num(&(lh->data.loc_est.list));

	for(i=0; i<10; i++) {
		data->order[i].pos_size = 0;
	}

	for(i=0; rs_list_get_ele(&(lh->data.loc_est.list), i, (void **) &eloc); i++) {
		const rs_size_t order_idx = i;

		data->order[order_idx].pos_size = rs_list_get_num(&(eloc->loc_dev.list));
		data->order[order_idx].distance = (double) eloc->ave.na_distance;
		data->order[order_idx].angle = (double) eloc->ave.na_angle;
		data->order[order_idx].level = (double) eloc->ave.na_peak_level;
		data->order[order_idx].prob = (double) eloc->ave.all_input;

		for(j=0; rs_list_get_ele(&(eloc->loc_dev.list), j, (void **) &dloc); j++) {
			const rs_size_t pos_idx = j;
			data->order[order_idx].pos[pos_idx].distance = dloc->distance;
			data->order[order_idx].pos[pos_idx].level = dloc->peak_level;
			data->order[order_idx].pos[pos_idx].angle = (double) dloc->angle;
			data->order[order_idx].pos[pos_idx].prob = dloc->input == RS_TRUE ? 1 : 2;
		}
	}

	return RS_OK;
}
