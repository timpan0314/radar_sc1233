/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * location_debug.h
 */

#ifndef __LOCATION_DEBUG_H__
#define __LOCATION_DEBUG_H__

#include <stdint.h>
#include <stdio.h>

#include "rs24g_types.h"
#include "rs24g_2d_sc1233.h"

#include "rs_list.h"
#include "common_2d.h"
#include "location.h"


extern void loc_debug_dump(struct loc_rank *data);

#endif	/* __LOCATION_DEBUG_H__ */
