/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file chipboot.h
 */


#ifndef __CHIPBOOT_H__
#define __CHIPBOOT_H__

#include "rs24g_types.h"
#include "rs_ctl_dev.h"
#include "rs_ctl_cmd_sc1233.h"


/**
 * boot ("Start-Up") command
 */
#define chipboot(...) rs_ctl_cmd_chipboot_sc1233(__VA_ARGS__)


#endif	/* __CHIPBOOT_H__ */
