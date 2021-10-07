/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file setup_fifo_iq_2rx_wide.c
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "rs24g_ctl_setup_base.h"
#include "rs24g_ctl_param_range.h"
#include "rs24g_ctl_setup_local.h"
#include "rs24g_ctl_setup_fifo.h"
#include "setup_common.h"
#include "setup_fifo_iq_2rx_wide.h"
#include "setup_selection.h"

#include "rs24g_initial_dataset.h"
#include "setup/evk/fifo_iq_2rx_wide/all.h"


/**
 * prepare parameters for fifo detection
 * @callergraph
 */
rs_ret_t fifo_iq_2rx_wide_getcode(const struct rs_fifo_param *lp, rs_code_ref_t code)
{
	const struct initial_dataset *setup = NULL;

	RET_CHECK(lp->format== RS_FIFO_FORMAT_IQ, RS_ERANGE_FIFO_FORMAT);
	RET_ORG(rs_ret_t, rs_param_check(RS_PARAM_INTERVAL, RS_MODE_FIFO, lp->interval), RS_OK);
	RET_CHECK(lp->rx_num == 2, RS_ERANGE_RX_NUM);
	RET_CHECK(lp->digital_beam_shaper == RS_DIGITAL_BEAM_SHAPER_WIDE, RS_ERANGE_DIGITAL_BEAM_SHAPER);

	switch(lp->rx_gain) {
	case 40:
		switch(lp->chirp_time) {
#if RS_SETUP_MODE_FIFO_IQ_2RX_WIDE_40DB_220US
		case 220:		setup = &mode_fifo_iq_2rx_wide_40db_220us;		break;
#endif
#if RS_SETUP_MODE_FIFO_IQ_2RX_WIDE_40DB_1100US
		case 1100:		setup = &mode_fifo_iq_2rx_wide_40db_1100us;	break;
#endif
#if RS_SETUP_MODE_FIFO_IQ_2RX_WIDE_40DB_4400US
		case 4400:		setup = &mode_fifo_iq_2rx_wide_40db_4400us;	break;
#endif
		default:		return RS_ERANGE_CHIRP_TIME;
		}
		break;
	case 50:
		switch(lp->chirp_time) {
#if RS_SETUP_MODE_FIFO_IQ_2RX_WIDE_50DB_220US
		case 220:		setup = &mode_fifo_iq_2rx_wide_50db_220us;		break;
#endif
#if RS_SETUP_MODE_FIFO_IQ_2RX_WIDE_50DB_1100US
		case 1100:		setup = &mode_fifo_iq_2rx_wide_50db_1100us;	break;
#endif
#if RS_SETUP_MODE_FIFO_IQ_2RX_WIDE_50DB_4400US
		case 4400:		setup = &mode_fifo_iq_2rx_wide_50db_4400us;	break;
#endif
		default:		return RS_ERANGE_CHIRP_TIME;
		}
		break;
	case 60:
		switch(lp->chirp_time) {
#if RS_SETUP_MODE_FIFO_IQ_2RX_WIDE_60DB_220US
		case 220:		setup = &mode_fifo_iq_2rx_wide_60db_220us;		break;
#endif
#if RS_SETUP_MODE_FIFO_IQ_2RX_WIDE_60DB_1100US
		case 1100:		setup = &mode_fifo_iq_2rx_wide_60db_1100us;	break;
#endif
#if RS_SETUP_MODE_FIFO_IQ_2RX_WIDE_60DB_4400US
		case 4400:		setup = &mode_fifo_iq_2rx_wide_60db_4400us;	break;
#endif
		default:		return RS_ERANGE_CHIRP_TIME;
		}
		break;
	default:
		return RS_ERANGE_RX_GAIN;
	}

	RET_ORG(rs_ret_t, rs_setup_getcode(code, setup), RS_OK);
	code->info.frame_nsec = 2;

	return RS_OK;
}

/**
 * update parameters for fifo detection
 * @callergraph
 */
rs_ret_t fifo_iq_2rx_wide_update_param(rs_handle_t handle, const struct rs_fifo_param *lp)
{
	RET_ORG(rs_ret_t, rs_setup_interval(handle, lp->interval), RS_OK);
	RET_ORG(rs_ret_t, rs_setup_hpf(handle, lp->high_pass_filter), RS_OK);

	return RS_OK;
}
