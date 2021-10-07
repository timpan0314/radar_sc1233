/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_2d_sc1233.h
 */

#ifndef __RS24G_2D_SC1233_H__
#define __RS24G_2D_SC1233_H__

#include <stdint.h>

#include "rs24g_types.h"
#include "rs24g_2d_types.h"
#include "rs24g_ctl_sc1233.h"
#include "rs24g_ctl_setup_sc1233.h"


/*
 * Type Definition
 */
typedef void *									rs_2d_handle_t;
typedef int32_t									rs_2d_mode_t;
typedef int32_t									rs_2d_sort_order_t;
typedef int32_t									rs_2d_stationary_t;

/*
 * Constant Definition
 */
#define RS_2D_MODE_RAW_ANGLE					((rs_2d_mode_t) (0))
#define RS_2D_MODE_MOVING_AVERAGE				((rs_2d_mode_t) (1))

#define RS_2D_SORT_ORDER_PEAK_LEVEL				((rs_2d_sort_order_t) (0))
#define RS_2D_SORT_ORDER_DISTANCE				((rs_2d_sort_order_t) (1))

#define RS_2D_STATIONARY_OFF					((rs_2d_stationary_t) (0))
#define RS_2D_STATIONARY_ON						((rs_2d_stationary_t) (1))

/*
 * Struct Definition
 */
struct rs_2d_location_data
{
	uint32_t									number;
	int32_t										angle[5];
	int32_t										distance[5];
	uint32_t									peak_level[5];
};

struct rs_2d_algo_param
{
	rs_2d_mode_t								mode;
	uint8_t										ntap_ave_distance;
	uint8_t										ntap_ave_angle;
	uint8_t										ntap_ave_peak_level;
	rs_2d_sort_order_t							sort_order;
	int32_t										lower_angle;
	int32_t										upper_angle;
	uint32_t									angle_threshold;
	rs_2d_stationary_t							stationary;
};

/*
 * Function Definition
 */
RS_IF rs_ret_t rs_2d_init(rs_2d_handle_t *handle, const struct rs_2rx_param *param);
RS_IF rs_ret_t rs_2d_deinit(rs_2d_handle_t handle);
RS_IF rs_ret_t rs_2d_get_init_algo_param(struct rs_2d_algo_param *param);
RS_IF rs_ret_t rs_2d_set_algo_param(rs_2d_handle_t handle, const struct rs_2d_algo_param *param);
RS_IF rs_ret_t rs_2d_exec(rs_2d_handle_t handle, const struct rs_2rx_peak_data *data);
RS_IF rs_ret_t rs_2d_get_location(rs_2d_handle_t handle, struct rs_2d_location_data *data);
RS_IF rs_ret_t rs_2d_get_lib_ver(uint16_t *major_ver, uint16_t *minor_ver, uint16_t *revision, uint16_t *hotfix);


#endif	/* __RS24G_2D_SC1233_H__ */
