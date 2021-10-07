/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_mutex.h
 */

#ifndef __RS24G_MUTEX_H__
#define __RS24G_MUTEX_H__

#include "rs24g_types.h"
#include "rs_mutex.h"

/*
 * Type Definition
 */
typedef rs_mutex_t								rs24g_mutex_t;

/*
 * Constant Definition
 */

/*
 * Struct Definition
 */

/*
 * Function Definition
 */
RS_INLINE rs_ret_t rs24g_mutex_init(rs24g_mutex_t *mutex)
{
	int ret = rs_mutex_init(mutex);
	return (ret == 0 ? RS_OK : (ret == -2 ? RS_ENOMEM : RS_EFAULT));
}
RS_INLINE rs_ret_t rs24g_mutex_destroy(rs24g_mutex_t mutex)
{
	return (rs_mutex_destroy(mutex) == 0 ? RS_OK : RS_EFAULT);
}

RS_INLINE rs_ret_t rs24g_mutex_lock(rs24g_mutex_t mutex)
{
	return (rs_mutex_lock(mutex) == 0 ? RS_OK : RS_EFAULT);
}
RS_INLINE rs_ret_t rs24g_mutex_unlock(rs24g_mutex_t mutex)
{
	return (rs_mutex_unlock(mutex) == 0 ? RS_OK : RS_EFAULT);
}

#endif	/* __RS24G_MUTEX_H__ */
