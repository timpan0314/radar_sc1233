/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_2d_types.h
 */

#ifndef __RS24G_2D_TYPES_H__
#define __RS24G_2D_TYPES_H__

#ifdef __cplusplus
#define RS_IF extern "C"
#else	/* __cplusplus */
#define RS_IF
#endif	/* __cplusplus */

#include <stdint.h>

#include "rs24g_types.h"


// rs_ret_t
#define RS_2D_ERANGE_MODE						((rs_ret_t) (-2048))
#define RS_2D_ERANGE_NTAP_AVE_DISTANCE			((rs_ret_t) (-2049))
#define RS_2D_ERANGE_NTAP_AVE_ANGLE				((rs_ret_t) (-2050))
#define RS_2D_ERANGE_NTAP_AVE_PEAK_LEVEL		((rs_ret_t) (-2051))
#define RS_2D_ERANGE_SORT_ORDER					((rs_ret_t) (-2052))
#define RS_2D_ERANGE_LOWER_ANGLE				((rs_ret_t) (-2053))
#define RS_2D_ERANGE_UPPER_ANGLE				((rs_ret_t) (-2054))
#define RS_2D_ERANGE_ANGLE_THRESHOLD			((rs_ret_t) (-2055))
#define RS_2D_ERANGE_STATIONARY					((rs_ret_t) (-2056))


#endif	/* __RS24G_2D_TYPES_H__ */
