/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file rs24g_ctl_sensor_ctl_type.c
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rs24g_types.h"
#include "rs_macro.h"

#include "rs24g_ctl_sensor_ctl_type.h"
#include "rs_handle.h"


/**
 * set sensor control type
 *
 * @callgraph
 */
RS_IF rs_ret_t rs_set_sensor_ctl_type(rs_handle_t handle, rs_sensor_ctl_type_t sensor_ctl_type)
{
	struct rs_handle *lh = (struct rs_handle *) handle;

	rs_ret_t ret = RS_OK;

	RET_CHECK(handle != NULL, RS_EINV);
	RET_CHECK(sensor_ctl_type == RS_SENSOR_CTL_TIMER || sensor_ctl_type == RS_SENSOR_CTL_CPU, RS_EINV);

	ret = rs24g_mutex_lock(lh->mutex);

	if(ret == RS_OK) {
		if(lh->state != RS_STATE_READY) {
			ret = RS_ESTATE;
		}

		if(ret == RS_OK) {
			lh->sensor_ctl_type = sensor_ctl_type;
		}

		if(ret == RS_OK) {
			ret = rs24g_mutex_unlock(lh->mutex);
		} else {
			rs24g_mutex_unlock(lh->mutex);
		}
	}

	return ret;
}

/**
 * get sensor control type
 *
 * @callgraph
 */
RS_IF rs_ret_t rs_get_sensor_ctl_type(rs_handle_t handle, rs_sensor_ctl_type_t *sensor_ctl_type)
{
	struct rs_handle *lh = (struct rs_handle *) handle;

	rs_ret_t ret = RS_OK;

	RET_CHECK(handle != NULL, RS_EINV);
	RET_CHECK(sensor_ctl_type != NULL, RS_EFAULT);

	if(ret == RS_OK) {
		if(lh->state != RS_STATE_READY) {
			ret = RS_ESTATE;
		}

		if(ret == RS_OK) {
			*sensor_ctl_type = lh->sensor_ctl_type;
		}

		if(ret == RS_OK) {
			ret = rs24g_mutex_unlock(lh->mutex);
		} else {
			rs24g_mutex_unlock(lh->mutex);
		}
	}

	return ret;
}
