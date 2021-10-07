/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs_ftdi.cpp
 */

#include <stdint.h>
#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#include "LibFT4222.h"
#pragma comment(lib, "winmm.lib")
#else	/* WIN32 */
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include "libft4222.h"
#endif	/* WIN32 */
#include <cstring>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "rs_ftdi.h"
#include "defs/rs_dev_term.h"

rs_ret_t rs_ftdi_spi::open(PVOID DEVNAME) {
	RET_CONV(FT_OpenEx(DEVNAME, FT_OPEN_BY_DESCRIPTION, &handle), FT_OK, RS_ECOM);
	RET_CONV(FT4222_SPIMaster_Init(handle, SPI_IO_SINGLE, CLK_DIV_2, CLK_IDLE_LOW, CLK_LEADING, 0x01), FT4222_OK, RS_ECOM);
	RET_CONV(FT4222_SPI_SetDrivingStrength(handle, DS_12MA, DS_12MA, DS_12MA), FT4222_OK, RS_ECOM);
	return RS_OK;
}

rs_ret_t rs_ftdi_spi::close(void) {
	RET_CONV(FT4222_UnInitialize(handle), FT4222_OK, RS_ECOM);
	RET_CONV(FT_Close(handle), FT_OK, RS_ECOM);
	handle = NULL;
	return RS_OK;
}

rs_ret_t rs_ftdi_spi::write(const uint8_t *wdata, size_t size, rs_bool_t end) {
	uint16 rsize = 0;
	RET_CONV(FT4222_SPIMaster_SingleWrite(handle, (uint8_t *) wdata, (uint16) size, &rsize, end), FT4222_OK, RS_ECOM);
	RET_CHECK(size == rsize, RS_ECOM);
	return RS_OK;
}

rs_ret_t rs_ftdi_spi::write_low_memory(const uint8_t *hdata, size_t hsize, const uint8_t *wdata, size_t wsize) {
	uint16 rsize = 0;
	RET_CONV(FT4222_SPIMaster_SingleWrite(handle, (uint8_t *) hdata, (uint16) hsize, &rsize, false), FT4222_OK, RS_ECOM);
	RET_CHECK(hsize == rsize, RS_ECOM);
	RET_CONV(FT4222_SPIMaster_SingleWrite(handle, (uint8_t *) wdata, (uint16) wsize, &rsize, false), FT4222_OK, RS_ECOM);
	RET_CHECK(wsize == rsize, RS_ECOM);
	return RS_OK;
}

rs_ret_t rs_ftdi_spi::read(const uint8_t *wdata, size_t wsize, uint8_t *rdata, size_t rsize) {
	uint16_t asize = 0;
#if 1
	uint16 size = static_cast<uint16>(wsize + rsize + 1);
	uint8_t *xwdata = new uint8_t[size]{};
	uint8_t *xrdata = new uint8_t[size]{};
	memcpy(xwdata, wdata, wsize);
	RET_CONV(FT4222_SPIMaster_SingleReadWrite(handle, &xrdata[0], &xwdata[0], (uint16) size, &asize, RS_TRUE), FT4222_OK, RS_ECOM);
	RET_CHECK(asize == size, RS_ECOM);
	memcpy(rdata, &xrdata[wsize + 1], rsize);
	delete [] xwdata;
	delete [] xrdata;
#else
	uint8_t dummy = 0;
	RET_CHECK((((wdata[0] == RS_CTL_OP_READ_DATA) && (wsize == 4)) ||
		   (wsize == 1)), RS_ECOM);
	RET_CHECK(rsize > 0, RS_ECOM);
	RET_CONV(FT4222_SPIMaster_SingleWrite(handle, (uint8_t *) wdata, (uint16) wsize, &asize, RS_FALSE), FT4222_OK, RS_ECOM);
	RET_CHECK(asize == wsize, RS_ECOM);
	RET_CONV(FT4222_SPIMaster_SingleWrite(handle, (uint8_t *) &dummy, (uint16) 1, &asize, RS_FALSE), FT4222_OK, RS_ECOM);
	RET_CONV(FT4222_SPIMaster_SingleRead(handle, &rdata[0], (uint16) rsize, &asize, RS_TRUE), FT4222_OK, RS_ECOM);
	RET_CHECK(asize == rsize, RS_ECOM);
#endif
	return RS_OK;
}

