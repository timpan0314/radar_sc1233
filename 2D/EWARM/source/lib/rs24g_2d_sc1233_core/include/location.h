/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * location.h
 */

#ifndef __LOCATION_H__
#define __LOCATION_H__

#include <stdint.h>
#include <stdio.h>

#include "rs24g_types.h"
#include "rs24g_2d_sc1233.h"

#include "rs_list.h"
#include "common_2d.h"


#define MAX_LOC_DEV									(62)
#define MAX_LOC_EST									(10)

#define RS_2D_DISTANCE_MIN						(0)
#define RS_2D_DISTANCE_MAX						(3000)


struct loc_dev
{
	int16_t										distance;
	int32_t										phase_diff_real;
	int32_t										phase_diff_imag;
	uint8_t										peak_level;
	rs_2d_fp_t									angle;
	rs_bool_t									input;
};

struct loc_est
{
	struct {
		struct rs_list_ctl						list;
		struct loc_dev							mem[MAX_LOC_DEV];
		uint8_t									list_mem[RS_LIST_CTL_BUFSIZE(MAX_LOC_DEV)];
	} loc_dev;

	struct
	{
		rs_size_t								all;
		rs_size_t								na_distance;
		rs_size_t								na_peak_level;
		rs_size_t								na_angle;
		rs_size_t								nc_distance;
		rs_size_t								dmax_angle;
	} num;

	struct
	{	
		rs_2d_fp_t								all_input;
		rs_2d_fp_t								na_distance;
		rs_2d_fp_t								na_peak_level;
		rs_2d_fp_t								na_angle;
		rs_2d_fp_t								nc_distance;
		rs_2d_fp_t								dmax_angle;
	} ave;

	rs_bool_t									flag_add_loc_dev;
	uint8_t										rank;
};

struct loc_rank
{
	struct {
		struct rs_list_ctl						list;
		struct loc_est							mem[MAX_LOC_EST];
		uint8_t									list_mem[RS_LIST_CTL_BUFSIZE(MAX_LOC_EST)];
	} loc_est;
};


struct score_eloc
{
	rs_2d_fp_t									prob;
	rs_2d_fp_t									delta_distance;
};

struct noise_floor_point
{
	rs_2d_fp_t									distance;
	rs_2d_fp_t									level;
};

struct noise_floor_table
{
	const struct noise_floor_point				*point;
	rs_size_t									size;
};

extern const struct noise_floor_table nf_220us_hpf1;
extern const struct noise_floor_table nf_220us_hpf2;
extern const struct noise_floor_table nf_1100us_hpf1;
extern const struct noise_floor_table nf_1100us_hpf2;

extern rs_ret_t loc_input(rs_2d_handle_t handle, const struct rs_2rx_peak_data *data);
extern rs_ret_t loc_get(rs_2d_handle_t handle, rs_size_t max_rank, struct rs_2d_location_data *data);

#endif	/* __LOCATION_H__ */
