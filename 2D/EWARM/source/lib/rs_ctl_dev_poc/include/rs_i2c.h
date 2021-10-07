/*
 * Copyright (c) 2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file rs_i2c.h
 * @brief Sensor driver implemantation for I2C on MBED\@HRM1017
 * @version (PRELIMINARY)
 * @warning This is just a sample source code.
 */

#ifndef __RS_I2C_H__
#define __RS_I2C_H__

//#include <mbed.h>  Tim
#include "rs24g_types.h"

/**
 * Sensor driver for I2C on MBED\@HRM1017
 *
 * @version (PRELIMINARY)
 */
class rs_i2c
{
private:
	I2C dev;
	int slave_address;

public:
	rs_i2c(int address, PinName sda, PinName scl, int hz)
		: dev(sda, scl)
		, slave_address(address)
	{
		dev.frequency(hz);
	}
	~rs_i2c() {}

public:
	/**
	 * raw I2C write operation with terminate option
	 *
	 * @param[in] wdata pointer of write data, might be NULL when read
	 * @param[in] size number of read data in bytes
	 * @param[in] end option to terminate a I2C transaction
	 */
	rs_ret_t write(const uint8_t *wdata, size_t size, rs_bool_t end = RS_TRUE);
	rs_ret_t write_low_memory(const uint8_t *hdata, size_t hsize, const uint8_t *wdata, size_t wsize);
	/**
	 * raw I2C read operation
	 *
	 * support only to read status registers
	 *
	 * @param[in] wdata pointer of write data
	 * @param[in] wsize number of write data in bytes
	 * @param[out] rdata pointer of read data
	 * @param[in] rsize number of read data in bytes
	 */
	rs_ret_t read(const uint8_t*wdata, size_t wsize, uint8_t *rdata, size_t rsize = 1);
	rs_ret_t spi_read(const uint8_t *wdata, uint8_t *rdata, size_t size);
};

#endif /* __RS_I2C_H__ */
