/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_ctl_setup_motion.h
 */

#ifndef __RS24G_CTL_SETUP_MOTION_H__
#define __RS24G_CTL_SETUP_MOTION_H__

#include <stdint.h>

#include "rs24g_types.h"
#include "rs24g_ctl_base.h"
#include "rs24g_ctl_motion.h"

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
struct rs_motion_param
{
	uint32_t									rx_gain;
	uint32_t									interval;
	uint8_t										alpha;
	uint16_t									motion_threshold;
	uint8_t										startup_count;
	uint32_t									lower_distance;
	uint32_t									upper_distance;
	uint32_t									chirp_time;
	rs_shaper_t									digital_beam_shaper;
	rs_hpf_t									high_pass_filter;
};

struct rs_update_motion_param
{
	uint32_t									interval;
	uint8_t										alpha;
	uint16_t									motion_threshold;
	uint8_t										startup_count;
	uint32_t									lower_distance;
	uint32_t									upper_distance;
	rs_hpf_t									high_pass_filter;
};

/*
 * Function Definition
 */

#endif	/* __RS24G_CTL_SETUP_MOTION_H__ */
