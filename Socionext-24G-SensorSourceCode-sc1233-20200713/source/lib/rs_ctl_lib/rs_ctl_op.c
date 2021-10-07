/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file rs_ctl_op.c
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "rs_sleep.h"
#include "rs_ctl_dev.h"
#include "rs_ctl_op.h"


static rs_ret_t rs_ctl_op_fast_control(rs_ctl_dev_t dev, uint8_t opcode)
{
	uint8_t wdata[1] = { opcode };
	return rs_ctl_dev_write(dev, wdata, sizeof(wdata));
}

static rs_ret_t rs_ctl_op_write_status(rs_ctl_dev_t dev, uint8_t opcode, uint8_t data)
{
	uint8_t wdata[2] = { opcode, data };
	return rs_ctl_dev_write(dev, wdata, sizeof(wdata));
}

static rs_ret_t rs_ctl_op_read_status(rs_ctl_dev_t dev, uint8_t opcode, uint8_t *data)
{
	uint8_t wdata[1] = { opcode };
	uint8_t rdata[1] = { 0 };
	rs_ret_t ret = rs_ctl_dev_read(dev, wdata, sizeof(wdata), rdata, sizeof(rdata));
	if (ret == RS_OK)
	{
		*data = rdata[0];
	}
	return ret;
}

static rs_ret_t rs_ctl_op_write_region(rs_ctl_dev_t dev, uint32_t address, const uint8_t *data, rs_size_t size)
{
#if defined(RS_LOW_MEMORY)
	uint8_t wdata[4];
	wdata[0] = RS_CTL_OP_WRITE_DATA;
	wdata[1] = (address >> 16) & 0xff;
	wdata[2] = (address >>  8) & 0xff;
	wdata[3] = (address >>  0) & 0xff;
	return rs_ctl_dev_write_low_memory(dev, wdata, sizeof(wdata), data, size);
#else
	rs_ret_t ret;
	rs_size_t wsize = 4 + size;
	uint8_t *wdata = (uint8_t *)malloc(wsize);
	if (wdata == NULL)
	{
		return RS_ENOMEM;
	}
	wdata[0] = RS_CTL_OP_WRITE_DATA;
	wdata[1] = (address >> 16) & 0xff;
	wdata[2] = (address >>  8) & 0xff;
	wdata[3] = (address >>  0) & 0xff;
	memcpy(&wdata[4], data, size);
	ret = rs_ctl_dev_write(dev, wdata, wsize);
	free(wdata);
	return ret;
#endif
}

static rs_ret_t rs_ctl_op_read_region(rs_ctl_dev_t dev, uint32_t address, uint8_t *data, rs_size_t size)
{
	uint8_t wdata[4];
	wdata[0] = RS_CTL_OP_READ_DATA;
	wdata[1] = (address >> 16) & 0xff;
	wdata[2] = (address >>  8) & 0xff;
	wdata[3] = (address >>  0) & 0xff;
	return rs_ctl_dev_read(dev, wdata, sizeof(wdata), data, size);
}

/**
 * send HRST Command
 * @sa rs_ctl_dev_write()
 */
RS_IF rs_ret_t rs_ctl_op_HRST(rs_ctl_dev_t dev)
{
	return rs_ctl_op_fast_control(dev, RS_CTL_OP_HARD_RESET);
}

/**
 * send SRST Command
 * @sa rs_ctl_dev_write()
 */
RS_IF rs_ret_t rs_ctl_op_SRST(rs_ctl_dev_t dev, int with_extra)
{
	rs_ret_t ret = rs_ctl_op_fast_control(dev, RS_CTL_OP_SOFT_RESET);
#if defined(RS_CPUIF_USE_I2C)
	if (with_extra != 0)
	{
		if (ret == RS_OK)
		{
			ret = rs_ctl_op_fast_control(dev, RS_CTL_OP_MODECTL);
		}
	}
#endif
	return ret;
}

/**
 * send DSLEEP Command
 * @sa rs_ctl_dev_write()
 */
RS_IF rs_ret_t rs_ctl_op_DSLEEP(rs_ctl_dev_t dev)
{
	return rs_ctl_op_fast_control(dev, RS_CTL_OP_DEEP_SLEEP);
}

