/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * location_debug.c
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "rs24g_types.h"

#include "rs_list.h"
#include "common_2d.h"
#include "rs_2d_handle_t.h"
#include "location.h"
#include "location_debug.h"


void loc_debug_dump(struct loc_rank *data)
{
	rs_size_t i;
	rs_size_t j;
	struct loc_est *eloc = NULL;
	struct loc_dev *dloc = NULL;
	static uint32_t no = 0;

	no++;

	printf("********** loc_est dump [no:%04d] **********\n", no);

	for(i=0; rs_list_get_ele(&(data->loc_est.list), i, (void **) &eloc); i++) {
		printf(
			"  %d(%2d): %6.2f %6.2f angle:%7.2f input:%6.1f%% [",
			i,
			eloc->num.all,
			eloc->ave.na_distance,
			eloc->ave.na_peak_level,
			eloc->ave.na_angle,
			eloc->ave.all_input * 100.0
		);

		for(j=0; rs_list_get_ele(&(eloc->loc_dev.list), j, (void **) &dloc); j++) {
			if(j > eloc->num.all) {
				continue;
			}
			if(j == eloc->num.all) {
				printf("...");
				break;
			}
			printf(" %4d", dloc->distance);
		}

		printf(" ]\n");
	}
	printf("********** loc_est dump [no:%04d] **********\n", no);
}

