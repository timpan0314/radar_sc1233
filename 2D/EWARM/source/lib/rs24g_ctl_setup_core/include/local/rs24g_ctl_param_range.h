/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_ctl_param_range.h
 */

#ifndef __RS24G_CTL_PARAM_RANGE_H__
#define __RS24G_CTL_PARAM_RANGE_H__


#define RS_INTERVAL_MIN							(8)
#define RS_INTERVAL_MAX							(5000)

#define RS_ALPHA_MIN							(0)
#define RS_ALPHA_MAX							(255)

#define RS_MOTION_THRESHOLD_MIN					(0)
#define RS_MOTION_THRESHOLD_MAX					(4095)

#define RS_STABLE_COUNT_MIN						(0)
#define RS_STABLE_COUNT_MAX						(255)

#define RS_DISTANCE_MIN_MODE_MOTION				(10)
#define RS_DISTANCE_MAX_MODE_MOTION				(2000)
#define RS_DISTANCE_MIN_MODE_DISTANCE			(4)
#define RS_DISTANCE_MAX_MODE_DISTANCE			(5600)

#define RS_DISTANCE_MIN_NORMAL					(10)
#define RS_DISTANCE_MAX_NORMAL					(5600)
#define RS_DISTANCE_MIN_FINE					(10)
#define RS_DISTANCE_MAX_FINE					(2800)
#define RS_DISTANCE_MIN_XFINE					(4)
#define RS_DISTANCE_MAX_XFINE					(1400)

#define RS_BETA_MIN								(0)
#define RS_BETA_MAX								(255)


#endif	/* __RS24G_CTL_PARAM_RANGE_H__ */
