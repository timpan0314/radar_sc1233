/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file rs_i2c.cpp
 */

#include "rs_i2c.h"
#include "defs/rs_dev_addr.h"
#include "defs/rs_dev_opcode.h"

rs_ret_t rs_i2c::write(const uint8_t *wdata, size_t size, rs_bool_t end)
{
	const char *write_data = (const char *)wdata;
	(void)end;
	int ack = dev.write(slave_address, write_data, size);
	return ((ack == 0) ? RS_OK : RS_EDEV);
}

rs_ret_t rs_i2c::write_low_memory(const uint8_t *hdata, size_t hsize, const uint8_t *wdata, size_t wsize)
{
	dev.start();
	int ack = dev.write(slave_address);
	for (size_t i = 0; ack == 1 && i < hsize; ++i)
	{
		ack = dev.write(hdata[i]);
	}
	for (size_t i = 0; ack == 1 && i < wsize; ++i)
	{
		ack = dev.write(wdata[i]);
	}
	dev.stop();
	return ((ack == 1) ? RS_OK : RS_EDEV);
}
	
rs_ret_t rs_i2c::read(const uint8_t *wdata, size_t wsize, uint8_t *rdata, size_t rsize)
{
	int ack = dev.write(slave_address, (const char *)&wdata[0], wsize, true);
	if (ack == 0)
	{
		ack = dev.read(slave_address, (char *)&rdata[0], rsize);
	}
	return ((ack == 0) ? RS_OK : RS_EDEV);
}

rs_ret_t rs_i2c::spi_read(const uint8_t *wdata, uint8_t *rdata, size_t size)
{
	return RS_ENOSUPPORT;
}