rs_ret_t rs_ftdi_spi::spi_read(const uint8_t *wdata, uint8_t *rdata, size_t size) {
	uint16_t asize = 0;
	RET_CHECK(size > 0, RS_ECOM);
	RET_CONV(FT4222_SPIMaster_SingleReadWrite(handle, (uint8_t *) rdata, (uint8_t *) wdata, (uint16) size, &asize, RS_TRUE), FT4222_OK, RS_ECOM);
	RET_CHECK(asize == size, RS_ECOM);

	return RS_OK;
}

rs_ret_t rs_ftdi_term::open(PVOID DEVNAME) {
	RET_CONV(FT_OpenEx(DEVNAME, FT_OPEN_BY_DESCRIPTION, &handle), FT_OK, RS_ECOM);

#ifdef WIN32
	gpio_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	RET_CHECK(gpio_event != NULL, RS_ECOM);
	RET_CONV(FT_SetEventNotification(handle, FT_EVENT_RXCHAR, gpio_event), FT_OK, RS_ECOM);
#else
	pthread_mutex_init(&(gpio_event.eMutex), NULL);
	pthread_cond_init(&(gpio_event.eCondVar), NULL);
	RET_CONV(FT_SetEventNotification(handle, FT_EVENT_RXCHAR, (PVOID) &(gpio_event)), FT_OK, RS_ECOM);
#endif

	{
		GPIO_Dir gpioDir[] = {GPIO_OUTPUT, GPIO_OUTPUT, GPIO_INPUT, GPIO_INPUT};
		RET_CONV(FT4222_GPIO_Init(handle, gpioDir), FT4222_OK, RS_ECOM);
	}

	RET_CONV(FT4222_SetSuspendOut(handle, FALSE), FT4222_OK, RS_ECOM);
	RET_CONV(FT4222_SetWakeUpInterrupt(handle, FALSE), FT4222_OK, RS_ECOM);
	return RS_OK;
}

rs_ret_t rs_ftdi_term::close(void) {
	RET_CONV(FT4222_UnInitialize(handle), FT4222_OK, RS_ECOM);
	RET_CONV(FT_Close(handle), FT_OK, RS_ECOM);
	handle = NULL;
	return RS_OK;
}

rs_ftdi_gpio* rs_ftdi_term::term2gpio(uint32_t term) {
	switch (term) {
	case RS_TERM_NRST:
		return &Nrst;
	case RS_TERM_CE:
		return &Ce;
	case RS_TERM_OR:
		return &Or;
	case RS_TERM_DETOUT:
		return &Detout;
	default:
		return NULL;
	}
}

rs_ftdi_gpio* rs_ftdi_term::term2trigger(uint32_t term) {
	return(term2gpio(term));
}

rs_ret_t rs_ftdi_term::get(uint32_t term, rs_bool_t *val) {
	return term2gpio(term)->get(val);
}

rs_ret_t rs_ftdi_term::set(uint32_t term, rs_bool_t val) {
	return term2gpio(term)->set(val);
}

rs_ret_t rs_ftdi_term::set_trigger(uint32_t term, uint32_t trigger) {
	return term2trigger(term)->set_trigger(trigger);
}

rs_ret_t rs_ftdi_term::clear_trigger(uint32_t term) {
	return term2trigger(term)->clear_trigger();
}

rs_ret_t rs_ftdi_term::wait_trigger(uint32_t term, uint32_t timeout /*ms */, uint32_t trigger, rs_bool_t *val) {
	return term2trigger(term)->wait_trigger(timeout, trigger, val);
}

static GPIO_Trigger convert_trigger(uint32_t trigger) {
	GPIO_Trigger trig = (GPIO_Trigger) 0;

	if((trigger & RS_TERM_TRIGGER_RISING) != 0) {
		trig = (GPIO_Trigger)(trig | GPIO_TRIGGER_RISING);
	}
	if((trigger & RS_TERM_TRIGGER_FALLING) != 0) {
		trig = (GPIO_Trigger)(trig | GPIO_TRIGGER_FALLING);
	}

	return trig;
}

rs_ret_t rs_ftdi_gpio::get(rs_bool_t *val) {
	BOOL lval;

	RET_CONV(FT4222_GPIO_Read(*handle, port, &lval), FT4222_OK, RS_ECOM);
	*val = lval == TRUE ? RS_TRUE : RS_FALSE;

	return RS_OK;
}

rs_ret_t rs_ftdi_gpio::set(rs_bool_t val) {
	RET_CONV(FT4222_GPIO_Write(*handle, port, val == RS_TRUE ? TRUE : FALSE), FT4222_OK, RS_ECOM);

	return RS_OK;
}

