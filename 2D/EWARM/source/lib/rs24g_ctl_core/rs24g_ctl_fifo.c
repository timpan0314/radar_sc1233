/*
 * Copyright (c) 2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file rs24g_ctl_fifo.c
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "defs/rs_dev_addr.h"
#include "rs_ctl_dev.h"
#include "rs_ctl_cmd.h"
#include "rs_ctl_data.h"
#include "rs_ctl_op.h"

#include "rs24g_ctl_local.h"
#include "rs_handle.h"
#include "rs24g_ctl_common.h"


/**
 * get frame size in FIFO
 * @callgraph
 */
RS_IF rs_ret_t rs_get_fifo_size(rs_handle_t handle, rs_size_t *size)
{
	struct rs_handle *lh = (struct rs_handle *) handle;

	rs_ret_t ret = RS_OK;

	RET_CHECK(handle != NULL, RS_EINV);
	RET_CHECK(size != NULL, RS_EFAULT);

	*size = 0;

	ret = rs24g_mutex_lock(lh->mutex);

	if(ret == RS_OK) {
		if((lh->state != RS_STATE_READY) && (lh->state != RS_STATE_SENSING)) {
			ret = RS_ESTATE;
		}
		if(ret == RS_OK) {
			if(lh->mode != RS_MODE_FIFO) {
				ret = RS_EMODE;
			}
		}

		if(ret == RS_OK) {
			ret = get_size_fifo(handle, size);
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
 * get FIFO data
 * @callgraph
 */
RS_IF rs_ret_t rs_read_fifo(rs_handle_t handle, uint32_t timeout, uint8_t *data, rs_size_t size, struct rs_fifo_info *info)
{
	struct rs_handle *lh = (struct rs_handle *) handle;

	rs_ret_t ret = RS_OK;

	RET_CHECK(handle != NULL, RS_EINV);
	RET_CHECK(data != NULL, RS_EFAULT);
	RET_CHECK(info != NULL, RS_EFAULT);

	memset(data, 0, size);
	info->status = 0xffffffff;
	info->header = 0xffffffff;
	info->end = 0;

	ret = rs24g_mutex_lock(lh->mutex);

	if(ret == RS_OK) {
		if(lh->state != RS_STATE_SENSING) {
			ret = RS_ESTATE;
		}
		if(ret == RS_OK) {
			if(lh->mode != RS_MODE_FIFO) {
				ret = RS_EMODE;
			}
		}

		if(ret == RS_OK) {
			ret = get_devfifo(handle, timeout, size, data, info);
		}

		if(ret == RS_OK) {
			ret = rs24g_mutex_unlock(lh->mutex);
		} else {
			rs24g_mutex_unlock(lh->mutex);
		}
	}

	return ret;
}
