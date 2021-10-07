/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file setup_fifo.c
 */

#include <stdint.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "defs/rs_dev_addr.h"
#include "rs24g_ctl_setup_base.h"
#include "rs24g_ctl_setup_fifo.h"
#include "setup_common.h"
#include "setup_fifo.h"
#include "rs24g_ctl_local.h"


rs_ret_t fifo_init(struct rs_fifo_param *lp)
{
	lp->format = RS_FIFO_FORMAT_INIT;
	lp->rx_gain = RS_FIFO_RX_GAIN_INIT;
	lp->interval = RS_FIFO_INTERVAL_INIT;
	lp->chirp_time = RS_FIFO_CHIRP_TIME_INIT;
	lp->rx_num = RS_FIFO_RX_NUM_INIT;
	lp->digital_beam_shaper = RS_FIFO_DIGITAL_BEAM_SHAPER_INIT;
	lp->high_pass_filter = RS_FIFO_HPF_INIT;

	return RS_OK;
}
