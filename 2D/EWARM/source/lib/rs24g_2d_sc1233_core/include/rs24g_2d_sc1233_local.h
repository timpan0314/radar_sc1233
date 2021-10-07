/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_2d_sc1233_local.h
 */

#ifndef __RS24G_2D_SC1233_LOCAL_H__
#define __RS24G_2D_SC1233_LOCAL_H__

#include <stdint.h>

#include "rs24g_types.h"
#include "rs24g_ctl_sc1233.h"
#include "rs24g_2d_sc1233.h"


/*
 * Type Definition
 */
typedef int32_t									rs_2d_param_t;

/*
 * Constant Definition
 */
#define RS_2D_PARAM_MODE						((rs_2d_param_t) (1))
#define RS_2D_PARAM_NTAP_AVE_DISTANCE			((rs_2d_param_t) (2))
#define RS_2D_PARAM_NTAP_AVE_ANGLE				((rs_2d_param_t) (3))
#define RS_2D_PARAM_NTAP_AVE_PEAK_LEVEL			((rs_2d_param_t) (4))
#define RS_2D_PARAM_SORT_ORDER					((rs_2d_param_t) (5))
#define RS_2D_PARAM_LOWER_ANGLE					((rs_2d_param_t) (6))
#define RS_2D_PARAM_UPPER_ANGLE					((rs_2d_param_t) (7))
#define RS_2D_PARAM_ANGLE_THRESHOLD				((rs_2d_param_t) (8))
#define RS_2D_PARAM_STATIONARY					((rs_2d_param_t) (9))

#define RS_2D_PARAM_LOCAL_RX_GAIN				((rs_2d_param_t) (101))
#define RS_2D_PARAM_LOCAL_INTERVAL				((rs_2d_param_t) (102))
#define RS_2D_PARAM_LOCAL_CHIRP_TIME			((rs_2d_param_t) (103))
#define RS_2D_PARAM_LOCAL_HPF					((rs_2d_param_t) (104))

#define RS_2D_ANGLE_INVALID						((int32_t) (0x7fffffff))
#define RS_2D_DISTANCE_INVALID					((int32_t) (0x7fffffff))
#define RS_2D_PEAK_LEVEL_INVALID				((uint32_t) (0xffffffff))

/*
 * Struct Definition
 */
struct rs_2d_param_type_range
{
	int32_t										min;
	int32_t										max;
};

struct rs_2d_param_type_choice
{
	const int32_t *								choice;
	rs_size_t									num;
};

struct rs_2d_local_data
{
	int32_t										angle[5];
};

struct rs_2d_internal_data
{
	rs_size_t									order_size;

	struct
	{
		rs_size_t								pos_size;

		struct
		{
			int16_t								distance;
			uint8_t								level;
			double								angle;
			uint8_t								prob;
		} pos[62];

		double									distance;
		double									angle;
		double									level;
		double									prob;
	} order[10];
};

/*
 * Function Definition
 */
RS_IF rs_ret_t rs_2d_param_get_range(rs_2d_param_t param, struct rs_2d_param_type_range *data);
RS_IF rs_ret_t rs_2d_param_get_choice(rs_2d_param_t param, struct rs_2d_param_type_choice *data);
RS_IF rs_ret_t rs_2d_param_check(rs_2d_param_t param, int32_t val);

RS_IF rs_ret_t rs_2d_get_location_all(rs_2d_handle_t handle, struct rs_2d_location_data *data);
RS_IF rs_ret_t rs_2d_get_internal_data(rs_2d_handle_t handle, struct rs_2d_internal_data *data);

#endif	/* __RS24G_2D_SC1233_LOCAL_H__ */
