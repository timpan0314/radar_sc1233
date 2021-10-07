/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file setup_common.c
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "defs/rs_dev_addr.h"
#include "rs24g_ctl_setup_base.h"
#include "rs24g_initial_dataset.h"
#include "setup_common.h"
#include "rs24g_ctl_local.h"
#include "rs24g_code2_local.h"

rs_ret_t read_modify_write_reg(rs_handle_t handle, uint16_t addr, uint32_t mask, uint32_t data)
{
	uint32_t rdata;
	RET_ORG(rs_ret_t, rs_read_reg(handle, addr, &rdata, 1), RS_OK);
	rdata = (rdata & (~mask)) | (data & mask);
	RET_ORG(rs_ret_t, rs_write_reg(handle, addr, &rdata, 1), RS_OK);

	return RS_OK;
}

rs_ret_t table_read_reg(rs_code_ref_t code, uint16_t addr, uint32_t *data)
{
	rs_size_t i;
	for(i=0; i<code->regs.num; i++) {
		if(code->regs.addr[i] == addr) {
			*data = code->regs.data[i];
			rs24gsetup_dp("addr:%02x data:%08x", addr, *data);

			return RS_OK;
		}
	}

	return RS_EFAULT;
}

static rs_ret_t rs_setup_update_code_after_getcode(rs_code_ref_t code)
{
	{
		uint32_t FRM_2;
		RS_CALL(table_read_reg(code, RS_CTL_REG_FRM_2, &FRM_2));
		code->info.frame_size = (FRM_2 >> 16) & 0x1fff;
	}
	{
		uint32_t FRM_3;
		RS_CALL(table_read_reg(code, RS_CTL_REG_FRM_3, &FRM_3));
		{
			uint32_t FRM_HEAD_2 = (FRM_3 >> 12) & 0x1;
			uint32_t CPUIF_DATA_SEL_FIFO = (FRM_3 >> 29) & 0x3;
			uint32_t CPUIF_EN_STATUS_FIFO = (FRM_3 >> 28) & 0x1;
			code->info.frame_header = FRM_HEAD_2;
			code->info.frame_status = CPUIF_EN_STATUS_FIFO;
			code->info.frame_datasel = CPUIF_DATA_SEL_FIFO;
		}
	}
	return RS_OK;
}

rs_ret_t rs_setup_getcode(rs_code_ref_t code, const struct initial_dataset *setup)
{
	RS_ASSERT(code != NULL, RS_EINV);
	RS_ASSERT(setup != NULL, RS_EFAULT);

	if (setup->timer_operation == 0) return RS_EFAULT;

	code->seqc.data = setup->seq;
	code->seqc.size = setup->seq_size;

	code->winf.data = setup->fftwin;
	code->winf.size = setup->fftwin_size;

	code->regs.addr = setup->reg_addr;
	code->regs.data = setup->reg_val;
	code->regs.num = setup->reg_size;

	RS_CALL(rs_setup_update_code_after_getcode(code));

	return RS_OK;
}

rs_ret_t rs_setup_getcode_ext(rs_code_ref_t code, const struct rs_code2_s *setup)
{
	RS_ASSERT(code != NULL, RS_EINV);
	RS_ASSERT(setup != NULL, RS_EFAULT);

	code->seqc.data = setup->seq.data;
	code->seqc.size = setup->seq.size;

	code->winf.data = setup->fft.data;
	code->winf.size = setup->fft.size;

	code->regs.addr = setup->reg.addr;
	code->regs.data = setup->reg.data;
	code->regs.num = setup->reg.num;

	RS_CALL(rs_setup_update_code_after_getcode(code));

	return RS_OK;
}

rs_ret_t rs_setup_getmode_ext(rs_mode_t *p_mode, const struct rs_code2_s *setup)
{
	RS_ASSERT(p_mode != NULL, RS_EFAULT);
	RS_ASSERT(setup != NULL, RS_EINV);

	*p_mode = setup->mode;

	return RS_OK;
}

double effective_chirp_bandwidth(const uint32_t fft_point) {
	return 180 * 1000000.0 * fft_point / 550;
}

