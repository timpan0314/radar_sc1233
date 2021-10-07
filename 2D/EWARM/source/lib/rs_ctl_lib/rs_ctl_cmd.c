/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file rs_ctl_cmd.c
 */

#include <stdint.h>
#include <stdlib.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "rs_sleep.h"
#include "defs/rs_dev_addr.h"
#include "defs/rs_dev_opcode.h"
#include "defs/rs_dev_term.h"
#include "rs_ctl_dev.h"
#include "rs_ctl_op.h"
#include "rs_ctl_data.h"
#include "rs_ctl_sensor_data.h"
#include "rs_ctl_cmd.h"


#define SEQ_DISABLE_REG_EN_SEQ					(0x00000000)
#define SEQ_ENABLE_REG_CLKGEN					RS_CTL_REG_CLKGEN_SEQ_ENABLE
#if defined(RS_CPUIF_USE_I2C)
#define DISABLE_SEQ_REG_CLKGEN					(0x00000011)
#else
#define DISABLE_SEQ_REG_CLKGEN					(0x00000033)
#endif
#define SEQ_DISABLE_REG_CLKGEN					(SEQ_ENABLE_REG_CLKGEN ^ DISABLE_SEQ_REG_CLKGEN)
#define SEQ_ENABLE_REG_EN_SEQ					(0x00000001)

/// @cond
/** obsoleted
 *  replaced by rs_ctl_cmd_chipboot_sc123x()
 */
/**
 * boot ("Start-Up") command \cite Boot
 *
 * @bug wait time to rising NRST and wait time for EFUSE is not optimized.
 *
 * @callgraph
 */
RS_IF rs_ret_t rs_ctl_cmd_boot(rs_ctl_dev_t dev)
{
	// CE/NRST setting
	RS_CALL(rs_ctl_dev_term_set(dev, RS_TERM_CE, RS_TRUE));
	rs_usleep(2 * 1000);
	RS_CALL(rs_ctl_dev_term_set(dev, RS_TERM_NRST, RS_TRUE));

	RS_CALL(rs_ctl_op_SRST(dev, 1));
	rs_usleep(5 * 1000);

	// read efuse
	RS_CALL(rs_ctl_write_reg(dev, RS_CTL_REG_RD_START_EFUSE, 0x00000001));
	rs_usleep(100);
	RS_CALL(rs_ctl_write_reg(dev, RS_CTL_REG_CLKGEN, 0x001FFC44));
	{
		uint32_t rvalue = 0;
		RS_CALL(rs_ctl_read_reg(dev, RS_CTL_REG_EFUSE37, &rvalue));
		if ((rvalue & 0x000000FF) != 0) {
			RS_CALL(rs_ctl_write_reg(dev, RS_CTL_REG_SELSET_F_4, 0x00000000));
		}
	}

	RS_CALL(rs_ctl_op_DSLEEP(dev));

	return RS_OK;
}
/// @endcond

/**
 * "shutdown" command \cite Shutdown
 *
 * @callgraph
 */
RS_IF rs_ret_t rs_ctl_cmd_shutdown(rs_ctl_dev_t dev)
{
	RS_CALL(rs_ctl_dev_term_set(dev, RS_TERM_NRST, RS_FALSE));
	rs_usleep(5);
	RS_CALL(rs_ctl_dev_term_set(dev, RS_TERM_CE, RS_FALSE));

	return RS_OK;
}

/**
 * "disable Sequencer" to write Sequencer Code \cite WriteSEQPrc
 *
 * @callgraph
 */
RS_IF rs_ret_t rs_ctl_cmd_disable_seq(rs_ctl_dev_t dev)
{
	RS_CALL(rs_ctl_write_reg(dev, RS_CTL_REG_EN_SEQ, SEQ_DISABLE_REG_EN_SEQ));
	RS_CALL(rs_ctl_write_reg(dev, RS_CTL_REG_CLKGEN, SEQ_DISABLE_REG_CLKGEN));

	return RS_OK;
}

/**
 * "enable Sequencer" (makes not possible to write Sequencer Code) \cite ActivateSEQ
 *
 * @callgraph
 */
RS_IF rs_ret_t rs_ctl_cmd_enable_seq(rs_ctl_dev_t dev)
{
	RS_CALL(rs_ctl_write_reg(dev, RS_CTL_REG_CLKGEN, SEQ_ENABLE_REG_CLKGEN));
	RS_CALL(rs_ctl_op_SRST(dev, 1));
	RS_CALL(rs_ctl_write_reg(dev, RS_CTL_REG_EN_SEQ, SEQ_ENABLE_REG_EN_SEQ));
	RS_CALL(rs_ctl_op_SRST(dev, 1));

	return RS_OK;
}

