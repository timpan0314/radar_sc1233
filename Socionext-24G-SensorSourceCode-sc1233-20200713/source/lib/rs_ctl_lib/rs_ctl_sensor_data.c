/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs_ctl_sensor_data.c
 */

#include <stdint.h>
#include <stdlib.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "rs_sleep.h"
#include "defs/rs_dev_term.h"
#include "defs/rs_dev_addr.h"
#include "defs/rs_dev_opcode.h"
#include "rs_ctl_op.h"
#include "rs_ctl_data.h"
#include "rs_ctl_sensor_data.h"


static rs_ret_t convhead(const uint8_t **d, rs_size_t *res, rs_size_t width, uint32_t *data)
{
	RET_CHECK(d != NULL, RS_EFAULT);
	RET_CHECK(*d != NULL, RS_EFAULT);
	RET_CHECK(res != NULL, RS_EFAULT);
	RET_CHECK(data != NULL, RS_EFAULT);
	RET_CHECK(*res >= width, RS_ECOM);

	*data = 0;

	switch(width) {
	case 3:
		*data += (*d)[0] << 16;
		*data += (*d)[1] <<  8;
		*data += (*d)[2] <<  0;
		break;
	case 4:
		*data += (*d)[0] << 24;
		*data += (*d)[1] << 16;
		*data += (*d)[2] <<  8;
		*data += (*d)[3] <<  0;
		break;
	default:
		return RS_ECOM;
	}

	*d += width;
	*res -= width;

	return RS_OK;
}

RS_IF rs_ret_t rs_ctl_sensor_data_init(struct rs_ctl_sensor_data *data)
{
	RET_CHECK(data != NULL, RS_EFAULT);

	data->sram.data = NULL;
	data->reg.data = NULL;

	return RS_OK;
}

RS_IF rs_ret_t rs_ctl_sensor_data_parse(const struct rs_ctl_sensor_data_set *set, struct rs_ctl_sensor_data *data)
{
	RET_CHECK(set != NULL, RS_EFAULT);
	RET_CHECK(data != NULL, RS_EFAULT);

	if(set->sram.enable) {
		const uint8_t *ind = data->sram.data;
		rs_size_t ires = data->sram.size;

		// status
		if(set->sram.exist_header_status) {
			RET_ORG(rs_ret_t, convhead(&ind, &ires, set->sram.bytesize_word, &(data->sram.parse.status)), RS_OK);
		}

		// header
		if(set->sram.exist_header_header) {
			RET_ORG(rs_ret_t, convhead(&ind, &ires, set->sram.bytesize_word, &(data->sram.parse.header)), RS_OK);
		}

		// body
		{
			const rs_size_t body_size = set->sram.wordsize_body * set->sram.bytesize_word;
			RET_CHECK(ires >= body_size, RS_ECOM);
			data->sram.parse.body.data = ind;
			data->sram.parse.body.size = body_size;
			ind += body_size;
			ires -= body_size;
		}

		// endcode
		{
			RET_ORG(rs_ret_t, convhead(&ind, &ires, set->sram.bytesize_word, &(data->sram.parse.endcode)), RS_OK);
		}

		RET_CHECK(ires == 0, RS_ECOM);
	}

	return RS_OK;
}

RS_IF rs_ret_t rs_ctl_sensor_data_get_sram_body_size(const struct rs_ctl_sensor_data_set *set, rs_size_t *size)
{
	RET_CHECK(set != NULL, RS_EFAULT);
	RET_CHECK(size != NULL, RS_EFAULT);

	if(set->sram.enable) {
		*size = set->sram.wordsize_body * set->sram.bytesize_word;
	} else {
		*size = 0;
	}

	return RS_OK;
}

RS_IF rs_ret_t rs_ctl_sensor_data_free(struct rs_ctl_sensor_data *data)
{
	RET_CHECK(data != NULL, RS_EFAULT);

	free(data->sram.data);
	free(data->reg.data);

	data->sram.data = NULL;
	data->reg.data = NULL;

	return RS_OK;
}

