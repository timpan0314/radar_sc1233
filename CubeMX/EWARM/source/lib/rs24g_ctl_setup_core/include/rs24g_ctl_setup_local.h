/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_ctl_setup_local.h
 */

#ifndef __RS24G_CTL_SETUP_LOCAL_H__
#define __RS24G_CTL_SETUP_LOCAL_H__

#include <stdint.h>

#include "rs24g_types.h"
#include "rs24g_ctl_base.h"

#include "rs24g_ctl_setup_base.h"

/*
 * Type Definition
 */
typedef int32_t									rs_param_t;

/*
 * Constant Definition
 */
#define RS_PARAM_RX_GAIN						((rs_param_t) (1))
#define RS_PARAM_INTERVAL						((rs_param_t) (2))
#define RS_PARAM_ALPHA							((rs_param_t) (3))
#define RS_PARAM_MOTION_THRESHOLD				((rs_param_t) (4))
#define RS_PARAM_STABLE_COUNT					((rs_param_t) (5))
#define RS_PARAM_LOWER_DISTANCE					((rs_param_t) (6))
#define RS_PARAM_UPPER_DISTANCE					((rs_param_t) (7))
#define RS_PARAM_CHIRP_TIME						((rs_param_t) (8))
#define RS_PARAM_BETA							((rs_param_t) (9))
#define RS_PARAM_HIGH_PASS_FILTER				((rs_param_t) (10))
#define RS_PARAM_DIGITAL_BEAM_SHAPER			((rs_param_t) (11))
#define RS_PARAM_DISTANCE_STEP					((rs_param_t) (12))

/*
 * Struct Definition
 */
struct rs_param_type_range
{
	int32_t min;
	int32_t max;
};

struct rs_param_type_choice
{
	const int32_t *choice;
	rs_size_t num;
};

/*
 * Function Definition
 */
RS_IF rs_ret_t rs_param_get_range(rs_param_t param, rs_mode_t mode, struct rs_param_type_range *data);
RS_IF rs_ret_t rs_param_get_choice(rs_param_t param, rs_mode_t mode, struct rs_param_type_choice *data);
RS_IF rs_ret_t rs_param_check(rs_param_t param, rs_mode_t mode, int32_t val);

#endif	/* __RS24G_CTL_SETUP_LOCAL_H__ */
