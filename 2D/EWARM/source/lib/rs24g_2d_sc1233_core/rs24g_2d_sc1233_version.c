/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_2d_sc1233_version.c
 */

#include <stdint.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "rs24g_ctl_sc1233.h"

#include "rs24g_2d_sc1233.h"
#include "rs24g_2d_sc1233_version.h"


RS_IF rs_ret_t rs_2d_get_lib_ver(uint16_t *major_ver, uint16_t *minor_ver, uint16_t *revision, uint16_t *hotfix)
{
	RET_CHECK(major_ver != NULL, RS_EFAULT);
	RET_CHECK(minor_ver != NULL, RS_EFAULT);
	RET_CHECK(revision != NULL, RS_EFAULT);
	RET_CHECK(hotfix != NULL, RS_EFAULT);

	*major_ver = rs24g_2d_sc1233_version_major;
	*minor_ver = rs24g_2d_sc1233_version_minor;
	*revision = rs24g_2d_sc1233_version_revision;
	*hotfix = rs24g_2d_sc1233_version_hotfix;

	return RS_OK;
}
