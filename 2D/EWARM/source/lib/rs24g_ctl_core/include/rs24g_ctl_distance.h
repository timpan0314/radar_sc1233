/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_ctl_distance.h
 */

#ifndef __RS24G_CTL_DISTANCE_H__
#define __RS24G_CTL_DISTANCE_H__

#include <stdint.h>

#include "rs24g_types.h"

#include "rs24g_ctl_base.h"

/*
 * Type Definition
 */

/*
 * Constant Definition
 */
#define RS_MODE_DISTANCE_DETECT					((rs_mode_t) (1))

/*
 * Struct Definition
 */
struct rs_distance_data
{
	int16_t										distance[5];
	uint8_t										peak_level[5];
	int16_t										peak_real[5];
	int16_t										peak_imag[5];
};

/*
 * Function Definition
 */
RS_IF rs_ret_t rs_set_peak_level_lower(rs_handle_t handle, uint8_t level);
RS_IF rs_ret_t rs_get_peak_level_lower(rs_handle_t handle, uint8_t *level);
RS_IF rs_ret_t rs_get_distance(rs_handle_t handle, uint32_t timeout, struct rs_distance_data *data);

#endif	/* __RS24G_CTL_DISTANCE_H__ */
