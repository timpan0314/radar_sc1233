 /*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs_mutex.cpp
 */

#include <stdio.h>
#if defined(__MBED__)
#include <mbed.h>
#else
//#include <mutex>
#endif

#include "rs_macro.h"
#include "rs_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

struct rs_mutex
{

};

int rs_mutex_init(rs_mutex_t *pmutex)
{
	

	return 0;
}

int rs_mutex_destroy(rs_mutex_t mutex)
{
	

	return 0;
}

int rs_mutex_lock(rs_mutex_t mutex)
{
	

	return 0;
}

int rs_mutex_unlock(rs_mutex_t mutex)
{
	

	return 0;
}

#ifdef __cplusplus
}
#endif