/**
 * send WRSR Command
 * @sa rs_ctl_dev_write()
 */
RS_IF rs_ret_t rs_ctl_op_WRSR(rs_ctl_dev_t dev, uint8_t data)
{
	return rs_ctl_op_write_status(dev, RS_CTL_OP_MODECTL, data);
}

/**
 * send RDSR Command
 * @sa rs_ctl_dev_read()
 */
RS_IF rs_ret_t rs_ctl_op_RDSR(rs_ctl_dev_t dev, uint8_t *data)
{
	return rs_ctl_op_read_status(dev, RS_CTL_OP_READ_STATUS_REG, data);
}

/**
 * send WRITE Command
 * @sa rs_ctl_dev_write()
 */
RS_IF rs_ret_t rs_ctl_op_WRITE(rs_ctl_dev_t dev, uint32_t address, const uint8_t *data, rs_size_t size)
{
	return rs_ctl_op_write_region(dev, address, data, size);
}

/**
 * send READ Command
 * @sa rs_ctl_dev_read()
 */
RS_IF rs_ret_t rs_ctl_op_READ(rs_ctl_dev_t dev, uint32_t address, uint8_t *data, rs_size_t size)
{
	return rs_ctl_op_read_region(dev, address, data, size);
}

/**
 * send RDSR2 Command
 * @sa rs_ctl_dev_read()
 */
RS_IF rs_ret_t rs_ctl_op_RDSR2(rs_ctl_dev_t dev, uint8_t *data)
{
	return rs_ctl_op_read_status(dev, RS_CTL_OP_READ_STATUS_REG2, data);
}

/**
 * send ENATM Command
 * @sa rs_ctl_dev_write()
 */
RS_IF rs_ret_t rs_ctl_op_ENATM(rs_ctl_dev_t dev)
{
	return rs_ctl_op_fast_control(dev, RS_CTL_OP_ENABLE_TIMER);
}

/**
 * send DISTM Command
 * @sa rs_ctl_dev_write()
 */
RS_IF rs_ret_t rs_ctl_op_DISTM(rs_ctl_dev_t dev)
{
	return rs_ctl_op_fast_control(dev, RS_CTL_OP_DISABLE_TIMER);
}

/**
 * send RUNTM Command
 * @sa rs_ctl_dev_write()
 */
RS_IF rs_ret_t rs_ctl_op_RUNTM(rs_ctl_dev_t dev)
{
	return rs_ctl_op_fast_control(dev, RS_CTL_OP_RUN_TIMER);
}

/**
 * send STPTM Command
 * @sa rs_ctl_dev_write()
 */
RS_IF rs_ret_t rs_ctl_op_STPTM(rs_ctl_dev_t dev)
{
	return rs_ctl_op_fast_control(dev, RS_CTL_OP_STOP_TIMER);
}

/**
 * send HLDDT Command
 * @sa rs_ctl_dev_write()
 */
RS_IF rs_ret_t rs_ctl_op_HLDDT(rs_ctl_dev_t dev, int with_timer, int in_deep_sleep)
{
	rs_ret_t ret = rs_ctl_op_fast_control(dev, RS_CTL_OP_HOLD_DATA);
#if defined(RS_CPUIF_USE_I2C)
	if (with_timer != 0)
	{
		if (ret == RS_OK)
		{
			if (in_deep_sleep != 0)
			{
				rs_usleep(5 * 1000);
			}
			ret = rs_ctl_op_fast_control(dev, RS_CTL_OP_SOFT_RESET);
		}
	}
#endif
	return ret;
}

/**
 * send UPDDT Command
 * @sa rs_ctl_dev_write()
 */
RS_IF rs_ret_t rs_ctl_op_UPDDT(rs_ctl_dev_t dev, int with_timer)
{
	rs_ret_t ret;
	rs_ret_t ret_pre = RS_OK;
#if defined(RS_CPUIF_USE_I2C)
	if (with_timer != 0)
	{
		ret_pre = rs_ctl_op_fast_control(dev, RS_CTL_OP_DEEP_SLEEP);
	}
#endif
	// always send UPDDT command
	ret = rs_ctl_op_fast_control(dev, RS_CTL_OP_UPDATE_DATA);
	if (ret == RS_OK)
	{
		ret = ret_pre;
	}
	return ret;
}
