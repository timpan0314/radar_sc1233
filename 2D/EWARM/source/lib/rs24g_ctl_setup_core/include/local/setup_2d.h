/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * setup_2d.h
 */

#ifndef __SETUP_2D_H__
#define __SETUP_2D_H__

#include <stdint.h>

#include "rs24g_types.h"
#include "rs24g_ctl_setup_base.h"
#include "rs24g_ctl_setup_distance.h"
#include "rs24g_ctl_setup_2d.h"
#include "setup_common.h"


#define RS_2D_RX_GAIN_INIT						(60)
#define RS_2D_INTERVAL_INIT						(25)
#define RS_2D_BETA_INIT							(205)
#define RS_2D_LOWER_DISTANCE_INIT				(10)
#define RS_2D_UPPER_DISTANCE_INIT				(1000)
#define RS_2D_CHIRP_TIME_INIT					(1100)
#define RS_2D_HPF_INIT							(RS_HPF_FIRST_ORDER)
#define RS_2D_DISTANCE_STEP_INIT				(RS_DISTANCE_STEP_NORMAL)


extern rs_ret_t mode_2d_init(struct rs_2rx_param *lp);
extern rs_ret_t mode_2d_getcode(const struct rs_2rx_param *lp, rs_code_ref_t code);
extern rs_ret_t mode_2d_update_param(rs_handle_t handle, const struct rs_2rx_param *lp);
extern rs_ret_t mode_2d_update_param_in_ready(rs_handle_t handle, const struct rs_update_distance_param *lp);
extern rs_ret_t mode_2d_update_param_check(rs_handle_t handle, const struct rs_update_distance_param *lp);

#endif	/* __SETUP_2D_H__ */