static rs_ret_t read_status(rs_ctl_dev_t dev, const struct rs_ctl_sensor_data_set *set, struct rs_ctl_sensor_data *data);
static rs_ret_t read_registers(rs_ctl_dev_t dev, const struct rs_ctl_sensor_data_set *set, struct rs_ctl_sensor_data *data);
static rs_ret_t read_fifo(rs_ctl_dev_t dev, const struct rs_ctl_sensor_data_set *set, struct rs_ctl_sensor_data *data);

/**
 * get distance data from Sensor FIFO or registers \cite ReadResult
 *
 * @callgraph
 */
RS_IF rs_ret_t rs_ctl_cmd_get_sensor_data(rs_ctl_dev_t dev, const struct rs_ctl_sensor_data_set *set, struct rs_ctl_sensor_data *data)
{
	RS_ASSERT(dev != NULL, RS_EINV);
	RS_ASSERT(set != NULL, RS_EFAULT);
	RS_ASSERT(data != NULL, RS_EFAULT);

	RS_CALL(read_status(dev, set, data));
#if defined(RS_CPUIF_USE_I2C)
	RS_CALL(rs_ctl_op_HLDDT(dev, 1, 0));
	RS_CALL(read_fifo(dev, set, data));
	RS_CALL(read_registers(dev, set, data));
	RS_CALL(rs_ctl_op_UPDDT(dev, 1));
#else
	RS_CALL(read_registers(dev, set, data));
	RS_CALL(read_fifo(dev, set, data));
#endif

	return RS_OK;
}

/**
 * wait trigger of OR pin and get sensor data
 *
 * @callgraph
 */
RS_IF rs_ret_t rs_ctl_cmd_wait_and_get_sensor_data(rs_ctl_dev_t dev, uint32_t timeout, const struct rs_ctl_sensor_data_set *set, struct rs_ctl_sensor_data *data)
{
	RS_ASSERT(dev != NULL, RS_EINV);
	RS_ASSERT(set != NULL, RS_EFAULT);
	RS_ASSERT(data != NULL, RS_EFAULT);

	{
		rs_bool_t val = RS_FALSE;
		RS_CALL(rs_ctl_dev_term_trig_wait(dev, timeout, RS_TERM_OR, RS_TERM_TRIGGER_RISING, &val));
		RS_ASSERT(val == RS_TRUE, RS_EDEV);
	}

	if(set->reg.enable || set->status.enable) {
#ifdef RS_SETUP_REGISTER_ACCESS_IS_CONCEALED
		/* no operation here */
#else
		rs_usleep(30);
#endif
	}

	return rs_ctl_cmd_get_sensor_data(dev, set, data);
}

/**
 * read status register
 * @callgraph
 */
static rs_ret_t read_status(rs_ctl_dev_t dev, const struct rs_ctl_sensor_data_set *set, struct rs_ctl_sensor_data *data)
{
	if(set->status.enable) {
		RS_CALL(rs_ctl_op_RDSR2(dev, &(data->status)));
	}
	return RS_OK;
}

/**
 * read registers
 * @callgraph
 */
static rs_ret_t read_registers(rs_ctl_dev_t dev, const struct rs_ctl_sensor_data_set *set, struct rs_ctl_sensor_data *data)
{
	if(set->reg.enable) {
		rs_size_t i = 0;
		data->reg.num = set->reg.num;
		RS_ASSERT((data->reg.data = (uint32_t *) calloc(set->reg.num, sizeof(uint32_t))) != NULL, RS_ENOMEM);
#if !defined(RS_CPUIF_USE_I2C)
		RS_CALL(rs_ctl_op_HLDDT(dev, 1, 0));
#endif
		for(i=0; i<set->reg.num; i++) {
			RS_CALL(rs_ctl_read_reg(dev, set->reg.addr[i], &(data->reg.data[i])));
		}
#if !defined(RS_CPUIF_USE_I2C)
		RS_CALL(rs_ctl_op_UPDDT(dev, 1));
#endif
	}
	return RS_OK;
}

/**
 * read FIFO
 * @callgraph
 */
static rs_ret_t read_fifo(rs_ctl_dev_t dev, const struct rs_ctl_sensor_data_set *set, struct rs_ctl_sensor_data *data)
{
	if(set->sram.enable) {
		rs_size_t wordsize_sram = 0;
		wordsize_sram += set->sram.exist_header_header == RS_TRUE ? 1 : 0;
		wordsize_sram += set->sram.exist_header_status == RS_TRUE ? 1 : 0;
		wordsize_sram += set->sram.wordsize_body;
		wordsize_sram += 1;
		data->sram.size = wordsize_sram * set->sram.bytesize_word;

		RS_ASSERT((data->sram.data = (uint8_t *) calloc(wordsize_sram, set->sram.bytesize_word)) != NULL, RS_ENOMEM);
		RS_CALL(rs_ctl_read_mem(dev, RS_CTL_TOPADDR_FIFO, data->sram.data, data->sram.size));
	}
	return RS_OK;
}
