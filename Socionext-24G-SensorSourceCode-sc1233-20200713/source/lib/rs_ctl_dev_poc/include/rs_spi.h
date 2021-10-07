/*
 * Copyright (c) 2015-2018 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file rs_spi.h
 * @brief Sensor driver implemantation for SPI on MBED\@HRM1017
 * @version (PRELIMINARY)
 * @warning This is just a sample source code.
 */

#ifndef __RS_SPI_H__
#define __RS_SPI_H__

//#include "mbed.h"  Tim
#include "rs24g_types.h"

/**
 * Sensor driver for SPI on MBED\@HRM1017
 *
 * @version (PRELIMINARY)
 */
class rs_spi
{
private:
	SPI spi;
	DigitalOut term_sel;

	/**
	 * raw SPI write/read operation with terminate option
	 *
	 * @param[in] wdata pointer of write data, might be NULL when read
	 * @param[in] wsize number of write data in bytes
	 * @param[out] rdata pointer of read data, might be NULL when write
	 * @param[in] rsize number of read data in bytes
	 * @param[in] end option to terminate a SPI transaction
	 */
	void readwrite(const void *wdata, size_t wsize, void *rdata, size_t rsize, bool end=true) {
		term_sel = 0;
		spi.write(reinterpret_cast<const char *>(wdata), wsize, reinterpret_cast<char *>(rdata), rsize);
		if (end)
		  term_sel = 1;
	}

public:
	rs_spi(PinName mosi, PinName miso, PinName sclk, PinName ssel, uint32_t freq) : spi(mosi, miso, sclk), term_sel(ssel) {
		spi.format(8, 0);
		spi.frequency(freq);
		term_sel = 1;
	}
	virtual ~rs_spi(void) {}

	/**
	 * SPI write operation with terminate option
	 *
	 * @param[in] wdata pointer of write data
	 * @param[in] size number of write data in bytes
	 * @param[in] end option to terminate a SPI transaction
	 */
	rs_ret_t write(const void *wdata, size_t size, bool end=true) {
	  	readwrite(wdata, size, NULL, 0, end);
		return RS_OK;
	}

	rs_ret_t write_low_memory(const void *hdata, size_t hsize, const void *wdata, size_t wsize) {
	  	readwrite(hdata, hsize, NULL, 0, false);
	  	readwrite(wdata, wsize, NULL, 0, true);
		return RS_OK;
	}

	/**
	 * SPI read operation
	 *
	 * @param[in] wdata pointer of write data
	 * @param[in] wsize number of write data in bytes
	 * @param[out] rdata pointer of read data
	 * @param[in] rsize number of read data in bytes
	 *
	 * @sa rs_spi::write(const void *, size_t, void *, size_t, bool)
	 */
	rs_ret_t read(const void *wdata, size_t wsize, void *rdata, size_t rsize) {
		uint8_t dummy = 0;
		readwrite(wdata, wsize, NULL, 0, false);
		readwrite(&dummy, 1, NULL, 0, false);
		readwrite(NULL, 0, rdata, rsize, true);
		return RS_OK;
	}

	/**
	 * SPI raw read operation
	 *
	 * @param[in] wdata pointer of write data
	 * @param[out] rdata pointer of read data
	 * @param[in] size number of read data in bytes
	 *
	 * @sa rs_spi::write(const void *, size_t, void *, size_t, bool)
	 */
	rs_ret_t spi_read(const void *wdata, void *rdata, size_t size) {
		readwrite(wdata, size, rdata, size);
		return RS_OK;
	}
};

#endif /* __RS_SPI_H__ */
