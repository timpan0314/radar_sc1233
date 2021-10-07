/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file setup_bin.c
 */

#include "setup_bin.h"
#include "setup_common.h"
#include "rs24g_bin_interface.h"

/**
 * prepare parameters
 * @callergraph
 */
rs_ret_t bin_getcode(const struct rs_bin_s *lp, uint16_t hw_type, rs_code_ref_t code, rs_resource_holder_t res)
{
	RS_ASSERT(lp != NULL, RS_EINV);
	RS_ASSERT(code != NULL, RS_EINV);
	RS_ASSERT(res != NULL, RS_EINV);
	RS_ASSERT(lp->header.hw_type == hw_type, RS_EFORM);

	{
		struct rs_bin_interface_s binif;
		RS_CALL(rs_bin_interface_get(&binif));
		RS_CALL(binif.rs_code2_load_minimum_from_bin(res->code, (rs_bin_t)lp));
	}
	RS_CALL(rs_setup_getcode_ext(code, res->code));

	return RS_OK;
}

/**
 * get mode for control
 * @callergraph
 */
rs_ret_t bin_getmode(rs_resource_holder_t res, rs_mode_t *p_mode)
{
	RS_ASSERT(res != NULL, RS_EINV);

	RS_CALL(rs_setup_getmode_ext(p_mode, res->code));

	return RS_OK;
}
