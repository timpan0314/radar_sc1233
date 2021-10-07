/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs_ctl_dev_evk.h
 */

#ifndef __RS_CTL_DEV_EVK_H__
#define __RS_CTL_DEV_EVK_H__

#include <stdint.h>

#include "rs24g_types.h"
#include "rs_ftdi.h"

//#define RS_CTL_DEV_EVK_DEBUG_PRINT

#ifdef	RS_CTL_DEV_EVK_DEBUG_PRINT
#define devevk_dp(fmt, ...)				{fprintf(stdout, "[devevk] %s:%d: " fmt "\n", __FUNCTION__, __LINE__, ## __VA_ARGS__); fflush(stdout);}
#else	/* RS_CTL_DEV_EVK_DEBUG_PRINT */
#define devevk_dp(...)
#endif	/* RS_CTL_DEV_EVK_DEBUG_PRINT */


/*
 * Type Definition
 */

/*
 * Constant Definition
 */
#define TRIGGER_OR								((GPIO_Trigger) GPIO_TRIGGER_RISING)
#define TRIGGER_DETOUT							((GPIO_Trigger) (GPIO_TRIGGER_RISING | GPIO_TRIGGER_FALLING))

#define GPIO_PORT_NRST							(GPIO_PORT1)
#define GPIO_PORT_CE							(GPIO_PORT0)
#define GPIO_PORT_OR							(GPIO_PORT3)
#define GPIO_PORT_DETOUT						(GPIO_PORT2)

#define DEVNAME_SPI								((PVOID) "FT4222 A")
#define DEVNAME_GPIO							((PVOID) "FT4222 B")

/*
 * Struct Definition
 */
struct _rs_ctl_dev_evk_t
{
	rs_ftdi_spi *spi;
	rs_ftdi_term *term;
};

/*
 * Function Definition
 */
#define evkdev(a)								((struct _rs_ctl_dev_evk_t *) (a))

#endif	/* __RS_CTL_DEV_EVK_H__ */
