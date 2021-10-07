/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_ctl_sensor_ctl_type.h
 */

#ifndef __RS24G_CTL_SENSOR_CTL_TYPE_H__
#define __RS24G_CTL_SENSOR_CTL_TYPE_H__

#include <stdint.h>

#include "rs24g_types.h"

#include "rs24g_ctl_base.h"

/*
 * Type Definition
 */
typedef int32_t									rs_sensor_ctl_type_t;

/*
 * Constant Definition
 */
#define RS_SENSOR_CTL_TIMER						((rs_sensor_ctl_type_t) (0))
#define RS_SENSOR_CTL_CPU						((rs_sensor_ctl_type_t) (1))

/*
 * Struct Definition
 */

/*
 * Function Definition
 */
RS_IF rs_ret_t rs_set_sensor_ctl_type(rs_handle_t handle, rs_sensor_ctl_type_t sensor_ctl_type);
RS_IF rs_ret_t rs_get_sensor_ctl_type(rs_handle_t handle, rs_sensor_ctl_type_t *sensor_ctl_type);

#endif	/* __RS24G_CTL_SENSOR_CTL_TYPE_H__ */
