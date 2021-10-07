/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file rs24g_ctl_distance.c
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "defs/rs_dev_addr.h"
#include "rs_ctl_dev.h"
#include "rs_ctl_cmd.h"
#include "rs_ctl_data.h"
#include "rs_ctl_op.h"

#include "rs24g_ctl_distance.h"
#include "rs24g_ctl_param.h"
#include "rs_handle.h"
#include "rs24g_ctl_common.h"


/**
 * get Distance
 * @callgraph
 */
RS_IF rs_ret_t rs_get_distance(rs_handle_t handle, uint32_t timeout, struct rs_distance_data *data)
{
	struct rs_handle *lh = (struct rs_handle *) handle;

	rs_ret_t ret = RS_OK;

	RET_CHECK(handle != NULL, RS_EINV);
	RET_CHECK(data != NULL, RS_EFAULT);

	ret = rs24g_mutex_lock(lh->mutex);

	if(ret == RS_OK) {
		if(lh->state != RS_STATE_SENSING) {
			ret = RS_ESTATE;
		}
		if(ret == RS_OK) {
			if(lh->mode != RS_MODE_DISTANCE_DETECT) {
				ret = RS_EMODE;
			}
		}

		if(ret == RS_OK) {
			const uint16_t reg_addr[] = {
				RS_CTL_REG_DISTANCE_RX1_12,
				RS_CTL_REG_DISTANCE_RX1_34,
				RS_CTL_REG_DISTANCE_RX1_5,
				RS_CTL_REG_PEAK_LEVEL_RX1_1,
				RS_CTL_REG_PEAK_LEVEL_RX1_2,
				RS_CTL_REG_PEAK_LEVEL_RX1_3,
				RS_CTL_REG_PEAK_LEVEL_RX1_4,
				RS_CTL_REG_PEAK_LEVEL_RX1_5,
				RS_CTL_REG_MY_COMPLEX_RX1_1,
				RS_CTL_REG_MY_COMPLEX_RX1_2,
				RS_CTL_REG_MY_COMPLEX_RX1_3,
				RS_CTL_REG_MY_COMPLEX_RX1_4,
				RS_CTL_REG_MY_COMPLEX_RX1_5,
			};
			uint32_t reg[ARRAY_SIZE(reg_addr)];

			ret = get_devdata(handle, timeout, reg_addr, ARRAY_SIZE(reg_addr), reg);

			if(ret == RS_OK) {
				ret = get_distance_data(reg, data, lh->peak_level_lower);
			}
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
 * set lower limit for peak level
 */
RS_IF rs_ret_t rs_set_peak_level_lower(rs_handle_t handle, uint8_t level)
{
	struct rs_handle *lh = (struct rs_handle *) handle;

	rs_ret_t ret = RS_OK;

	RET_CHECK(handle != NULL, RS_EINV);
	RET_CHECK(RS_PEAK_LEVEL_LOWER_MIN <= level, RS_EINV);
	RET_CHECK(level <= RS_PEAK_LEVEL_LOWER_MAX, RS_EINV);

	ret = rs24g_mutex_lock(lh->mutex);

	if(ret == RS_OK) {
		lh->peak_level_lower = level;

		ret = rs24g_mutex_unlock(lh->mutex);
	}

	return ret;
}

/**
 * get lower limit for peak level
 */
RS_IF rs_ret_t rs_get_peak_level_lower(rs_handle_t handle, uint8_t *level)
{
	struct rs_handle *lh = (struct rs_handle *) handle;

	rs_ret_t ret = RS_OK;

	RET_CHECK(handle != NULL, RS_EINV);
	RET_CHECK(level != NULL, RS_EFAULT);

	ret = rs24g_mutex_lock(lh->mutex);

	if(ret == RS_OK) {
		*level = lh->peak_level_lower;

		ret = rs24g_mutex_unlock(lh->mutex);
	}

	return ret;
}
