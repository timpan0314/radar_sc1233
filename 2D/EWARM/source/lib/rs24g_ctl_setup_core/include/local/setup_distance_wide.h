/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * setup_distance_wide.h
 */

#ifndef __SETUP_DISTANCE_WIDE_H__
#define __SETUP_DISTANCE_WIDE_H__

#include <stdint.h>

#include "rs24g_types.h"
#include "rs24g_ctl_setup_base.h"
#include "rs24g_ctl_setup_distance.h"

#include "setup_common.h"
#include "setup_distance.h"


extern rs_ret_t distance_wide_getcode(const struct rs_distance_param *lp, rs_code_ref_t code);
extern rs_ret_t distance_wide_update_param(rs_handle_t handle, const struct rs_distance_param *lp);

#endif	/* __SETUP_DISTANCE_WIDE_H__ */
