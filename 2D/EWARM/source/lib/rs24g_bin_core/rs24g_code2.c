/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_code2.c
 */

#include <stdlib.h>
#include "rs24g_code2_local.h"
#include "rs24g_bin_local.h"
#include "rs_macro.h"
#include "convbin.h"

RS_IF rs_ret_t rs_code2_alloc(rs_code2_t *pcode)
{
	rs_code2_t p;

	RS_ASSERT(pcode != NULL, RS_EINV);

	*pcode = NULL;

	p = malloc(sizeof(struct rs_code2_s));
	RS_ASSERT(p != NULL, RS_ENOMEM);
	p->normal = malloc(sizeof(struct rs_code2_mode_s));
	if(p->normal == NULL) {
		free(p);
		return RS_ENOMEM;
	}
	p->lowpower = malloc(sizeof(struct rs_code2_mode_s));
	if(p->lowpower == NULL) {
		free(p->normal);
		free(p);
		return RS_ENOMEM;
	}

	p->seq.data         = NULL;
	p->seq.size         = 0;

	p->reg.addr         = NULL;
	p->reg.data         = NULL;
	p->reg.num          = 0;

	p->fft.data         = NULL;
	p->fft.size         = 0;

	p->normal->sense_addr.start        = 0;
	p->normal->sense_addr.end          = 0;
	p->normal->cal_addr.start          = 0;
	p->normal->cal_addr.end            = 0;

	p->normal->frame.mode              = 0;
	p->normal->frame.size              = 0;
	p->normal->frame.frame_header      = 0;
	p->normal->frame.status_register   = 0;
	p->normal->frame.data_type         = 0;

	p->normal->presdet.mode            = 0;

	p->lowpower->sense_addr.start      = 0;
	p->lowpower->sense_addr.end        = 0;
	p->lowpower->cal_addr.start        = 0;
	p->lowpower->cal_addr.end          = 0;

	p->lowpower->frame.mode            = 0;
	p->lowpower->frame.size            = 0;
	p->lowpower->frame.frame_header    = 0;
	p->lowpower->frame.status_register = 0;
	p->lowpower->frame.data_type       = 0;

	p->lowpower->presdet.mode          = 0;

	p->norm_lowp = 0;

	p->mode = 0;  //ope_mode

	*pcode = p;

	return RS_OK;
}

RS_IF rs_ret_t rs_code2_free(rs_code2_t *pcode)
{
	rs_code2_t p;

	RS_ASSERT(pcode != NULL, RS_EINV);
	RS_ASSERT(*pcode != NULL, RS_EINV);

	p = *pcode;

	(void)rs_code2_free_seq_data(p);
	(void)rs_code2_free_reg_addr_and_data(p);
	(void)rs_code2_free_fft_data(p);

	free(p->normal);
	free(p->lowpower);
	free(p);

	*pcode = NULL;

	return RS_OK;
}

RS_IF rs_ret_t rs_code2_load_minimum_from_bin(rs_code2_t code, rs_bin_t bin)
{
	struct rs_bin_data_section_s *code_section;
	const void *buf;
	size_t res;
	uint32_t size;

	RS_ASSERT(code != NULL, RS_EINV);
	RS_ASSERT(bin != NULL, RS_EINV);

	RS_CALL(rs_bin_get_code_section(bin, &code_section));
	buf = code_section->data;
	res = code_section->size;

	RS_CALL(rs_code2_free_seq_data(code));
	RS_CALL(rs_code2_free_reg_addr_and_data(code));
	RS_CALL(rs_code2_free_fft_data(code));

	RS_ASSERT(bload32s(&buf, &res, &code->mode) == 0, RS_EINV);

	RS_ASSERT(bload32(&buf, &res, &size) == 0, RS_EINV);
	RS_CALL(rs_code2_alloc_seq_data(code, size));
	RS_ASSERT(bload8a(&buf, &res, code->seq.data, code->seq.size) == 0, RS_EINV);

	RS_ASSERT(bload32(&buf, &res, &size) == 0, RS_EINV);
	RS_CALL(rs_code2_alloc_reg_addr_and_data(code, size));
	RS_ASSERT(bload16a(&buf, &res, code->reg.addr, code->reg.num) == 0, RS_EINV);
	RS_ASSERT(bload32a(&buf, &res, code->reg.data, code->reg.num) == 0, RS_EINV);

	RS_ASSERT(bload32(&buf, &res, &size) == 0, RS_EINV);
	RS_CALL(rs_code2_alloc_fft_data(code, size));
	RS_ASSERT(bload8a(&buf, &res, code->fft.data, code->fft.size) == 0, RS_EINV);

	return RS_OK;
}
