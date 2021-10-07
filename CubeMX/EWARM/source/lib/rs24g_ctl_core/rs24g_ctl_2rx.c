/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_ctl_2rx.c
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "defs/rs_dev_addr.h"
#include "defs/rs_dev_addr_2rx.h"
#include "rs_ctl_dev.h"
#include "rs_ctl_cmd.h"
#include "rs_ctl_data.h"
#include "rs_ctl_op.h"

#include "rs24g_ctl_distance.h"
#include "rs24g_ctl_2rx.h"
#include "rs24g_ctl_param.h"
#include "rs_handle.h"
#include "rs24g_ctl_common.h"


rs_ret_t get_2rx_peak_data(const uint32_t *regs, struct rs_2rx_peak_data *data, uint8_t peak_level_lower)
{
	int i;

	RET_ORG(rs_ret_t, get_distance_data(regs, &(data->distance), peak_level_lower), RS_OK);

	regs += 13;

	for(i=0; i<5; i++) {
		data->peak_rx2_real[i] = (int16_t) ((regs[i] >> 16) & 0xffff);
		data->peak_rx2_imag[i] = (int16_t) ((regs[i] >>  0) & 0xffff);
	}

	return RS_OK;
}

RS_IF rs_ret_t rs_get_2rx_peak(rs_handle_t handle, uint32_t timeout, struct rs_2rx_peak_data *data)
{
	struct rs_handle *lh = (struct rs_handle *) handle;

	rs_ret_t ret = RS_OK;

	RET_CHECK(handle != NULL, RS_EINV);
	RET_CHECK(data != NULL, RS_EFAULT);

	ret = rs24g_mutex_lock(lh->mutex);

	if(ret == RS_OK) {
		if(lh->state != RS_STATE_SENSING) {
			ret = RS_ESTATE;
		}
		if(ret == RS_OK) {
			if(lh->mode != RS_MODE_2D_DETECT) {
				ret = RS_EMODE;
			}
		}

		if(ret == RS_OK) {
			const uint16_t reg_addr[] = {
				RS_CTL_REG_DISTANCE_RX1_12,
				RS_CTL_REG_DISTANCE_RX1_34,
				RS_CTL_REG_DISTANCE_RX1_5,
				RS_CTL_REG_PEAK_LEVEL_RX1_1,
				RS_CTL_REG_PEAK_LEVEL_RX1_2,
				RS_CTL_REG_PEAK_LEVEL_RX1_3,
				RS_CTL_REG_PEAK_LEVEL_RX1_4,
				RS_CTL_REG_PEAK_LEVEL_RX1_5,
				RS_CTL_REG_MY_COMPLEX_RX1_1,
				RS_CTL_REG_MY_COMPLEX_RX1_2,
				RS_CTL_REG_MY_COMPLEX_RX1_3,
				RS_CTL_REG_MY_COMPLEX_RX1_4,
				RS_CTL_REG_MY_COMPLEX_RX1_5,
				RS_CTL_REG_OTHER_COMPLEX_RX1_1,
				RS_CTL_REG_OTHER_COMPLEX_RX1_2,
				RS_CTL_REG_OTHER_COMPLEX_RX1_3,
				RS_CTL_REG_OTHER_COMPLEX_RX1_4,
				RS_CTL_REG_OTHER_COMPLEX_RX1_5,
			};
			uint32_t reg[ARRAY_SIZE(reg_addr)];

			ret = get_devdata(handle, timeout, reg_addr, ARRAY_SIZE(reg_addr), reg);

			if(ret == RS_OK) {
				ret = get_2rx_peak_data(reg, data, lh->peak_level_lower);
			}
		}

		if(ret == RS_OK) {
			ret = rs24g_mutex_unlock(lh->mutex);
		} else {
			rs24g_mutex_unlock(lh->mutex);
		}
	}

	return ret;
}

