/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs_handle.h
 */

#ifndef __RS_HANDLE_H__
#define __RS_HANDLE_H__

#include <stdint.h>

#include "rs24g_types.h"
#include "rs_ctl_dev.h"
#include "rs24g_mutex.h"
#include "rs24g_ctl_base.h"
#include "rs24g_ctl_code.h"
#include "rs24g_ctl_sensor_ctl_type.h"
#include "rs24g_resource.h"

struct rs_handle
{
	rs_ctl_dev_t dev;

	rs24g_mutex_t mutex;

	rs_state_t state;
	rs_mode_t mode;
	uint8_t peak_level_lower;
	struct rs_code_ref_s code_ref;
	rs_sensor_ctl_type_t sensor_ctl_type;

	struct rs_resource_holder_s res_holder;
};

#endif	/* __RS_HANDLE_H__ */
