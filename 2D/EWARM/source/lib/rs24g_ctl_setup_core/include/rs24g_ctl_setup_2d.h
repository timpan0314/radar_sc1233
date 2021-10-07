/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_ctl_setup_2d.h
 */

#ifndef __RS24G_CTL_SETUP_2D_H__
#define __RS24G_CTL_SETUP_2D_H__

#include <stdint.h>

#include "rs24g_types.h"
#include "rs24g_ctl_base.h"
#include "rs24g_ctl_2rx.h"

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
struct rs_2rx_param
{
	uint32_t									rx_gain;
	uint32_t									interval;
	uint8_t										beta;
	uint32_t									lower_distance;
	uint32_t									upper_distance;
	uint32_t									chirp_time;
	rs_hpf_t									high_pass_filter;
	rs_step_t									distance_step;
};

/*
 * Function Definition
 */

#endif	/* __RS24G_CTL_SETUP_2D_H__ */
