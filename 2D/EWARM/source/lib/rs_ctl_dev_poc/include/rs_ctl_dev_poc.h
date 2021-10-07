/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs_ctl_dev_poc.h
 */

#ifndef __RS_CTL_DEV_POC_H__
#define __RS_CTL_DEV_POC_H__

#include <stdint.h>

#include "rs24g_types.h"
#include "rs_ctl_dev.h"
#if defined(RS_CPUIF_USE_I2C)
#include "rs_i2c.h"
#else
#include "rs_spi.h"
#endif
#include "rs_term.h"

//#define RS_CTL_DEV_POC_DEBUG_PRINT

#ifdef	RS_CTL_DEV_POC_DEBUG_PRINT
#define devpoc_dp(fmt, ...)				{fprintf(stdout, "[devpoc] %s:%d: " fmt "\n", __FUNCTION__, __LINE__, ## __VA_ARGS__); fflush(stdout);}
#else	/* RS_CTL_DEV_POC_DEBUG_PRINT */
#define devpoc_dp(...)
#endif	/* RS_CTL_DEV_POC_DEBUG_PRINT */


/*
 * Type Definition
 */

/*
 * Constant Definition
 */

/*
 * Struct Definition
 */
struct _rs_ctl_dev_poc_t
{
#if defined(RS_CPUIF_USE_I2C)
	rs_i2c *spi;
#else
	rs_spi *spi;
#endif
	rs_term *term;
};

/*
 * Function Definition
 */
#define pocdev(a)								((struct _rs_ctl_dev_poc_t *) (a))

#endif	/* __RS_CTL_DEV_POC_H__ */
