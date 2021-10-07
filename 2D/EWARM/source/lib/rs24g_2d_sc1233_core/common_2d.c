/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * common_2d.c
 */

#define _USE_MATH_DEFINES

#ifndef M_PI
#define M_PI 3.14159265358979323846  //_Tim
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "rs24g_types.h"
#include "rs_macro.h"

#include "rs24g_2d_sc1233.h"
#include "rs24g_2d_sc1233_local.h"
#include "rs_2d_handle_t.h"
#include "common_2d.h"


/* const value */
static const rs_2d_fp_t d = (rs_2d_fp_t) 0.00576;
static const rs_2d_fp_t lambda = (rs_2d_fp_t) (299792458.0 / 24.150e9);


rs_bool_t calc_phase_diff(int16_t rx1r, int16_t rx1i, int16_t rx2r, int16_t rx2i, int32_t *phase_diff_real, int32_t *phase_diff_imag)
{
	/* check invalid input */
	if (((rx1r == 0) && (rx1i == 0)) || ((rx2r == 0) && (rx2i == 0))){
		return RS_FALSE;
	}

	// read m1, m2
	const int16_t m1_r = rx1r;
	const int16_t m1_i = rx1i;
	const int16_t m2_r = rx2r;
	const int16_t m2_i = rx2i;

	const int32_t a_r = m1_r * m2_r - (-m1_i) * m2_i;
	const int32_t a_i = m1_r * m2_i + (-m1_i) * m2_r;

#if 1
	rs24g2d("m1:%d + %d i", m1_r, m1_i);
	rs24g2d("m2:%d + %d i", m2_r, m2_i);
	rs24g2d("a:%d + %d i", a_r, a_i);
#endif

	*phase_diff_real = a_r;
	*phase_diff_imag = a_i;

	return RS_TRUE;
}

rs_bool_t calc_angle(int32_t phase_diff_real, int32_t phase_diff_imag, rs_2d_fp_t *angle)
{
	if (phase_diff_real == 0 && phase_diff_imag == 0){
		return RS_FALSE;
	}

	const rs_2d_fp_t phi = (rs_2d_fp_t)atan2((rs_2d_fp_t)phase_diff_imag, (rs_2d_fp_t)phase_diff_real);
	const rs_2d_fp_t opt = (rs_2d_fp_t)(-lambda / (2.0 * M_PI * d) * phi);

	if (opt < -1.0 || opt > 1.0){
		return RS_FALSE;
	}

	*angle = (rs_2d_fp_t)(asin(opt) * 180.0 / M_PI);

	if (isnan(*angle)) {
		return RS_FALSE;
	}

#if 1
	rs24g2d("phi:%.2f", phi);
	rs24g2d("opt:%.2f", opt);
	rs24g2d("angle:%.2f", *angle);
#endif

	return RS_TRUE;
}

int32_t calc_angle_int(int16_t rx1r, int16_t rx1i, int16_t rx2r, int16_t rx2i)
{
	int32_t angle = RS_2D_ANGLE_INVALID;
	int32_t phase_diff_real = 0;
	int32_t phase_diff_imag = 0;
	rs_2d_fp_t lang = 0.0;

	if (calc_phase_diff(rx1r, rx1i, rx2r, rx2i, &phase_diff_real, &phase_diff_imag) &&
		calc_angle(phase_diff_real, phase_diff_imag, &lang)) {
		angle = (int32_t)lang;
	}

	return angle;
}

rs_ret_t angle_get(rs_2d_handle_t handle, rs_size_t max_rank, struct rs_2d_location_data *data)
{
	struct rs_2d_handle *lh = (struct rs_2d_handle *) handle;
	rs_size_t i;

	RET_CHECK(max_rank <= RS_2D_MAXRANK, RS_EINV);

	data->number = 0;

	for(i=0; i<RS_2D_MAXRANK; i++) {
		data->angle[i] = RS_2D_ANGLE_INVALID;
		data->distance[i] = RS_2D_DISTANCE_INVALID;
		data->peak_level[i] = RS_2D_PEAK_LEVEL_INVALID;
	}

	for(i=0; i<max_rank; i++) {
		if(lh->last_data.distance.distance[i] != 0x7fff) {
			data->angle[i] = calc_angle_int(
				lh->last_data.distance.peak_real[i],
				lh->last_data.distance.peak_imag[i],
				lh->last_data.peak_rx2_real[i],
				lh->last_data.peak_rx2_imag[i]
			);
			data->distance[i] = lh->last_data.distance.distance[i];
			data->peak_level[i] = lh->last_data.distance.peak_level[i];

			data->number = i + 1;
		}
	}

	return RS_OK;
}

