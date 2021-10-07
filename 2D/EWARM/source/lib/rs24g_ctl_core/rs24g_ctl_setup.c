/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file rs24g_ctl_setup.c
 */

#include <stdint.h>
#include <stdio.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "rs_sleep.h"
#include "defs/rs_dev_addr.h"
#include "rs_ctl_cmd.h"
#include "rs_ctl_data.h"
#include "rs_ctl_op.h"

#include "rs24g_ctl_base.h"
#include "rs24g_ctl_sensor_ctl_type.h"
#include "rs24g_ctl_local.h"
#include "rs_handle.h"
#include "chipboot.h"


RS_IF rs_ret_t rs_get_setup_code_handle(rs_handle_t handle, rs_code_ref_t *code_handle)
{
	struct rs_handle *lh = (struct rs_handle *) handle;

	RS_ASSERT(handle != NULL, RS_EINV);
	RS_ASSERT(code_handle != NULL, RS_EFAULT);

	*code_handle = &(lh->code_ref);
	return RS_OK;
}

RS_IF rs_ret_t rs_get_setup_res_handle(rs_handle_t handle, rs_resource_holder_t *res_handle)
{
	struct rs_handle *lh = (struct rs_handle *) handle;

	RS_ASSERT(handle != NULL, RS_EINV);
	RS_ASSERT(res_handle != NULL, RS_EFAULT);

	*res_handle = &lh->res_holder;
	return RS_OK;
}

static uint16_t get_expected_crc(uint32_t addr, const uint8_t *data, rs_size_t size)
{
	uint16_t expected = 0xFFFF;
	uint8_t addr16[2]; 

	addr16[0] = (addr >> 8) & 0xff;
	addr16[1] = (addr >> 0) & 0xff;
	expected = rs_calc_crc16(expected, addr16, 2);
	expected = rs_calc_crc16(expected, data, size);

	return expected;
}

/**
 * Setting registers, Sequencer Code and FFT parameters \cite WriteFFTPrc, \cite WriteSEQPrc, \cite ActivateSEQ
 *
 * @callgraph
 */
static rs_ret_t rs_setup_seq(rs_handle_t handle)
{
	struct rs_handle *lh = (struct rs_handle *) handle;
	uint16_t expected;
	uint32_t crc;
	uint16_t crc_seqpgm;
	uint16_t crc_fftwin;
#if defined(RS_WRITE_MEM_LIMIT)
	rs_size_t size;
	uint32_t addr;
	const uint8_t *data;
#endif

	/* register parameters of code */
	{
		size_t i;

		for (i=0; i<lh->code_ref.regs.num; i++) {
			RS_CALL(rs_ctl_write_reg(lh->dev, lh->code_ref.regs.addr[i], lh->code_ref.regs.data[i]));
		}
	}
	/* wait until stabilized, as the setting for crystal may be changed at this register update */
	rs_usleep(5 * 1000);
	RS_CALL(rs_ctl_cmd_disable_seq(lh->dev));
	/* write Sequencer code */
#if defined(RS_WRITE_MEM_LIMIT)
	size = lh->code_ref.seqc.size;
	addr = RS_CTL_TOPADDR_SEQ;
	data = (const uint8_t *)(lh->code_ref.seqc.data);
	while (size > 0)
	{
		rs_size_t unitsize = size < RS_WRITE_MEM_LIMIT ? size : RS_WRITE_MEM_LIMIT;
		RS_CALL(rs_ctl_write_mem(lh->dev, addr, data, unitsize));
		/* check CRC */
		expected = get_expected_crc(addr, data, unitsize);
		RS_CALL(rs_ctl_read_reg(lh->dev, RS_CTL_REG_CPUIF_MON_2, &crc));
		crc_seqpgm = (crc >> 16) & 0xFFFF;
		RS_ASSERT(crc_seqpgm == expected, RS_ELOAD);
		/* increment */
		addr += unitsize / 16;
		data += unitsize;
		size -= unitsize;
	}
#else
	RS_CALL(rs_ctl_write_mem(lh->dev, RS_CTL_TOPADDR_SEQ, (const uint8_t *)(lh->code_ref.seqc.data), lh->code_ref.seqc.size));
	/* check CRC of Sequencer code */
	expected = get_expected_crc(RS_CTL_TOPADDR_SEQ, (const uint8_t *)(lh->code_ref.seqc.data), lh->code_ref.seqc.size);
	RS_CALL(rs_ctl_read_reg(lh->dev, RS_CTL_REG_CPUIF_MON_2, &crc));
	crc_seqpgm = (crc >> 16) & 0xFFFF;
	RS_ASSERT(crc_seqpgm == expected, RS_ELOAD);
#endif
	/* write Window function */
#if defined(RS_WRITE_MEM_LIMIT)
	size = lh->code_ref.winf.size;
	addr = RS_CTL_TOPADDR_FFT;
	data = (const uint8_t *)(lh->code_ref.winf.data);
	while (size > 0)
	{
		rs_size_t unitsize = size < RS_WRITE_MEM_LIMIT ? size : RS_WRITE_MEM_LIMIT;
		RS_CALL(rs_ctl_write_mem(lh->dev, addr, data, unitsize));
		/* check CRC */
		expected = get_expected_crc(addr, data, unitsize);
		RS_CALL(rs_ctl_read_reg(lh->dev, RS_CTL_REG_CPUIF_MON_2, &crc));
		crc_fftwin = (crc >>  0) & 0xFFFF;
		RS_ASSERT(crc_fftwin == expected, RS_ELOAD);
		/* increment */
		addr += unitsize / 2;
		data += unitsize;
		size -= unitsize;
	}
#else
	RS_CALL(rs_ctl_write_mem(lh->dev, RS_CTL_TOPADDR_FFT, (const uint8_t *)(lh->code_ref.winf.data), lh->code_ref.winf.size));
	/* check CRC of Window function */
	expected = get_expected_crc(RS_CTL_TOPADDR_FFT, (const uint8_t *)(lh->code_ref.winf.data), lh->code_ref.winf.size);
	RS_CALL(rs_ctl_read_reg(lh->dev, RS_CTL_REG_CPUIF_MON_2, &crc));
	crc_fftwin = (crc >>  0) & 0xFFFF;
	RS_ASSERT(crc_fftwin == expected, RS_ELOAD);
#endif
	RS_CALL(rs_ctl_cmd_enable_seq(lh->dev));

	return RS_OK;
}

