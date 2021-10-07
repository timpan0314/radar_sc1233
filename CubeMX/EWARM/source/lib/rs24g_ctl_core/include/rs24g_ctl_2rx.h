/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_ctl_2rx.h
 */

#ifndef __RS24G_CTL_2RX_H__
#define __RS24G_CTL_2RX_H__

#include <stdint.h>

#include "rs24g_types.h"

#include "rs24g_ctl_base.h"
#include "rs24g_ctl_distance.h"

/*
 * Type Definition
 */

/*
 * Constant Definition
 */
#define RS_MODE_2D_DETECT						((rs_mode_t) (2))

/*
 * Struct Definition
 */
struct rs_2rx_peak_data
{
	struct rs_distance_data						distance;
	int16_t										peak_rx2_real[5];
	int16_t										peak_rx2_imag[5];
};

/*
 * Function Definition
 */
RS_IF rs_ret_t rs_get_2rx_peak(rs_handle_t handle, uint32_t timeout, struct rs_2rx_peak_data *data);

#endif	/* __RS24G_CTL_2RX_H__ */