rs_ret_t rs_ftdi_gpio::set_trigger(uint32_t trigger) {
	RET_CONV(FT4222_GPIO_SetInputTrigger(*handle, port, convert_trigger(trigger)), FT4222_OK, RS_ECOM);

	return RS_OK;
}

rs_ret_t rs_ftdi_gpio::get_trigger(uint32_t trigger, rs_bool_t *latest_val, rs_bool_t *triggered) {
	uint16 tsize = 0;
	rs_ret_t ret = RS_OK;
	GPIO_Trigger trig = convert_trigger(trigger);

	RET_CONV(FT4222_GPIO_GetTriggerStatus(*handle, port, &tsize), FT4222_OK, RS_ECOM);

	if (tsize == 0) {
		return RS_OK;
	}

	{
		int num = 0;
		uint16 rsize = 0;
		GPIO_Trigger *list_trig = (GPIO_Trigger *) calloc(tsize, sizeof(GPIO_Trigger));

		if(list_trig != NULL) {
			FT4222_STATUS fret;
			fret = FT4222_GPIO_ReadTriggerQueue(*handle, port, list_trig, tsize, &rsize);

			if(fret == FT4222_OK) {
				for(uint16_t i=0; i<tsize; i++) {
					if(list_trig[i] == GPIO_TRIGGER_RISING) {
						*latest_val = RS_TRUE;
					} else if(list_trig[i] == GPIO_TRIGGER_FALLING) {
						*latest_val = RS_FALSE;
					}
					if((list_trig[i] & trig) != 0) {
						num++;
					}
				}
			} else {
				ret = RS_ECOM;
			}
		}
		free(list_trig);
		if (num > 0) *triggered = RS_TRUE;
		else *triggered = RS_FALSE;
	}

	return ret;
}

rs_ret_t rs_ftdi_gpio::clear_trigger(void) {
	uint32_t trigger = 0;
	rs_bool_t val = RS_FALSE;
	rs_bool_t triggered = RS_FALSE;
	RET_ORG(get_trigger(trigger, &val, &triggered), RS_OK);

	return RS_OK;
}

rs_ret_t rs_ftdi_gpio::wait_trigger(uint32_t timeout /*ms */, uint32_t trigger, rs_bool_t *val) {
#ifdef WIN32
	const DWORD start_time = timeGetTime();
	const DWORD timeout_time = start_time + timeout;
#else
	struct timespec start_time;
	struct timespec timeout_time;
	timespec_get(&start_time, TIME_UTC);
	timeout_time.tv_nsec = start_time.tv_nsec + (timeout % 1000) * 1000 * 1000;
	timeout_time.tv_sec = start_time.tv_sec + (timeout / 1000) + timeout_time.tv_nsec / (1000 * 1000 * 1000);
	timeout_time.tv_nsec %= (1000 * 1000 * 1000);
#endif
	rs_bool_t triggered = RS_FALSE;
	*val = RS_FALSE;
	for(;;) {
#ifdef WIN32
#else
		pthread_mutex_lock(&(gpio_event->eMutex));
#endif
		RET_ORG(get_trigger(trigger, val, &triggered), RS_OK);

		if(triggered == RS_TRUE) {
#ifdef WIN32
#else
			pthread_mutex_unlock(&(gpio_event->eMutex));
#endif
			break;
		}

#ifdef WIN32
		{
			const DWORD ret = WaitForSingleObject(*gpio_event, timeout_time - timeGetTime());
			if(ret == WAIT_TIMEOUT) {
				return RS_ETOUT;
			}
			if(ret != WAIT_OBJECT_0) {
				return RS_ECOM;
			}
		}
#else
		{
			const int ret = pthread_cond_timedwait(&(gpio_event->eCondVar), &(gpio_event->eMutex), &timeout_time);
			pthread_mutex_unlock(&(gpio_event->eMutex));
			if(ret == ETIMEDOUT) {
				return RS_ETOUT;
			}
			if(ret != 0) {
				return RS_ECOM;
			}
		}
#endif
	}

#ifdef WIN32
#else
	{
		struct timespec cur_time;
		timespec_get(&cur_time, TIME_UTC);
		time_t pass = (cur_time.tv_sec - start_time.tv_sec) * 1000;
		pass += (cur_time.tv_nsec - start_time.tv_nsec) / (1000 * 1000);
	}
#endif

	return RS_OK;
}