double point_per_chirp(rs_handle_t handle, const uint32_t fft_point) {
	uint32_t val;
	const double c = RS_C;
	RET_ORG(rs_ret_t, rs_read_reg(handle, RS_CTL_REG_PRESDET_56, &val, 1), RS_OK);
	const double a_reg = (((val ^ 0xffffffff) + 1) & 0x000fffff) / 256.0;
	return round(a_reg * 2 * effective_chirp_bandwidth(fft_point) * fft_point / c);
}

rs_ret_t rs_setup_range(rs_handle_t handle, uint16_t addr, uint32_t upper, uint32_t lower, int32_t conv_offset)
{
	const uint32_t fft_point = 512;
	const double a = RS_C * point_per_chirp(handle, fft_point) / (2 * effective_chirp_bandwidth(fft_point) * fft_point);

	rs24gsetup_dp("conv_offset:%d", conv_offset);
	rs24gsetup_dp("upper:%d", upper);
	rs24gsetup_dp("lower:%d", lower);

	uint32_t up = (uint32_t) ceil(512 / 2 - (upper - conv_offset) / a);
	uint32_t lo = (uint32_t) floor(512 / 2 - (lower - conv_offset) / a);
	rs24gsetup_dp("up:%d lo:%d", up, lo);

	{
		uint32_t range[16];
		uint32_t i;
		for(i=0; i<sizeof(range)/sizeof(*range); i++) {
			range[i] = 0;
		}
		for(i=0; i<512; i++) {
			if((lo >= i) && (i >= up)) {
				range[15 - i / 32] |= 0x00000001 << (i % 32);
			}
		}
		for(i=0; i<sizeof(range)/sizeof(*range); i++) {
			RET_ORG(rs_ret_t, read_modify_write_reg(handle, addr + i, 0xffffffff, range[i]), RS_OK);
		}
	}

	return RS_OK;
}

/**
 * Update interval register
 */
rs_ret_t rs_setup_interval(rs_handle_t handle, uint32_t interval)
{
	RET_ORG(rs_ret_t, read_modify_write_reg(handle, RS_CTL_REG_CPUIF_4, 0xffffffff, (interval * 0x40) << 0), RS_OK);

	return RS_OK;
}

/**
 * Update HPF register
 */
rs_ret_t rs_setup_hpf(rs_handle_t handle, rs_hpf_t hpf)
{
	switch(hpf) {
	case RS_HPF_FIRST_ORDER:
		RET_ORG(rs_ret_t, read_modify_write_reg(handle, RS_CTL_REG_IF1_1, 0xffffffff, 0x33443301), RS_OK);
		RET_ORG(rs_ret_t, read_modify_write_reg(handle, RS_CTL_REG_IF1_2, 0xffffffff, 0x00031900), RS_OK);
		RET_ORG(rs_ret_t, read_modify_write_reg(handle, RS_CTL_REG_IF2_1, 0xffffffff, 0x33443301), RS_OK);
		RET_ORG(rs_ret_t, read_modify_write_reg(handle, RS_CTL_REG_IF2_2, 0xffffffff, 0x00031900), RS_OK);
		RET_ORG(rs_ret_t, read_modify_write_reg(handle, RS_CTL_REG_SELSET_C_3, 0xffffffff, 0xe0380820), RS_OK);
		break;
	case RS_HPF_SECOND_ORDER:
		RET_ORG(rs_ret_t, read_modify_write_reg(handle, RS_CTL_REG_IF1_1, 0xffffffff, 0x33443300), RS_OK);
		RET_ORG(rs_ret_t, read_modify_write_reg(handle, RS_CTL_REG_IF1_2, 0xffffffff, 0x00031300), RS_OK);
		RET_ORG(rs_ret_t, read_modify_write_reg(handle, RS_CTL_REG_IF2_1, 0xffffffff, 0x33443300), RS_OK);
		RET_ORG(rs_ret_t, read_modify_write_reg(handle, RS_CTL_REG_IF2_2, 0xffffffff, 0x00031300), RS_OK);
		RET_ORG(rs_ret_t, read_modify_write_reg(handle, RS_CTL_REG_SELSET_C_3, 0xffffffff, 0x00000820), RS_OK);
		break;
	default:
		return RS_ERANGE_HPF;
	}

	return RS_OK;
}
