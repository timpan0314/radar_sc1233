/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_ctl_setup_fifo.h
 */

#ifndef __RS24G_CTL_SETUP_FIFO_H__
#define __RS24G_CTL_SETUP_FIFO_H__

#include <stdint.h>

#include "rs24g_types.h"
#include "rs24g_ctl_base.h"
#include "rs24g_ctl_fifo.h"

#include "rs24g_ctl_setup_base.h"

/*
 * Type Definition
 */
typedef int32_t									rs_fifo_format_t;

/*
 * Constant Definition
 */
#define RS_FIFO_FORMAT_FFT						((rs_fifo_format_t) (0))
#define RS_FIFO_FORMAT_IQ						((rs_fifo_format_t) (1))

/*
 * Struct Definition
 */
struct rs_fifo_param
{
	rs_fifo_format_t							format;
	uint32_t									rx_gain;
	uint32_t									interval;
	uint32_t									chirp_time;
	uint32_t									rx_num;
	rs_shaper_t									digital_beam_shaper;
	rs_hpf_t									high_pass_filter;
};

/*
 * Function Definition
 */

#endif	/* __RS24G_CTL_SETUP_FIFO_H__ */
