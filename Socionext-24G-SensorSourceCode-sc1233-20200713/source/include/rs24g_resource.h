/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_resource.h
 */

#ifndef __RS24G_RESOURCE_H__
#define __RS24G_RESOURCE_H__

#include "rs24g_types.h"
#include "rs24g_bin_interface.h"
#include "rs_macro.h"

/*
 * Type Definition
 */
typedef struct rs_resource_holder_s *rs_resource_holder_t;

/*
 * Constant Definition
 */

/*
 * Struct Definition
 */
struct rs_resource_holder_s
{
	rs_code2_t code;
};

/*
 * Function Definition
 */
static __inline rs_ret_t rs_resource_alloc_holder(rs_resource_holder_t holder)
{
	rs_ret_t ret = RS_OK;
#if defined(RS_RESOURCE_CONSTANT)
#else
	RS_ASSERT(holder != NULL, RS_EINV);
	RS_ASSERT(holder->code == NULL, RS_EFAULT);
	{
		struct rs_bin_interface_s binif;
		ret = rs_bin_interface_get(&binif);
		if (ret == RS_OK) {
			ret = binif.rs_code2_alloc(&holder->code);
			if (ret == RS_ENOSUPPORT) {
				/* ignore */
				holder->code = NULL;
				ret = RS_OK;
			}
		} else {
			/* ignore */
			ret = RS_OK;
		}
	}
#endif
	return ret;
}

static __inline rs_ret_t rs_resource_free_holder(rs_resource_holder_t holder)
{
#if defined(RS_RESOURCE_CONSTANT)
#else
	RS_ASSERT(holder != NULL, RS_EINV);
	if (holder->code != NULL)
	{
		struct rs_bin_interface_s binif;
		RS_CALL(rs_bin_interface_get(&binif));
		RS_CALL(binif.rs_code2_free(&holder->code));
	}
#endif
	return RS_OK;
}

#endif	/* __RS24G_RESOURCE_H__ */
