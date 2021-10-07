/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_ctl_setup_base.h
 */

#ifndef __RS24G_CTL_SETUP_BASE_H__
#define __RS24G_CTL_SETUP_BASE_H__

#include <stdint.h>

#include "rs24g_types.h"
#include "rs24g_ctl_base.h"

/*
 * Type Definition
 */
typedef int32_t									rs_shaper_t;
typedef int32_t									rs_hpf_t;
typedef int32_t									rs_step_t;

/*
 * Constant Definition
 */
#define RS_DIGITAL_BEAM_SHAPER_WIDE				((rs_shaper_t) (0))
#define RS_DIGITAL_BEAM_SHAPER_NARROW			((rs_shaper_t) (1))

#define RS_HPF_FIRST_ORDER						((rs_hpf_t) (1))
#define RS_HPF_SECOND_ORDER						((rs_hpf_t) (2))

#define RS_DISTANCE_STEP_NORMAL					((rs_step_t) (0))
#define RS_DISTANCE_STEP_FINE					((rs_step_t) (1))
#define RS_DISTANCE_STEP_XFINE					((rs_step_t) (2))

#define RS_C								((double)2.998 * 10000000000.0)

/*
 * Struct Definition
 */

/*
 * Function Definition
 */
RS_IF rs_ret_t rs_setup_param(rs_handle_t handle, rs_mode_t mode, const void *param);
RS_IF rs_ret_t rs_param_init(rs_mode_t mode, void *param);
RS_IF rs_ret_t rs_get_setup_lib_ver(uint16_t *major_ver, uint16_t *minor_ver, uint16_t *revision, uint16_t *hotfix);
RS_IF rs_ret_t rs_update_param(rs_handle_t handle, rs_mode_t mode, const void *param);

#endif	/* __RS24G_CTL_SETUP_BASE_H__ */
