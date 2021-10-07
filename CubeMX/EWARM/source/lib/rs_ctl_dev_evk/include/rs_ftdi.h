/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs_ftdi.h
 */

#ifndef __RS_FTDI_H__
#define __RS_FTDI_H__

#include <stdint.h>
#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#include "LibFT4222.h"
typedef HANDLE GPIO_EVENT_HANDLE;
#pragma comment(lib, "winmm.lib")
#else	/* WIN32 */
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include "libft4222.h"
typedef EVENT_HANDLE GPIO_EVENT_HANDLE;
#endif	/* WIN32 */

#include "rs24g_types.h"
#include "rs_macro.h"

class rs_ftdi_spi {
private:
	FT_HANDLE handle = NULL;
public:
	rs_ret_t open(PVOID devname);
	rs_ret_t close(void);
	rs_ret_t write(const uint8_t *wdata, size_t size, rs_bool_t end = RS_TRUE);
	rs_ret_t write_low_memory(const uint8_t *hdata, size_t hsize, const uint8_t *wdata, size_t wsize);
	rs_ret_t read(const uint8_t *wdata, size_t wsize, uint8_t *rdata, size_t rsize);
	rs_ret_t spi_read(const uint8_t *wdata, uint8_t *rdata, size_t size);
};

class rs_ftdi_gpio {
private:
	FT_HANDLE *handle;
	GPIO_Port port;
	GPIO_EVENT_HANDLE *gpio_event;
public:
	rs_ftdi_gpio(FT_HANDLE *handle, GPIO_Port port, GPIO_EVENT_HANDLE *gpio_event)
		: handle(handle)
		, port(port)
		, gpio_event(gpio_event)
	{}
	rs_ret_t get(rs_bool_t *val);
	rs_ret_t set(rs_bool_t val);
	rs_ret_t get_trigger(uint32_t trigger, rs_bool_t *val, rs_bool_t *triggered);
	rs_ret_t set_trigger(uint32_t trigger);
	rs_ret_t clear_trigger(void);
	rs_ret_t wait_trigger(uint32_t timeout /*ms */, uint32_t trigger, rs_bool_t *val);
};

class rs_ftdi_term {
private:
	FT_HANDLE handle = NULL;
	GPIO_EVENT_HANDLE gpio_event;
public:
	rs_ftdi_gpio Nrst;
	rs_ftdi_gpio Ce;
	rs_ftdi_gpio Or;
	rs_ftdi_gpio Detout;
public:
	rs_ftdi_term(GPIO_Port _nrst, GPIO_Port _ce, GPIO_Port _or, GPIO_Port _detout)
		: Nrst(&handle, _nrst, &gpio_event)
		, Ce(&handle, _ce, &gpio_event)
		, Or(&handle, _or, &gpio_event)
		, Detout(&handle, _detout, &gpio_event)
	{}
	rs_ret_t open(PVOID devname);
	rs_ret_t close(void);
	rs_ftdi_gpio* term2gpio(uint32_t term);
	rs_ftdi_gpio* term2trigger(uint32_t term);
	rs_ret_t get(uint32_t term, rs_bool_t *val);
	rs_ret_t set(uint32_t term, rs_bool_t val);
	rs_ret_t set_trigger(uint32_t term, uint32_t trigger);
	rs_ret_t clear_trigger(uint32_t term);
	rs_ret_t wait_trigger(uint32_t term, uint32_t timeout /*ms */, uint32_t trigger, rs_bool_t *val);
};
#endif /* __RS_FTDI_H__ */
