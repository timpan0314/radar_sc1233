/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs_ctl_dev_evk.cpp
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

#include "rs24g_types.h"
#include "rs_macro.h"
#include "rs_ctl_dev.h"
#include "rs_ctl_dev_evk.h"

RS_IF rs_ret_t rs_dev_open(rs_ctl_dev_t *dev) {
	RET_CHECK(dev != NULL, RS_EFAULT);

	*dev = malloc(sizeof(struct _rs_ctl_dev_evk_t));

	RET_CHECK(*dev != NULL, RS_ENOMEM);

	evkdev(*dev)->spi = new rs_ftdi_spi();
	evkdev(*dev)->term = new rs_ftdi_term(GPIO_PORT_NRST, GPIO_PORT_CE, GPIO_PORT_OR, GPIO_PORT_DETOUT);

	evkdev(*dev)->spi->open(DEVNAME_SPI);
	evkdev(*dev)->term->open(DEVNAME_GPIO);
	return RS_OK;
}

RS_IF rs_ret_t rs_dev_close(rs_ctl_dev_t dev) {
	RET_CHECK(dev != NULL, RS_EINV);

	evkdev(dev)->spi->close();
	evkdev(dev)->term->close();

	delete evkdev(dev)->spi;
	delete evkdev(dev)->term;

	free(dev);
	return RS_OK;
}

RS_IF rs_ret_t rs_dev_write(rs_ctl_dev_t dev, const uint8_t *wdata, size_t size) {
	return evkdev(dev)->spi->write(wdata, size, RS_TRUE);
}

RS_IF rs_ret_t rs_dev_write_low_memory(rs_ctl_dev_t dev, const uint8_t *hdata, size_t hsize, const uint8_t *wdata, size_t wsize) {
	return evkdev(dev)->spi->write_low_memory(hdata, hsize, wdata, wsize);
}

RS_IF rs_ret_t rs_dev_read(rs_ctl_dev_t dev, const uint8_t *wdata, size_t wsize, uint8_t *rdata, size_t rsize) {
	return evkdev(dev)->spi->read(wdata, wsize, rdata, rsize);
}

RS_IF rs_ret_t rs_dev_spi_read(rs_ctl_dev_t dev, const uint8_t *wdata, uint8_t *rdata, size_t size) {
	return evkdev(dev)->spi->spi_read(wdata, rdata, size);
}

RS_IF rs_ret_t rs_dev_term_get(rs_ctl_dev_t dev, uint32_t term, rs_bool_t *val) {
	return evkdev(dev)->term->get(term, val);
}

RS_IF rs_ret_t rs_dev_term_set(rs_ctl_dev_t dev, uint32_t term, rs_bool_t val) {
	return evkdev(dev)->term->set(term, val);
}

RS_IF rs_ret_t rs_dev_term_set_trigger(rs_ctl_dev_t dev, uint32_t term, uint32_t trigger) {
	return evkdev(dev)->term->set_trigger(term, trigger);
}

RS_IF rs_ret_t rs_dev_term_clear_trigger(rs_ctl_dev_t dev, uint32_t term) {
	return evkdev(dev)->term->clear_trigger(term);
}

RS_IF rs_ret_t rs_dev_term_wait_trigger(rs_ctl_dev_t dev, uint32_t term, uint32_t timeout /*ms */, uint32_t trigger, rs_bool_t *val) {
	return evkdev(dev)->term->wait_trigger(term, timeout, trigger, val);
}
