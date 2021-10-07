/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs_list.h
 */

#ifndef __RS_LIST_H__
#define __RS_LIST_H__

#include <stdint.h>
#include <stdio.h>

#include "rs24g_types.h"


typedef void *									rs_list_ele_t;

#define RS_LIST_CTL_BUFSIZE(a)					((a) * sizeof(rs_list_ele_t))

#ifdef WIN32
typedef int (__cdecl *qsort_cmp)(void *, const void *, const void *);
#else	/* WIN32 */
typedef int (*qsort_cmp)(const void *, const void *, void *);
#endif	/* WIN32 */


struct rs_list_ctl
{
	rs_list_ele_t								*ele;
	rs_size_t									data_num;
	rs_size_t									total_num;
};

extern rs_ret_t rs_list_init(struct rs_list_ctl *ctl, rs_size_t ele_num, rs_list_ele_t ele_top, rs_size_t ele_size, void *buf, rs_size_t bufsize);

extern void rs_list_clear(struct rs_list_ctl *ctl);
extern rs_bool_t rs_list_pop_front(struct rs_list_ctl *ctl);
extern rs_bool_t rs_list_push_back(struct rs_list_ctl *ctl);
extern rs_bool_t rs_list_delete(struct rs_list_ctl *ctl, rs_size_t idx);

extern rs_size_t rs_list_get_num(const struct rs_list_ctl *ctl);
extern rs_bool_t rs_list_get_ele(const struct rs_list_ctl *ctl, rs_size_t idx, rs_list_ele_t *ele);
extern rs_bool_t rs_list_get_ele_end(const struct rs_list_ctl *ctl, rs_list_ele_t *ele);

extern void rs_list_sort(struct rs_list_ctl *ctl, qsort_cmp cmp, rs_list_ele_t arg);


#endif	/* __RS_LIST_H__ */
