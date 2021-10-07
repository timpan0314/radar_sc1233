/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file setup_base_local.h
 */

#ifndef __SETUP_BASE_LOCAL_H__
#define __SETUP_BASE_LOCAL_H__

#include <stdint.h>

#include "rs24g_types.h"
#include "rs_macro.h"

#include "rs24g_ctl_base.h"
#include "setup_motion_wide.h"
#include "setup_distance_wide.h"
#include "setup_2d.h"
#include "setup_fifo_fft_1rx_wide.h"
#include "setup_fifo_fft_2rx_wide.h"
#include "setup_fifo_iq_1rx_wide.h"
#include "setup_fifo_iq_2rx_wide.h"
#include "setup_bin.h"


static __inline rs_ret_t rs_param_init_local(rs_mode_t mode, void *param)
{
	switch(mode) {
	case RS_MODE_MOTION_DETECT:
		RET_ORG(rs_ret_t, motion_init(param), RS_OK);
		break;
	case RS_MODE_DISTANCE_DETECT:
		RET_ORG(rs_ret_t, distance_init(param), RS_OK);
		break;
	case RS_MODE_2D_DETECT:
		RET_ORG(rs_ret_t, mode_2d_init(param), RS_OK);
		break;
	case RS_MODE_FIFO:
		RET_ORG(rs_ret_t, fifo_init(param), RS_OK);
		break;
	default:
		return RS_ERANGE_MODE_TYPE;
	}

	return RS_OK;
}

/**
 * prepare parameters
 * @callergraph
 */
static __inline rs_ret_t rs_setup_param_local(rs_mode_t mode, const void *param, rs_code_ref_t code, rs_resource_holder_t res)
{
	switch(mode) {
	case RS_MODE_MOTION_DETECT:
		RET_ORG(rs_ret_t, motion_wide_getcode(param, code), RS_OK);
		break;
	case RS_MODE_DISTANCE_DETECT:
		RET_ORG(rs_ret_t, distance_wide_getcode(param, code), RS_OK);
		break;
	case RS_MODE_2D_DETECT:
		RET_ORG(rs_ret_t, mode_2d_getcode(param, code), RS_OK);
		break;
	case RS_MODE_FIFO:
		switch(((const struct rs_fifo_param *) param)->format) {
		case RS_FIFO_FORMAT_FFT:
			switch(((const struct rs_fifo_param *) param)->rx_num) {
			case 1:
				RET_ORG(rs_ret_t, fifo_fft_1rx_wide_getcode(param, code), RS_OK);
				break;
			case 2:
				RET_ORG(rs_ret_t, fifo_fft_2rx_wide_getcode(param, code), RS_OK);
				break;
			default:
				return RS_ERANGE_RX_NUM;
			}
			break;
		case RS_FIFO_FORMAT_IQ:
			switch(((const struct rs_fifo_param *) param)->rx_num) {
			case 1:
				RET_ORG(rs_ret_t, fifo_iq_1rx_wide_getcode(param, code), RS_OK);
				break;
			case 2:
				RET_ORG(rs_ret_t, fifo_iq_2rx_wide_getcode(param, code), RS_OK);
				break;
			default:
				return RS_ERANGE_RX_NUM;
			}
			break;
		default:
			return RS_ERANGE_FIFO_FORMAT;
		}
		break;
	case RS_MODE_BIN:
		RET_ORG(rs_ret_t, bin_getcode(param, 3, code, res), RS_OK);
		break;
	default:
		return RS_ERANGE_MODE_TYPE;
	}

	return RS_OK;
}

/**
 * get mode for control
 * @callergraph
 */
static __inline rs_ret_t rs_get_ctl_mode_from_setup_mode(rs_mode_t setup_mode, rs_resource_holder_t res, rs_mode_t *p_ctl_mode)
{
	RS_ASSERT(p_ctl_mode != NULL, RS_EFAULT);

	if (setup_mode == RS_MODE_BIN) {
		RS_CALL(bin_getmode(res, p_ctl_mode));
	} else {
		*p_ctl_mode = setup_mode;
	}

	return RS_OK;
}

/**
 * update parameters
 * @callergraph
 */
static __inline rs_ret_t rs_update_param_local(rs_handle_t handle, rs_mode_t mode, const void *param)
{
	switch(mode) {
	case RS_MODE_MOTION_DETECT:
		RET_ORG(rs_ret_t, motion_wide_update_param(handle, param), RS_OK);
		break;
	case RS_MODE_DISTANCE_DETECT:
		RET_ORG(rs_ret_t, distance_wide_update_param(handle, param), RS_OK);
		break;
	case RS_MODE_2D_DETECT:
		RET_ORG(rs_ret_t, mode_2d_update_param(handle, param), RS_OK);
		break;
	case RS_MODE_FIFO:
		switch(((const struct rs_fifo_param *) param)->format) {
		case RS_FIFO_FORMAT_FFT:
			switch(((const struct rs_fifo_param *) param)->rx_num) {
			case 1:
				RET_ORG(rs_ret_t, fifo_fft_1rx_wide_update_param(handle, param), RS_OK);
				break;
			case 2:
				RET_ORG(rs_ret_t, fifo_fft_2rx_wide_update_param(handle, param), RS_OK);
				break;
			default:
				return RS_ERANGE_RX_NUM;
			}
			break;
		case RS_FIFO_FORMAT_IQ:
			switch(((const struct rs_fifo_param *) param)->rx_num) {
			case 1:
				RET_ORG(rs_ret_t, fifo_iq_1rx_wide_update_param(handle, param), RS_OK);
				break;
			case 2:
				RET_ORG(rs_ret_t, fifo_iq_2rx_wide_update_param(handle, param), RS_OK);
				break;
			default:
				return RS_ERANGE_RX_NUM;
			}
			break;
		default:
			return RS_ERANGE_FIFO_FORMAT;
		}
		break;
	case RS_MODE_BIN:
		break;
	default:
		return RS_ERANGE_MODE_TYPE;
	}

	return RS_OK;
}

/*
 * update parameters in ready state
 */
static __inline rs_ret_t rs_update_param_in_ready_local(rs_handle_t handle, rs_mode_t mode, const void *param)
{
	switch(mode) {
	case RS_MODE_MOTION_DETECT:
		RET_ORG(rs_ret_t, motion_update_param_in_ready(handle, param), RS_OK);
		break;
	case RS_MODE_DISTANCE_DETECT:
		RET_ORG(rs_ret_t, distance_update_param_in_ready(handle, param), RS_OK);
		break;
	case RS_MODE_2D_DETECT:
		RET_ORG(rs_ret_t, mode_2d_update_param_in_ready(handle, param), RS_OK);
		break;
	default:
		return RS_ERANGE_MODE_TYPE;
	}

	return RS_OK;
}

/*
 * check update parameters in ready state
 */
static __inline rs_ret_t rs_update_param_in_ready_check(rs_handle_t handle, rs_mode_t mode, const void *param)
{
	switch(mode) {
	case RS_MODE_MOTION_DETECT:
		RET_ORG(rs_ret_t, motion_update_param_check(handle, param), RS_OK);
		break;
	case RS_MODE_DISTANCE_DETECT:
		RET_ORG(rs_ret_t, distance_update_param_check(handle, param), RS_OK);
		break;
	case RS_MODE_2D_DETECT:
		RET_ORG(rs_ret_t, mode_2d_update_param_check(handle, param), RS_OK);
		break;
	default:
		return RS_ERANGE_MODE_TYPE;
	}

	return RS_OK;
}

#endif	/* __SETUP_BASE_LOCAL_H__ */
