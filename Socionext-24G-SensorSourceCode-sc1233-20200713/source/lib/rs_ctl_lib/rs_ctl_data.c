/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file rs_ctl_data.c
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "rs_ctl_dev.h"
#include "rs_ctl_op.h"
#include "rs_ctl_data.h"

#define CRC16_POLY								(0x1021)

/**
 * calculate CRC-16
 */
RS_IF uint16_t rs_calc_crc16(uint16_t crc16, const uint8_t *data, rs_size_t size)
{
	rs_size_t i;
	int j;

	for (i=0; i<size; i++) {
		for (j=7; j>=0; j--) {
			if(((data[i] >> j) & 0x01) ^ ((crc16 >> 15) & 0x0001)) {
				crc16 = (crc16 << 1) ^ CRC16_POLY;
			} else {
				crc16 = (crc16 << 1);
			}
		}
	}

	return crc16;
}

static void uint32_to_uint8(uint32_t src, uint8_t dst[4])
{
	dst[0] = (src >> 24) & 0xff;
	dst[1] = (src >> 16) & 0xff;
	dst[2] = (src >>  8) & 0xff;
	dst[3] = (src >>  0) & 0xff;
}

static void uint8_to_uint32(uint8_t src[4], uint32_t *dst)
{
	uint32_t t = 0;
	int i;
	for (i = 0; i < 4; ++i)
	{
		t <<= 8;
		t += src[i];
	}
	*dst = t;
}

/**
 * write Sensor Memory (Sequencer Code or FFT Window Function)
 *
 * @callgraph
 */
RS_IF rs_ret_t rs_ctl_write_mem(rs_ctl_dev_t dev, uint32_t addr, const uint8_t *data, rs_size_t size)
{
	return rs_ctl_op_WRITE(dev, addr, data, size);
}

/**
 * read Sensor Memory (FIFO memory)
 *
 * @callgraph
 */
RS_IF rs_ret_t rs_ctl_read_mem(rs_ctl_dev_t dev, uint32_t addr, uint8_t *data, rs_size_t size)
{
	return rs_ctl_op_READ(dev, addr, data, size);
}

/**
 * write a Sensor Register
 *
 * @callgraph
 */
RS_IF rs_ret_t rs_ctl_write_reg(rs_ctl_dev_t dev, uint32_t addr, uint32_t data)
{
	rs_ret_t ret;
	uint8_t wdata[4];
	uint32_to_uint8(data, &wdata[0]);
	ret = rs_ctl_op_WRITE(dev, addr, wdata, 4);
	return ret;
}

/**
 * read a Sensor Register
 *
 * @callgraph
 */
RS_IF rs_ret_t rs_ctl_read_reg(rs_ctl_dev_t dev, uint32_t addr, uint32_t *data)
{
	uint8_t rdata[4] = { 0 };
	rs_ret_t ret = rs_ctl_op_READ(dev, addr, rdata, 4);
	if (ret == RS_OK)
	{
		uint8_to_uint32(&rdata[0], data);
	}
	return ret;
}

/**
 * write Sensor Registers
 *
 * @callgraph
 */
RS_IF rs_ret_t rs_ctl_write_regs(rs_ctl_dev_t dev, uint32_t addr, const uint32_t *data, rs_size_t num)
{
	rs_ret_t ret;
	rs_size_t i;
	uint8_t *wdata = (uint8_t *)calloc(num, sizeof(uint32_t));
	if (wdata == NULL)
	{
		return RS_ENOMEM;
	}
	for (i = 0; i < num; ++i)
	{
		uint32_to_uint8(data[i], &wdata[4 * i]);
	}
	ret = rs_ctl_op_WRITE(dev, addr, wdata, num * sizeof(uint32_t));
	free(wdata);
	return ret;
}

/**
 * read Sensor Registers
 *
 * @callgraph
 */
RS_IF rs_ret_t rs_ctl_read_regs(rs_ctl_dev_t dev, uint32_t addr, uint32_t *data, rs_size_t num)
{
	rs_ret_t ret;
	rs_size_t i;
	uint8_t *rdata = (uint8_t *)calloc(num, sizeof(uint32_t));
	if (rdata == NULL)
	{
		return RS_ENOMEM;
	}
	ret = rs_ctl_op_READ(dev, addr, rdata, num * sizeof(uint32_t));
	for (i = 0; i < num; ++i)
	{
		uint8_to_uint32(&rdata[4 * i], &data[i]);
	}
	free(rdata);
	return ret;
}
