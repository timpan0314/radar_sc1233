/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_code2_local.c
 */

#include <stdlib.h>
#include "rs24g_code2_local.h"
#include "rs_macro.h"

RS_IF rs_ret_t rs_code2_alloc_seq_data(rs_code2_t code, rs_size_t size)
{
	RS_ASSERT(code != NULL, RS_EINV);

	code->seq.data = calloc(size, sizeof(uint8_t));
	RS_ASSERT(code->seq.data != NULL, RS_ENOMEM);

	code->seq.size = size;

	return RS_OK;
}

RS_IF rs_ret_t rs_code2_alloc_reg_addr_and_data(rs_code2_t code, rs_size_t num)
{
	RS_ASSERT(code != NULL, RS_EINV);

	code->reg.addr = calloc(num, sizeof(uint16_t));
	code->reg.data = calloc(num, sizeof(uint32_t));
	RS_ASSERT(code->reg.addr != NULL, RS_ENOMEM);
	RS_ASSERT(code->reg.data != NULL, RS_ENOMEM);

	code->reg.num = num;

	return RS_OK;
}

RS_IF rs_ret_t rs_code2_alloc_fft_data(rs_code2_t code, rs_size_t size)
{
	RS_ASSERT(code != NULL, RS_EINV);

	code->fft.data = calloc(size, sizeof(uint8_t));
	RS_ASSERT(code->fft.data != NULL, RS_ENOMEM);

	code->fft.size = size;

	return RS_OK;
}

RS_IF rs_ret_t rs_code2_free_seq_data(rs_code2_t code)
{
	RS_ASSERT(code != NULL, RS_EINV);

	if (code->seq.data != NULL) free(code->seq.data);

	code->seq.data = NULL;
	code->seq.size = 0;

	return RS_OK;
}

RS_IF rs_ret_t rs_code2_free_reg_addr_and_data(rs_code2_t code)
{
	RS_ASSERT(code != NULL, RS_EINV);

	if (code->reg.addr != NULL) free(code->reg.addr);
	if (code->reg.data != NULL) free(code->reg.data);

	code->reg.addr = NULL;
	code->reg.data = NULL;
	code->reg.num = 0;

	return RS_OK;
}

RS_IF rs_ret_t rs_code2_free_fft_data(rs_code2_t code)
{
	RS_ASSERT(code != NULL, RS_EINV);

	if (code->fft.data != NULL) free(code->fft.data);

	code->fft.data = NULL;
	code->fft.size = 0;

	return RS_OK;
}
