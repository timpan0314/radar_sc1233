/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file rs24g_ctl_motion.c
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "rs_sleep.h"
#include "defs/rs_dev_term.h"
#include "rs_ctl_dev.h"
#include "rs_ctl_cmd.h"
#include "rs_ctl_data.h"
#include "rs_ctl_op.h"

#include "rs24g_ctl_motion.h"
#include "rs_handle.h"


/**
 * get Motion
 * @callgraph
 */
RS_IF rs_ret_t rs_get_motion(rs_handle_t handle, rs_bool_t *motion)
{
	struct rs_handle *lh = (struct rs_handle *) handle;

	rs_ret_t ret = RS_OK;

	RET_CHECK(handle != NULL, RS_EINV);
	RET_CHECK(motion != NULL, RS_EFAULT);

	ret = rs24g_mutex_lock(lh->mutex);

	if(ret == RS_OK) {
		if(lh->state != RS_STATE_SENSING) {
			ret = RS_ESTATE;
		}
		if(ret == RS_OK) {
			if(lh->mode != RS_MODE_MOTION_DETECT) {
				ret = RS_EMODE;
			}
		}

		if(ret == RS_OK) {
			ret = rs_ctl_dev_term_get(lh->dev, RS_TERM_DETOUT, motion);
		}
		if(ret == RS_OK) {
			ret = rs_ctl_dev_term_trig_clear(lh->dev, RS_TERM_DETOUT);
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
 * wait changeing motion
 * @callgraph
 */
RS_IF rs_ret_t rs_wait_motion_change(rs_handle_t handle, uint32_t timeout, rs_bool_t *motion)
{
	struct rs_handle *lh = (struct rs_handle *) handle;

	rs_ret_t ret = RS_OK;

	RET_CHECK(handle != NULL, RS_EINV);
	RET_CHECK(motion != NULL, RS_EFAULT);

	ret = rs24g_mutex_lock(lh->mutex);

	if(ret == RS_OK) {
		if(lh->state != RS_STATE_SENSING) {
			ret = RS_ESTATE;
		}
		if(ret == RS_OK) {
			if(lh->mode != RS_MODE_MOTION_DETECT) {
				ret = RS_EMODE;
			}
		}

		if(ret == RS_OK) {
			ret = rs_ctl_dev_term_trig_wait(lh->dev, timeout, RS_TERM_DETOUT, RS_TERM_TRIGGER_ALL, motion);
		}

		if(ret == RS_OK) {
			ret = rs24g_mutex_unlock(lh->mutex);
		} else {
			rs24g_mutex_unlock(lh->mutex);
		}
	}

	return ret;
}

static rs_ret_t rs_smoothed_level_mem_prep(rs_handle_t handle, uint32_t fft_point)
{
	struct rs_handle *lh = (struct rs_handle *) handle;
	uint32_t rdata;
	uint32_t reg_fft_point;

	if(fft_point == 512) {
		reg_fft_point = 0x00040000;
	} else if(fft_point == 256) {
		reg_fft_point = 0x00020000;
	} else {
		reg_fft_point = 0x00000000;
	}

	RS_CALL(rs_ctl_op_SRST(lh->dev, 1));
	rs_usleep(5 * 1000);
	RS_CALL(rs_ctl_write_reg(lh->dev, RS_CTL_REG_SELDIG_09, 0x00060000));
	RS_CALL(rs_ctl_write_reg(lh->dev, RS_CTL_REG_SELDIG_03, reg_fft_point));
	RS_CALL(rs_ctl_read_reg(lh->dev, RS_CTL_REG_PRESDET_00, &rdata));
	RS_CALL(rs_ctl_write_reg(lh->dev, RS_CTL_REG_PRESDET_00, 0x10000000 | rdata));

	return RS_OK;
}

static rs_ret_t rs_smoothed_level_mem_postp(rs_handle_t handle)
{
	struct rs_handle *lh = (struct rs_handle *) handle;
	uint32_t rdata;

	RS_CALL(rs_ctl_read_reg(lh->dev, RS_CTL_REG_PRESDET_00, &rdata));
	RS_CALL(rs_ctl_write_reg(lh->dev, RS_CTL_REG_PRESDET_00, 0xEFFFFFFF & rdata));
	RS_CALL(rs_ctl_write_reg(lh->dev, RS_CTL_REG_SELDIG_09, 0x00000000));
	rs_ctl_op_DSLEEP(lh->dev);

	return RS_OK;
}

static rs_ret_t rs_set_smoothed_level_operation(rs_handle_t handle, rs_smoothedope_t smoothed_level_ope)
{
	struct rs_handle *lh = (struct rs_handle *) handle;
	uint32_t rdata;
	uint32_t reg_smoothed_level_ope = (((uint32_t) smoothed_level_ope) << 24);

	RS_CALL(rs_ctl_read_reg(lh->dev, RS_CTL_REG_PRESDET_01, &rdata));
	RS_CALL(rs_ctl_write_reg(lh->dev, RS_CTL_REG_PRESDET_01, reg_smoothed_level_ope | (rdata & 0xfeffffff)));

	return RS_OK;
}

/**
 * setup smoothed level
 * @callgraph
 */
RS_IF rs_ret_t rs_setup_smoothed_level(rs_handle_t handle, rs_smoothedope_t smoothed_level_ope, uint32_t fft_point, uint16_t addr, const uint32_t *val, rs_size_t num)
{
	struct rs_handle *lh = (struct rs_handle *) handle;

	rs_ret_t ret = RS_OK;

	RET_CHECK(handle != NULL, RS_EINV);
	RET_CHECK(val != NULL, RS_EFAULT);
	RET_CHECK(smoothed_level_ope == RS_SMOOTHED_LEVEL_UPDATE || smoothed_level_ope == RS_SMOOTHED_LEVEL_HOLD, RS_EINV);
	RET_CHECK(num != 0, RS_EINV);
	RET_CHECK(fft_point == 512 || fft_point == 256 || fft_point == 128, RS_EINV);
	RET_CHECK(addr + num <= fft_point, RS_EINV);

	ret = rs24g_mutex_lock(lh->mutex);

	if(ret == RS_OK) {
		if(lh->state != RS_STATE_READY) {
			ret = RS_ESTATE;
		}

		if(ret == RS_OK) {
			if(lh->mode != RS_MODE_MOTION_DETECT) {
				ret = RS_EMODE;
			}
		}

		if(ret == RS_OK) {
			ret = rs_smoothed_level_mem_prep(handle, fft_point);
		}

		if(ret == RS_OK) {
			ret = rs_ctl_write_regs(lh->dev, addr | RS_CTL_TOPADDR_PRESDET1, val, num);
		}

		if(ret == RS_OK) {
			ret = rs_set_smoothed_level_operation(handle, smoothed_level_ope);
		}

		if(ret == RS_OK) {
			ret = rs_smoothed_level_mem_postp(handle);
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
 * read smoothed level
 * @callgraph
 */
RS_IF rs_ret_t rs_read_smoothed_level(rs_handle_t handle, uint32_t fft_point, uint16_t addr, uint32_t *val, rs_size_t num)
{
	struct rs_handle *lh = (struct rs_handle *) handle;

	rs_ret_t ret = RS_OK;

	RET_CHECK(handle != NULL, RS_EINV);
	RET_CHECK(val != NULL, RS_EFAULT);
	RET_CHECK(num != 0, RS_EINV);
	RET_CHECK(fft_point == 512 || fft_point == 256 || fft_point == 128, RS_EINV);
	RET_CHECK(addr + num <= fft_point, RS_EINV);

	ret = rs24g_mutex_lock(lh->mutex);

	if(ret == RS_OK) {
		if(lh->state != RS_STATE_READY) {
			ret = RS_ESTATE;
		}

		if(ret == RS_OK) {
			if(lh->mode != RS_MODE_MOTION_DETECT) {
				ret = RS_EMODE;
			}
		}

		if(ret == RS_OK) {
			ret = rs_smoothed_level_mem_prep(handle, fft_point);
		}

		if(ret == RS_OK) {
			ret = rs_ctl_read_regs(lh->dev, addr | RS_CTL_TOPADDR_PRESDET1, val, num);
		}

		if(ret == RS_OK) {
			ret = rs_smoothed_level_mem_postp(handle);
		}

		if(ret == RS_OK) {
			ret = rs24g_mutex_unlock(lh->mutex);
		} else {
			rs24g_mutex_unlock(lh->mutex);
		}
	}

	return ret;
}
