/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_ctl_base.h
 */

#ifndef __RS24G_CTL_BASE_H__
#define __RS24G_CTL_BASE_H__

#include <stdint.h>

#include "rs24g_types.h"

/*
 * Type Definition
 */
typedef void *									rs_handle_t;
typedef int32_t									rs_state_t;
typedef int32_t									rs_mode_t;

/*
 * Constant Definition
 */
#define RS_STATE_SHUTDOWN						((rs_state_t) (0))
#define RS_STATE_READY							((rs_state_t) (1))
#define RS_STATE_SENSING						((rs_state_t) (2))

/*
 * Struct Definition
 */

/*
 * Function Definition
 */
RS_IF rs_ret_t rs_open(rs_handle_t *handle);
RS_IF rs_ret_t rs_close(rs_handle_t handle);

RS_IF rs_ret_t rs_shutdown(rs_handle_t handle);
RS_IF rs_ret_t rs_start(rs_handle_t handle);
RS_IF rs_ret_t rs_stop(rs_handle_t handle);
RS_IF rs_ret_t rs_resume(rs_handle_t handle);

RS_IF rs_ret_t rs_get_state(rs_handle_t handle, rs_state_t *state);
RS_IF rs_ret_t rs_get_sensor_lib_ver(uint16_t *major_ver, uint16_t *minor_ver, uint16_t *revision, uint16_t *hotfix);

#endif	/* __RS24G_CTL_BASE_H__ */
