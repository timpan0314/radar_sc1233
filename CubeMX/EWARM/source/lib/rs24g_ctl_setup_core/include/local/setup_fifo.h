/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * setup_fifo.h
 */

#ifndef __SETUP_FIFO_H__
#define __SETUP_FIFO_H__

#include <stdint.h>

#include "rs24g_types.h"
#include "rs24g_ctl_setup_base.h"
#include "rs24g_ctl_setup_fifo.h"

#include "setup_common.h"


#define RS_FIFO_FORMAT_INIT						(RS_FIFO_FORMAT_FFT)
#define RS_FIFO_RX_GAIN_INIT					(60)
#define RS_FIFO_INTERVAL_INIT					(25)
#define RS_FIFO_CHIRP_TIME_INIT					(1100)
#define RS_FIFO_RX_NUM_INIT						(1)
#define RS_FIFO_DIGITAL_BEAM_SHAPER_INIT		(RS_DIGITAL_BEAM_SHAPER_WIDE)
#define RS_FIFO_HPF_INIT						(RS_HPF_FIRST_ORDER)


extern rs_ret_t fifo_init(struct rs_fifo_param *lp);

#endif	/* __SETUP_FIFO_H__ */