/**
 * Loading Sequencer Code
 */
RS_IF rs_ret_t rs_load_seqcode(rs_handle_t handle, rs_mode_t mode)
{
	struct rs_handle *lh = (struct rs_handle *) handle;

	rs_ret_t ret = RS_OK;

	RS_ASSERT(handle != NULL, RS_EINV);

	ret = rs24g_mutex_lock(lh->mutex);

	if(ret == RS_OK) {
		if(lh->state != RS_STATE_SHUTDOWN) {
			ret = RS_ESTATE;
		}

		if(ret == RS_OK) {
			ret = chipboot(lh->dev);
		}

		if(ret == RS_OK) {
			ret = rs_setup_seq(handle);
		}

		if(ret == RS_OK) {
			lh->state = RS_STATE_READY;
			lh->mode = mode;
			lh->sensor_ctl_type = RS_SENSOR_CTL_TIMER;
		}

		if(ret == RS_OK) {
			ret = rs24g_mutex_unlock(lh->mutex);
		} else {
			rs24g_mutex_unlock(lh->mutex);
		}
	}

	return ret;
}

RS_IF rs_ret_t rs_write_reg(rs_handle_t handle, uint16_t addr, const uint32_t *val, rs_size_t num)
{
	struct rs_handle *lh = (struct rs_handle *) handle;

	rs_ret_t ret = RS_OK;

	RET_CHECK(handle != NULL, RS_EINV);
	RET_CHECK(val != NULL, RS_EFAULT);
	RET_CHECK(num != 0, RS_EINV);

	ret = rs24g_mutex_lock(lh->mutex);

	if(ret == RS_OK) {
		if(lh->state == RS_STATE_SHUTDOWN) {
			ret = RS_ESTATE;
		}

		if(ret == RS_OK) {
			ret = rs_ctl_write_regs(lh->dev, addr, val, num);
		}

		if(ret == RS_OK) {
			ret = rs24g_mutex_unlock(lh->mutex);
		} else {
			rs24g_mutex_unlock(lh->mutex);
		}
	}

	return ret;
}

RS_IF rs_ret_t rs_read_reg(rs_handle_t handle, uint16_t addr, uint32_t *val, rs_size_t num)
{
	struct rs_handle *lh = (struct rs_handle *) handle;

	rs_ret_t ret = RS_OK;

	RET_CHECK(handle != NULL, RS_EINV);
	RET_CHECK(val != NULL, RS_EFAULT);
	RET_CHECK(num != 0, RS_EINV);

	ret = rs24g_mutex_lock(lh->mutex);

	if(ret == RS_OK) {
		if(lh->state == RS_STATE_SHUTDOWN) {
			ret = RS_ESTATE;
		}

		if(ret == RS_OK) {
			ret = rs_ctl_read_regs(lh->dev, addr, val, num);
		}

		if(ret == RS_OK) {
			ret = rs24g_mutex_unlock(lh->mutex);
		} else {
			rs24g_mutex_unlock(lh->mutex);
		}
	}

	return ret;
}

RS_IF rs_ret_t rs_op_DSLEEP(rs_handle_t handle) 
{
	struct rs_handle *lh = (struct rs_handle *) handle;

	rs_ret_t ret = RS_OK;

	RET_CHECK(handle != NULL, RS_EINV);

	ret = rs24g_mutex_lock(lh->mutex);

	if(ret == RS_OK) {
		ret = rs_ctl_op_DSLEEP(lh->dev);

		if(ret == RS_OK) {
			ret = rs24g_mutex_unlock(lh->mutex);
		} else {
			rs24g_mutex_unlock(lh->mutex);
		}
	}

	return ret;
}

RS_IF rs_ret_t rs_update_param_in_ready_prep(rs_handle_t handle, rs_mode_t mode) 
{
	struct rs_handle *lh = (struct rs_handle *) handle;

	rs_ret_t ret = RS_OK;

	RET_CHECK(handle != NULL, RS_EINV);

	ret = rs24g_mutex_lock(lh->mutex);

	if(ret == RS_OK) {
		if(lh->state != RS_STATE_READY) {
			ret = RS_ESTATE;
		}

		if(ret == RS_OK) {
			if(lh->mode != mode) {
				ret = RS_EMODE;
			}
		}

		if(ret == RS_OK) {
			ret = rs_ctl_op_SRST(lh->dev, 1);
			rs_usleep(5 * 1000);
		}

		if(ret == RS_OK) {
			ret = rs24g_mutex_unlock(lh->mutex);
		} else {
			rs24g_mutex_unlock(lh->mutex);
		}
	}

	return ret;
}
