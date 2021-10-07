/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file rs_ctl_sensor_data.h
 * @brief sensor data
 * @version (PRELIMINARY)
 * @warning This is a part of sensor library source code for Evaluation Kit.
 */

#ifndef __RS_CTL_SENSOR_DATA_H__
#define __RS_CTL_SENSOR_DATA_H__

#include <stdint.h>

#include "rs24g_types.h"


/*
 * Type Definition
 */

/*
 * Constant Definition
 */

/*
 * Struct Definition
 */
struct rs_data
{
	rs_size_t size;
	const void *data;
};

struct rs_ctl_sensor_data_set
{
	struct
	{
		rs_bool_t enable;

		uint8_t mode;
		rs_size_t wordsize_body;
		rs_bool_t exist_header_header;
		rs_bool_t exist_header_status;
		rs_size_t bytesize_word;
	} sram;

	struct
	{
		rs_bool_t enable;

		uint8_t mode;
		rs_size_t num;
		const uint16_t *addr;
	} reg;

	struct
	{
		rs_bool_t enable;
	} status;
};

struct rs_ctl_sensor_data
{
	struct
	{
		uint8_t *data;
		rs_size_t size;

		struct
		{
			uint32_t status;
			uint32_t header;
			struct rs_data body;
			uint32_t endcode;
		} parse;
	} sram;

	struct
	{
		uint32_t *data;
		rs_size_t num;
	} reg;

	uint8_t status;
};

/*
 * Function Definition
 */
RS_IF rs_ret_t rs_ctl_sensor_data_init(struct rs_ctl_sensor_data *data);
RS_IF rs_ret_t rs_ctl_sensor_data_parse(const struct rs_ctl_sensor_data_set *set, struct rs_ctl_sensor_data *data);
RS_IF rs_ret_t rs_ctl_sensor_data_get_sram_body_size(const struct rs_ctl_sensor_data_set *set, rs_size_t *size);
RS_IF rs_ret_t rs_ctl_sensor_data_free(struct rs_ctl_sensor_data *data);

#endif	/* __RS_CTL_SENSOR_DATA_H__ */
