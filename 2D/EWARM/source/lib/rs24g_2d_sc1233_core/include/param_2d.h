/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * param_2d.h
 */

#ifndef __PARAM_2D_H__
#define __PARAM_2D_H__

#include <stdint.h>

#include "rs24g_types.h"
#include "rs24g_2d_sc1233.h"


#define RS_2D_MODE_INIT							(RS_2D_MODE_MOVING_AVERAGE)
#define RS_2D_NTAP_AVE_DISTANCE_INIT			(10)
#define RS_2D_NTAP_AVE_ANGLE_INIT				(10)
#define RS_2D_NTAP_AVE_PEAK_LEVEL_INIT			(10)
#define RS_2D_SORT_ORDER_INIT					(RS_2D_SORT_ORDER_PEAK_LEVEL)
#define RS_2D_LOWER_ANGLE_INIT					(-80)
#define RS_2D_UPPER_ANGLE_INIT					(80)
#define RS_2D_ANGLE_THRESHOLD_INIT				(5000)
#define RS_2D_STATIONARY_INIT					(RS_2D_STATIONARY_OFF)

#define RS_2D_DREF_INIT							(120.0)
#define RS_2D_LA_INIT							(3)
#define RS_2D_TARGET_THRESHOLD_PROB				(0.60)
#define RS_2D_NOISE_THRESHOLD_PROB				(0.25)


#define RS_2D_NTAP_AVE_DISTANCE_MIN				(1)
#define RS_2D_NTAP_AVE_DISTANCE_MAX				(20)

#define RS_2D_NTAP_AVE_ANGLE_MIN				(1)
#define RS_2D_NTAP_AVE_ANGLE_MAX				(20)

#define RS_2D_NTAP_AVE_PEAK_LEVEL_MIN			(1)
#define RS_2D_NTAP_AVE_PEAK_LEVEL_MAX			(20)

#define RS_2D_LOWER_ANGLE_MIN					(-90)
#define RS_2D_LOWER_ANGLE_MAX					(90)

#define RS_2D_UPPER_ANGLE_MIN					(-90)
#define RS_2D_UPPER_ANGLE_MAX					(90)

#define RS_2D_ANGLE_THRESHOLD_MIN				(1)
#define RS_2D_ANGLE_THRESHOLD_MAX				(5000)


#define RS_2D_LOCAL_INTERVAL_MIN				(8)
#define RS_2D_LOCAL_INTERVAL_MAX				(5000)


#endif	/* __PARAM_2D_H__ */
