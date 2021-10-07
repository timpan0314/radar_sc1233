/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_ctl_setup_distance.h
 */

#ifndef __RS24G_CTL_SETUP_DISTANCE_H__
#define __RS24G_CTL_SETUP_DISTANCE_H__

#include <stdint.h>

#include "rs24g_types.h"
#include "rs24g_ctl_base.h"
#include "rs24g_ctl_distance.h"

#include "rs24g_ctl_setup_base.h"

/*
 * Type Definition
 */

/*
 * Constant Definition
 */

/*
 * Struct Definition
 */
struct rs_distance_param
{
	uint32_t									rx_gain;
	uint32_t									interval;
	uint8_t										beta;
	uint32_t									lower_distance;
	uint32_t									upper_distance;
	uint32_t									chirp_time;
	rs_shaper_t									digital_beam_shaper;
	rs_hpf_t									high_pass_filter;
	rs_step_t									distance_step;
};

struct rs_update_distance_param
{
	uint32_t									interval;
	uint8_t										beta;
	uint32_t									lower_distance;
	uint32_t									upper_distance;
	rs_hpf_t									high_pass_filter;
};

/*
 * Function Definition
 */
RS_IF rs_ret_t rs_param_distance_range_check(rs_step_t dist_step, uint32_t upper, uint32_t lower);

#endif	/* __RS24G_CTL_SETUP_DISTANCE_H__ */
