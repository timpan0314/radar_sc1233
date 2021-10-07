/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file rs_mutex.h
 */

#ifndef __RS_MUTEX_H__
#define __RS_MUTEX_H__

/*
 * Type Definition
 */
typedef struct rs_mutex *							rs_mutex_t;

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

int rs_mutex_init(rs_mutex_t *pmutex);
int rs_mutex_destroy(rs_mutex_t mutex);

int rs_mutex_lock(rs_mutex_t mutex);
int rs_mutex_unlock(rs_mutex_t mutex);

#ifdef __cplusplus
}
#endif

#endif	/* __RS_MUTEX_H__ */
