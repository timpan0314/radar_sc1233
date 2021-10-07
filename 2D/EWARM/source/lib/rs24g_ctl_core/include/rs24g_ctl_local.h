/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_ctl_local.h
 */

#ifndef __RS24G_CTL_LOCAL_H__
#define __RS24G_CTL_LOCAL_H__

#include <stdint.h>

#include "rs24g_types.h"
#include "rs24g_ctl_base.h"
#include "rs24g_ctl_code.h"
#include "rs24g_resource_forwarders.h"

/*
 * Type Definition
 */

/*
 * Constant Definition
 */

/*
 * Struct Definition
 */

/*
 * Function Definition
 */
/* for setup library */
RS_IF rs_ret_t rs_get_setup_code_handle(rs_handle_t handle, rs_code_ref_t *code_handle);
RS_IF rs_ret_t rs_get_setup_res_handle(rs_handle_t handle, rs_resource_holder_t *res_handle);
RS_IF rs_ret_t rs_load_seqcode(rs_handle_t handle, rs_mode_t mode);
RS_IF rs_ret_t rs_write_reg(rs_handle_t handle, uint16_t addr, const uint32_t *val, rs_size_t num);
RS_IF rs_ret_t rs_read_reg(rs_handle_t handle, uint16_t addr, uint32_t *val, rs_size_t num);
RS_IF rs_ret_t rs_op_DSLEEP(rs_handle_t handle);
RS_IF rs_ret_t rs_update_param_in_ready_prep(rs_handle_t handle, rs_mode_t mode);

/* for debug */
RS_IF rs_ret_t rs_write_sram(rs_handle_t handle, uint32_t addr, const uint8_t *val, rs_size_t num);
RS_IF rs_ret_t rs_read_sram(rs_handle_t handle, uint32_t addr, uint8_t *val, rs_size_t num);

#endif	/* __RS24G_CTL_LOCAL_H__ */
