/*
 * Copyright (c) 2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_ctl_code.h
 */

#ifndef __RS24G_CTL_CODE_H__
#define __RS24G_CTL_CODE_H__

#include "rs24g_types.h"

/*
 * Type Definition
 */
typedef struct rs_code_ref_s *rs_code_ref_t;

/*
 * Constant Definition
 */

/*
 * Struct Definition
 */
struct rs_code_ref_mem_s
{
	const uint8_t *data;
	rs_size_t size;
};

struct rs_code_ref_regs_s
{
	const uint16_t *addr;
	const uint32_t *data;
	rs_size_t num;
};

struct rs_code_ref_info_s
{
	uint8_t frame_header;
	uint8_t frame_status;
	uint8_t frame_datasel;
	uint8_t frame_nsec;
	uint32_t frame_size;
};

struct rs_code_ref_s
{
	struct rs_code_ref_mem_s seqc;
	struct rs_code_ref_mem_s winf;
	struct rs_code_ref_regs_s regs;
	struct rs_code_ref_info_s info;
};

/*
 * Function Definition
 */

#endif	/* __RS24G_CTL_CODE_H__ */
