/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_code2_local.h
 */

#ifndef __RS24G_CODE2_LOCAL_H__
#define __RS24G_CODE2_LOCAL_H__

#include "rs24g_code2.h"
#include "rs24g_ctl_base.h"

/*
 * Type Definition
 */

/*
 * Constant Definition
 */

/*
 * Struct Definition
 */
struct rs_code2_mode_s
{
	struct seq_addr_info
	{
		uint16_t start;
		uint16_t end;
	} sense_addr, cal_addr;
	struct frame_info
	{
		uint32_t mode;
		uint32_t size;
		uint32_t frame_header;
		uint32_t status_register;
		uint32_t data_type;
	} frame;
	struct presdet_info
	{
		uint32_t mode;
	} presdet;
};

struct rs_code2_s
{
	rs_mode_t mode; //ope_mode

	struct seq_info
	{
		uint8_t *data;
		rs_size_t size;
	} seq;
	struct reg_info
	{
		uint16_t *addr;
		uint32_t *data;
		rs_size_t num;
	} reg;
	struct fft_info
	{
		uint8_t *data;
		rs_size_t size;
	} fft;

	struct rs_code2_mode_s *normal;
	struct rs_code2_mode_s *lowpower;

	uint32_t norm_lowp;
};

/*
 * Function Definition
 */
RS_IF rs_ret_t rs_code2_alloc_seq_data(rs_code2_t code, rs_size_t size);
RS_IF rs_ret_t rs_code2_alloc_reg_addr_and_data(rs_code2_t code, rs_size_t num);
RS_IF rs_ret_t rs_code2_alloc_fft_data(rs_code2_t code, rs_size_t size);
RS_IF rs_ret_t rs_code2_free_seq_data(rs_code2_t code);
RS_IF rs_ret_t rs_code2_free_reg_addr_and_data(rs_code2_t code);
RS_IF rs_ret_t rs_code2_free_fft_data(rs_code2_t code);

#endif /* __RS24G_CODE2_LOCAL_H__ */
