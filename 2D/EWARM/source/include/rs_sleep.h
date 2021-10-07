/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file rs_sleep.h
 */

#ifndef __RS_SLEEP_H__
#define __RS_SLEEP_H__

#include <stdint.h>

/*
 * Type Definition
 */

/*
 * Constant Definition
 */

/*
 * Struct Definition
 */

/*
 * Function Definition
 */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * sleep @a usec microseconds.
 *
 * @param[in] usec sleep time in microseconds
 */
void rs_usleep(uint32_t usec);

#ifdef __cplusplus
}
#endif

#endif	/* __RS_SLEEP_H__ */
