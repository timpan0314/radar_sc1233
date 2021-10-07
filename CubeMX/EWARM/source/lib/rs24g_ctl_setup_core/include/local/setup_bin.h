/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * setup_bin.h
 */

#ifndef __SETUP_BIN_H__
#define __SETUP_BIN_H__

#include "rs24g_ctl_setup_bin.h"
#include "rs24g_bin.h"
#include "rs24g_ctl_code.h"
#include "rs24g_resource.h"

extern rs_ret_t bin_getcode(const struct rs_bin_s *lp, uint16_t hw_type, rs_code_ref_t code, rs_resource_holder_t res);
extern rs_ret_t bin_getmode(rs_resource_holder_t res, rs_mode_t *p_mode);

#endif	/* __SETUP_BIN_H__ */
