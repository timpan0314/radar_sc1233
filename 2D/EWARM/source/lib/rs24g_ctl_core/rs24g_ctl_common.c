/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file rs24g_ctl_common.c
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "rs_ctl_sensor_data.h"
#include "rs_ctl_cmd.h"

#include "rs24g_ctl_param.h"
#include "rs_handle.h"
#include "rs24g_ctl_common.h"


/**
 * get data from Sensor
 * @callgraph
 */
extern rs_ret_t get_devdata(rs_handle_t handle, uint32_t timeout, const uint16_t *reg_addr, rs_size_t reg_size, uint32_t *reg)
{
	rs_ret_t ret = RS_OK;

	struct rs_handle *lh = (struct rs_handle *) handle;

	struct rs_ctl_sensor_data_set set;
	struct rs_ctl_sensor_data get;

	set.sram.enable = RS_TRUE;
	set.sram.exist_header_header = (lh->code_ref.info.frame_header != 0) ? RS_TRUE : RS_FALSE;
	set.sram.exist_header_status = (lh->code_ref.info.frame_status != 0) ? RS_TRUE : RS_FALSE;
	set.sram.bytesize_word = (lh->code_ref.info.frame_datasel != 0) ? 3 : 4;
	set.sram.wordsize_body = lh->code_ref.info.frame_size;

	set.reg.enable = RS_TRUE;
	set.reg.num = reg_size;
	set.reg.addr = reg_addr;

	set.status.enable = RS_FALSE;

	ret = rs_ctl_sensor_data_init(&get);

	if(ret == RS_OK) {
		ret = rs_ctl_cmd_wait_and_get_sensor_data(lh->dev, timeout, &set, &get);

		if(ret == RS_OK) {
			memcpy(reg, get.reg.data, reg_size * sizeof(uint32_t));
		}

		if(ret == RS_OK) {
			ret = rs_ctl_sensor_data_free(&get);
		} else {
			rs_ctl_sensor_data_free(&get);
		}
	}

	return ret;
}

/**
 * get FIFO data from Sensor
 * @callgraph
 */
extern rs_ret_t get_devfifo(rs_handle_t handle, uint32_t timeout, rs_size_t fifo_size, uint8_t *fifo_data, struct rs_fifo_info *fifo_info)
{
	rs_ret_t ret = RS_OK;

	struct rs_handle *lh = (struct rs_handle *) handle;

	struct rs_ctl_sensor_data_set set;
	struct rs_ctl_sensor_data get;

	set.sram.enable = RS_TRUE;
	set.sram.exist_header_header = (lh->code_ref.info.frame_header != 0) ? RS_TRUE : RS_FALSE;
	set.sram.exist_header_status = (lh->code_ref.info.frame_status != 0) ? RS_TRUE : RS_FALSE;
	set.sram.bytesize_word = (lh->code_ref.info.frame_datasel != 0) ? 3 : 4;
	set.sram.wordsize_body = lh->code_ref.info.frame_size;

	set.reg.enable = RS_FALSE;

	set.status.enable = RS_FALSE;

	ret = rs_ctl_sensor_data_init(&get);

	if(ret == RS_OK) {
		ret = rs_ctl_cmd_wait_and_get_sensor_data(lh->dev, timeout, &set, &get);

		if(ret == RS_OK) {
			ret = rs_ctl_sensor_data_parse(&set, &get);
		}

		if(ret == RS_OK) {
			size_t src_sz = get.sram.parse.body.size;
			size_t dst_sz = fifo_size;
			if(dst_sz < src_sz) {
				memcpy(fifo_data, get.sram.parse.body.data, dst_sz);
			} else {
				memcpy(fifo_data, get.sram.parse.body.data, src_sz);
				if(dst_sz > src_sz) {
					memset(&fifo_data[src_sz], 0, dst_sz - src_sz);
				}
			}
			if(fifo_info != NULL) {
				if(set.sram.exist_header_status) {
					fifo_info->status = get.sram.parse.status;
				}
				if(set.sram.exist_header_header) {
					fifo_info->header = get.sram.parse.header;
				}
				fifo_info->end = get.sram.parse.endcode;
			}
		}

		if(ret == RS_OK) {
			ret = rs_ctl_sensor_data_free(&get);
		} else {
			rs_ctl_sensor_data_free(&get);
		}
	}

	return ret;
}

/**
 * get frame size in FIFO
 * @callgraph
 */
extern rs_ret_t get_size_fifo(rs_handle_t handle, rs_size_t *size)
{
	struct rs_handle *lh = (struct rs_handle *) handle;

	struct rs_ctl_sensor_data_set set;

	set.sram.enable = RS_TRUE;
	set.sram.exist_header_header = (lh->code_ref.info.frame_header != 0) ? RS_TRUE : RS_FALSE;
	set.sram.exist_header_status = (lh->code_ref.info.frame_status != 0) ? RS_TRUE : RS_FALSE;
	set.sram.bytesize_word = (lh->code_ref.info.frame_datasel != 0) ? 3 : 4;
	set.sram.wordsize_body = lh->code_ref.info.frame_size;

	return rs_ctl_sensor_data_get_sram_body_size(&set, size);
}

static rs_ret_t distance_range(struct rs_distance_data *data, uint8_t peak_level_lower)
{
	int i;

	for(i=0; i<5; i++) {
		if((data->distance[i] > RS_DISTANCE_MAX) || (data->distance[i] < RS_DISTANCE_MIN)) {
			data->distance[i] = RS_DISTANCE_INVALID;
			data->peak_level[i] = RS_PEAK_LEVEL_INVALID;
		}
		if(data->peak_level[i] < peak_level_lower) {
			data->distance[i] = RS_DISTANCE_INVALID;
		}
	}

	return RS_OK;
}


static uint8_t conv_power(uint32_t val)
{
	RET_CHECK(val != 0, 0);

	return (uint8_t) (10.0 * log10(val));
}

rs_ret_t get_distance_data(const uint32_t *regs, struct rs_distance_data *data, uint8_t peak_level_lower)
{
	const uint32_t * const regs_distance = regs + 0;
	const uint32_t * const regs_peak_level = regs + 3;
	const uint32_t * const regs_complex = regs + 8;
	int i;

	data->distance[0] = (int16_t) ((regs_distance[ 0] >> 16) & 0xffff);
	data->distance[1] = (int16_t) ((regs_distance[ 0] >>  0) & 0xffff);
	data->distance[2] = (int16_t) ((regs_distance[ 1] >> 16) & 0xffff);
	data->distance[3] = (int16_t) ((regs_distance[ 1] >>  0) & 0xffff);
	data->distance[4] = (int16_t) ((regs_distance[ 2] >>  0) & 0xffff);

	for(i=0; i<5; i++) {
		if(regs_peak_level[i] == 0) {
			data->peak_level[i] = RS_PEAK_LEVEL_INVALID;
		} else {
			data->peak_level[i] = conv_power(regs_peak_level[i]);
		}

		data->peak_real[i] = (int16_t) ((regs_complex[i] >> 16) & 0xffff);
		data->peak_imag[i] = (int16_t) ((regs_complex[i] >>  0) & 0xffff);
	}

	RET_ORG(rs_ret_t, distance_range(data, peak_level_lower), RS_OK)

	return RS_OK;
}

