/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs_2d_handle_t.h
 */

#ifndef __RS_2D_HANDLE_H__
#define __RS_2D_HANDLE_H__

#include <stdint.h>

#include "rs24g_types.h"

#include "rs24g_ctl_setup_sc1233.h"
#include "rs24g_2d_sc1233.h"
#include "common_2d.h"
#include "location.h"


struct rs_2d_handle
{
	struct rs_2rx_param init_param;
	struct rs_2d_algo_param algo_param;

	struct loc_rank data;
	struct rs_2rx_peak_data last_data;

	int state;

	// tentative
	const struct noise_floor_table *noise_floor;
	rs_size_t nc;
	rs_size_t dmax;
	rs_2d_fp_t dref;
	rs_2d_fp_t la;
	rs_2d_fp_t target_threshold_prob;
	rs_2d_fp_t noise_threshold_prob;
};

#endif	/* __RS_2D_HANDLE_H__ */
