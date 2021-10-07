/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs_ctl_dev.c
 */

#include <stdint.h>
#include <stdlib.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "rs_ctl_dev.h"
#include "rs_dev.h"
#include "defs/rs_dev_term.h"

RS_IF rs_ret_t rs_ctl_dev_open(rs_ctl_dev_t *dev, const void *attr)
{
	return rs_dev_open(dev);
}

RS_IF rs_ret_t rs_ctl_dev_close(rs_ctl_dev_t dev)
{
	return rs_dev_close(dev);
}

RS_IF rs_ret_t rs_ctl_dev_write(rs_ctl_dev_t dev, const uint8_t *wdata, rs_size_t size)
{
	RS_ASSERT(dev != NULL, RS_EINV);
	RS_ASSERT(wdata != NULL, RS_EFAULT);
	RS_ASSERT(size > 0, RS_EINV);

#ifdef	RS_CTL_DEV_EVK_DEBUG_PRINT
	{
		rs_size_t i;
		printf("%s", __FUNCTION__);
		for(i=0; i<size; i++) {
			if((i % 16) == 0) {
				printf("\n");
			}
			printf("%02x ", wdata[i]);
		}
		printf("\n");
		fflush(stdout);
	}
#endif	/* RS_CTL_DEV_EVK_DEBUG_PRINT */

	return rs_dev_write(dev, wdata, size);
}

RS_IF rs_ret_t rs_ctl_dev_write_low_memory(rs_ctl_dev_t dev, const uint8_t *hdata, rs_size_t hsize, const uint8_t *wdata, rs_size_t wsize)
{
	RS_ASSERT(dev != NULL, RS_EINV);
	RS_ASSERT(wdata != NULL, RS_EFAULT);
	RS_ASSERT(hsize > 0, RS_EINV);

#ifdef	RS_CTL_DEV_EVK_DEBUG_PRINT
	{
		rs_size_t i;
		printf("%s", __FUNCTION__);
		for(i=0; i<size; i++) {
			if((i % 16) == 0) {
				printf("\n");
			}
			printf("%02x ", wdata[i]);
		}
		printf("\n");
		fflush(stdout);
	}
#endif	/* RS_CTL_DEV_EVK_DEBUG_PRINT */

	return rs_dev_write_low_memory(dev, hdata, hsize, wdata, wsize);
}

RS_IF rs_ret_t rs_ctl_dev_read(rs_ctl_dev_t dev, const uint8_t *wdata, rs_size_t wsize, uint8_t *rdata, rs_size_t rsize)
{
	RS_ASSERT(dev != NULL, RS_EINV);
	RS_ASSERT(rdata != NULL, RS_EFAULT);
	RS_ASSERT(wdata != NULL, RS_EFAULT);
	RS_ASSERT(rsize > 0, RS_EINV);
	RS_ASSERT(wsize > 0, RS_EINV);

	RS_CALL(rs_dev_read(dev, wdata, wsize, rdata, rsize));

#ifdef	RS_CTL_DEV_EVK_DEBUG_PRINT
	{
		rs_size_t i;
		printf("%s", __FUNCTION__);
		for(i=0; i<rsize; i++) {
			if((i % 16) == 0) {
				printf("\n");
			}
			printf("%02x ", rdata[i]);
		}
		printf("\n");
		fflush(stdout);
	}
#endif	/* RS_CTL_DEV_EVK_DEBUG_PRINT */

	return RS_OK;
}

RS_IF rs_ret_t rs_ctl_dev_spi_read(rs_ctl_dev_t dev, const uint8_t *wdata, uint8_t *rdata, rs_size_t size)
{
	RS_ASSERT(dev != NULL, RS_EINV);
	RS_ASSERT(rdata != NULL, RS_EFAULT);
	RS_ASSERT(wdata != NULL, RS_EFAULT);
	RS_ASSERT(size > 0, RS_EINV);

	RS_CALL(rs_dev_spi_read(dev, wdata, rdata, size));

#ifdef	RS_CTL_DEV_EVK_DEBUG_PRINT
	{
		rs_size_t i;
		printf("%s", __FUNCTION__);
		for(i=0; i<size; i++) {
			if((i % 16) == 0) {
				printf("\n");
			}
			printf("%02x ", rdata[i]);
		}
		printf("\n");
		fflush(stdout);
	}
#endif	/* RS_CTL_DEV_EVK_DEBUG_PRINT */

	return RS_OK;
}

RS_IF rs_ret_t rs_ctl_dev_frame_read(rs_ctl_dev_t dev, uint8_t *rdata, rs_size_t size)
{
	return RS_ENOSUPPORT;
}

RS_IF rs_ret_t rs_ctl_dev_term_set(rs_ctl_dev_t dev, uint32_t term, rs_bool_t val)
{
	RS_ASSERT(dev != NULL, RS_EINV);
	RS_ASSERT((RS_TERM_TYPE_MASK & term) == RS_TERM_TYPE_IN, RS_EINV);

	RS_CALL(rs_dev_term_set(dev, term, val != RS_FALSE));

	return RS_OK;
}

RS_IF rs_ret_t rs_ctl_dev_term_get(rs_ctl_dev_t dev, uint32_t term, rs_bool_t *val)
{
	RS_ASSERT(dev != NULL, RS_EINV);
	RS_ASSERT(val != NULL, RS_EFAULT);

	RS_CALL(rs_dev_term_get(dev, term, val));

	return RS_OK;
}

RS_IF rs_ret_t rs_ctl_dev_term_trig_clear(rs_ctl_dev_t dev, uint32_t term)
{
	RS_ASSERT(dev != NULL, RS_EINV);
	RS_ASSERT((RS_TERM_TYPE_MASK & term) == RS_TERM_TYPE_OUT, RS_EINV);

	RS_CALL(rs_dev_term_clear_trigger(dev, term));

	return RS_OK;
}

RS_IF rs_ret_t rs_ctl_dev_term_trig_set(rs_ctl_dev_t dev, uint32_t term, uint32_t trigger)
{
	RS_ASSERT(dev != NULL, RS_EINV);
	RS_ASSERT((RS_TERM_TYPE_MASK & term) == RS_TERM_TYPE_OUT, RS_EINV);

	RS_CALL(rs_dev_term_set_trigger(dev, term, trigger));

	return RS_OK;
}

RS_IF rs_ret_t rs_ctl_dev_term_trig_get(rs_ctl_dev_t dev, uint32_t term, uint32_t trigger, rs_bool_t *val)
{
	RS_ASSERT(dev != NULL, RS_EINV);
	RS_ASSERT(val != NULL, RS_EFAULT);
	RS_ASSERT((RS_TERM_TYPE_MASK & term) == RS_TERM_TYPE_OUT, RS_EINV);

	return RS_ENOSUPPORT;
}

RS_IF rs_ret_t rs_ctl_dev_term_trig_wait(rs_ctl_dev_t dev, uint32_t timeout, uint32_t term, uint32_t trigger, rs_bool_t *val)
{
	RS_ASSERT(dev != NULL, RS_EINV);
	RS_ASSERT(val != NULL, RS_EFAULT);
	RS_ASSERT((RS_TERM_TYPE_MASK & term) == RS_TERM_TYPE_OUT, RS_EINV);

	RS_CALL(rs_dev_term_wait_trigger(dev, term, timeout, trigger, val));

	return RS_OK;
}
