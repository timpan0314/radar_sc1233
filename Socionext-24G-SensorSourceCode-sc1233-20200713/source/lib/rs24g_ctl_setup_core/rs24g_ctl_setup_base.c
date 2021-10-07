/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file rs24g_ctl_setup_base.c
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "rs24g_ctl_base.h"
#include "rs24g_ctl_local.h"

#include "rs24g_ctl_setup_base.h"
#include "rs24g_ctl_setup_local.h"
#include "setup_common.h"
#include "setup_base_local.h"


RS_IF rs_ret_t rs_param_init(rs_mode_t mode, void *param)
{
	RET_CHECK(param != NULL, RS_EFAULT);

	RET_ORG(rs_ret_t, rs_param_init_local(mode, param), RS_OK);

	return RS_OK;
}

/**
 * Setup Parameters
 *
 * @param[in] handle Sensor Handle
 * @param[in] mode Sensor Operation Mode
 * @param[in] param Sensor Parameters
 * @callgraph
 */
RS_IF rs_ret_t rs_setup_param(rs_handle_t handle, rs_mode_t mode, const void *param)
{
	rs_code_ref_t code;
	rs_resource_holder_t res;
	rs_mode_t ctl_mode;

	RET_CHECK(param != NULL, RS_EFAULT);

	RET_ORG(rs_ret_t, rs_get_setup_code_handle(handle, &code), RS_OK);
	RET_ORG(rs_ret_t, rs_get_setup_res_handle(handle, &res), RS_OK);
	RET_ORG(rs_ret_t, rs_setup_param_local(mode, param, code, res), RS_OK);
	RET_ORG(rs_ret_t, rs_get_ctl_mode_from_setup_mode(mode, res, &ctl_mode), RS_OK);
	RET_ORG(rs_ret_t, rs_load_seqcode(handle, ctl_mode), RS_OK);
	RET_ORG(rs_ret_t, rs_update_param_local(handle, mode, param), RS_OK);
	RET_ORG(rs_ret_t, rs_op_DSLEEP(handle), RS_OK);

	return RS_OK;
}

/**
 * Update Parameters
 *
 * @param[in] handle Sensor Handle
 * @param[in] mode Sensor Operation Mode
 * @param[in] param Sensor Parameters for updates
 * @callgraph
 */
RS_IF rs_ret_t rs_update_param(rs_handle_t handle, rs_mode_t mode, const void *param)
{
	RET_CHECK(param != NULL, RS_EFAULT);

	RET_ORG(rs_ret_t, rs_update_param_in_ready_check(handle, mode, param), RS_OK);
	RET_ORG(rs_ret_t, rs_update_param_in_ready_prep(handle, mode), RS_OK);
	RET_ORG(rs_ret_t, rs_update_param_in_ready_local(handle, mode, param), RS_OK);
	RET_ORG(rs_ret_t, rs_op_DSLEEP(handle), RS_OK);

	return RS_OK;
}
