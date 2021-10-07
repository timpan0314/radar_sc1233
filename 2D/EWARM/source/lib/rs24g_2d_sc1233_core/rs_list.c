/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs_list.c
 */

#ifndef WIN32
#define _GNU_SOURCE								// for qsort_r()
#endif	/* WIN32 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "rs24g_types.h"
#include "rs_list.h"


//#define rs24g2dlist(fmt, ...)					{fprintf(stdout, "[rs24g2dlist] %s:%d: " fmt "\n", __FUNCTION__, __LINE__, ## __VA_ARGS__); fflush(stdout);}
#define rs24g2dlist(...)


static __inline rs_bool_t check_idx(struct rs_list_ctl *ctl, rs_size_t idx)
{
	if(idx >= ctl->total_num) {
		return RS_FALSE;
	}

	return RS_TRUE;
}

static __inline rs_bool_t check_idx_data(struct rs_list_ctl *ctl, rs_size_t idx)
{
	if(idx >= ctl->data_num) {
		return RS_FALSE;
	}

	return RS_TRUE;
}

static __inline rs_bool_t swap_ele(struct rs_list_ctl *ctl, rs_size_t idx1, rs_size_t idx2)
{
	if(!check_idx_data(ctl, idx1)) {
		return RS_FALSE;
	}
	if(!check_idx_data(ctl, idx2)) {
		return RS_FALSE;
	}

	rs_list_ele_t tmp = ctl->ele[idx1];
	ctl->ele[idx1] = ctl->ele[idx2];
	ctl->ele[idx2] = tmp;

	return RS_TRUE;
}


void rs_list_clear(struct rs_list_ctl *ctl)
{
	ctl->data_num = 0;
}

rs_bool_t rs_list_pop_front(struct rs_list_ctl *ctl)
{
	return rs_list_delete(ctl, 0);
}

rs_bool_t rs_list_push_back(struct rs_list_ctl *ctl)
{
	if(ctl->data_num >= ctl->total_num) {
		return RS_FALSE;
	}

	ctl->data_num++;

	return RS_TRUE;
}

rs_bool_t rs_list_delete(struct rs_list_ctl *ctl, rs_size_t idx)
{
	rs_size_t i;

	if(!check_idx_data(ctl, idx)) {
		return RS_FALSE;
	}

	for(i=idx+1; i<ctl->data_num; i++) {
		if(!swap_ele(ctl, i - 1, i)) {
			return RS_FALSE;
		}
	}

	ctl->data_num--;

	return RS_TRUE;
}

rs_size_t rs_list_get_num(const struct rs_list_ctl *ctl)
{
	return ctl->data_num;
}

rs_bool_t rs_list_get_ele(const struct rs_list_ctl *ctl, rs_size_t idx, rs_list_ele_t *ele)
{
	if(idx >= rs_list_get_num(ctl)) {
		return RS_FALSE;
	}

	*ele = ctl->ele[idx];

	return RS_TRUE;
}

rs_bool_t rs_list_get_ele_end(const struct rs_list_ctl *ctl, rs_list_ele_t *ele)
{
	const rs_size_t size = rs_list_get_num(ctl);

	if(size == 0) {
		return RS_FALSE;
	}

	return rs_list_get_ele(ctl, size - 1, ele);
}

rs_ret_t rs_list_init(struct rs_list_ctl *ctl, rs_size_t ele_num, rs_list_ele_t ele_top, rs_size_t ele_size, void *buf, rs_size_t bufsize)
{
	rs_size_t i;
	uint8_t *ele = (uint8_t *) ele_top;

	if(RS_LIST_CTL_BUFSIZE(ele_num) > bufsize) {
		return RS_ENOMEM;
	}

	ctl->ele = (rs_list_ele_t) buf;
	ctl->data_num = 0;
	ctl->total_num = ele_num;

	for(i=0; i<ele_num; i++) {
		ctl->ele[i] = (rs_list_ele_t) ele;
		ele += ele_size;
	}

	rs24g2dlist("num:%u", ctl->total_num);

	return RS_OK;
}

#ifdef WIN32
#define qsort_func(...) qsort_s(__VA_ARGS__)
#else	/* WIN32 */
#define qsort_func(...) qsort_r(__VA_ARGS__)
#endif	/* WIN32 */

extern void rs_list_sort(struct rs_list_ctl *ctl, qsort_cmp cmp, rs_list_ele_t arg)
{
	qsort_func(ctl->ele, ctl->data_num, sizeof(*(ctl->ele)), cmp, arg);
}
