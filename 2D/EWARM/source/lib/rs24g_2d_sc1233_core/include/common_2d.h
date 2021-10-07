/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * common_2d.h
 */

#ifndef __COMMON_2D_H__
#define __COMMON_2D_H__

#include <stdint.h>
#include <stdio.h>

#include "rs24g_types.h"
#include "rs24g_2d_sc1233.h"

#include "rs_list.h"


//#define rs24g2d(fmt, ...)					{fprintf(stdout, "[rs24g_2d_sc1233] %s:%d: " fmt "\n", __FUNCTION__, __LINE__, ## __VA_ARGS__); fflush(stdout);}
#define rs24g2d(...)


#define RS_2D_FP_DOUBLE

#define RS_2D_STATE_INVALID						(0)
#define RS_2D_STATE_INIT						(1)
#define RS_2D_STATE_EXECUTABLE					(2)


#define RS_MAX(a,b)								((a) > (b) ? (a) : (b))
#define RS_MIN(a,b)								((a) < (b) ? (a) : (b))

#define RS_2D_ARRAY_SIZE(a)						(sizeof(a) / sizeof((a)[0]))

#ifndef RS_2D_FP_DOUBLE
typedef float									rs_2d_fp_t;
#else /* RS_2D_FP_DOUBLE */
typedef double									rs_2d_fp_t;
#endif /* RS_2D_FP_DOUBLE */

#define RS_2D_MAXRANK							(5)


extern rs_bool_t calc_angle(int32_t phase_diff_real, int32_t phase_diff_imag, rs_2d_fp_t *angle);
extern rs_bool_t calc_phase_diff(int16_t rx1r, int16_t rx1i, int16_t rx2r, int16_t rx2i, int32_t *phase_diff_real, int32_t *phase_diff_imag);

extern rs_ret_t angle_get(rs_2d_handle_t handle, rs_size_t max_rank, struct rs_2d_location_data *data);

#endif	/* __COMMON_2D_H__ */
