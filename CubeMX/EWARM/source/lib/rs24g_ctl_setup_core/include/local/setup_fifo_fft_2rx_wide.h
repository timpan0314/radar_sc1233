/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * setup_fifo_2rx_wide.h
 */

#ifndef __SETUP_FIFO_2RX_WIDE_H__
#define __SETUP_FIFO_2RX_WIDE_H__

#include <stdint.h>

#include "rs24g_types.h"
#include "rs24g_ctl_setup_base.h"
#include "rs24g_ctl_setup_fifo.h"

#include "setup_common.h"
#include "setup_fifo.h"


extern rs_ret_t fifo_fft_2rx_wide_getcode(const struct rs_fifo_param *lp, rs_code_ref_t code);
extern rs_ret_t fifo_fft_2rx_wide_update_param(rs_handle_t handle, const struct rs_fifo_param *lp);

#endif	/* __SETUP_FIFO_2RX_WIDE_H__ */
