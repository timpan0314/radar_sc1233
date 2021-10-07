/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * location.c
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "rs24g_ctl_sc1233.h"
#include "rs24g_2d_sc1233.h"
#include "rs24g_2d_sc1233_local.h"

#include "rs_list.h"
#include "common_2d.h"
#include "rs_2d_handle_t.h"
#include "location.h"
#include "location_debug.h"


static const struct noise_floor_point nf_220us_hpf1_point[] = {
	{    0.0, 18.4},
	{  200.0,  9.5},
	{  400.0,  5.8},
	{ 1200.0,  2.5},
	{ 2000.0,  1.3},
	{ 3000.0,  0.0},
};
static const struct noise_floor_point nf_220us_hpf2_point[] = {
	{    0.0, 14.7},
	{  200.0,  9.3},
	{  400.0,  5.9},
	{ 1200.0,  2.1},
	{ 2000.0,  1.3},
	{ 3000.0,  0.0},
};
static const struct noise_floor_point nf_1100us_hpf1_point[] = {
	{    0.0, 5.5},
	{  200.0, 4.5},
	{  400.0, 3.5},
	{ 1200.0, 1.6},
	{ 2000.0, 0.6},
	{ 3000.0, 0.0},
};
static const struct noise_floor_point nf_1100us_hpf2_point[] = {
	{    0.0, 4.4},
	{  200.0, 4.4},
	{  400.0, 4.3},
	{ 1200.0, 1.7},
	{ 2000.0, 0.5},
	{ 3000.0, 0.0},
};

const struct noise_floor_table nf_220us_hpf1 = {
	nf_220us_hpf1_point,
	RS_2D_ARRAY_SIZE(nf_220us_hpf1_point),
};
const struct noise_floor_table nf_220us_hpf2 = {
	nf_220us_hpf2_point,
	RS_2D_ARRAY_SIZE(nf_220us_hpf2_point),
};
const struct noise_floor_table nf_1100us_hpf1 = {
	nf_1100us_hpf1_point,
	RS_2D_ARRAY_SIZE(nf_1100us_hpf1_point),
};
const struct noise_floor_table nf_1100us_hpf2 = {
	nf_1100us_hpf2_point,
	RS_2D_ARRAY_SIZE(nf_1100us_hpf2_point),
};

static rs_2d_fp_t get_level(rs_2d_fp_t inlev, rs_2d_fp_t indist, const struct noise_floor_table *nf)
{
	rs_size_t i;
	rs_2d_fp_t noise_level = 0.0;
	rs_2d_fp_t level = 0.0;

//	rs24g2d("indist:%f inlev:%f", indist, inlev);

	if(nf == NULL) {
		noise_level = 0.0;
	} else {
		for(i=0; i<nf->size-1; i++) {
			const rs_2d_fp_t d1 = nf->point[i].distance;
			const rs_2d_fp_t d2 = nf->point[i+1].distance;
			if((indist >= d1) && (indist <= d2)) {
				//rs24g2d("in! %f %f", d1, d2);
				const rs_2d_fp_t l1 = nf->point[i].level;
				const rs_2d_fp_t l2 = nf->point[i+1].level;
				const rs_2d_fp_t a = (l1 - l2) / (d1 - d2);
				const rs_2d_fp_t b = l1 - d1 * a;
				noise_level = a * indist + b;

				//rs24g2d("lev = %f", lev);
				break;
			}
		}
	}

	level = inlev - noise_level;

//	rs24g2d("out:%7.2f in:%7.2f", level, inlev);
	return level;
}

static void init_eloc_score(struct score_eloc *score)
{
	score->prob = 0.0;
	score->delta_distance = 0.0;
}

static int32_t compare_eloc_score(const struct score_eloc *score1, const struct score_eloc *score2)
{
	if(score1->prob > score2->prob) {
		return -1;
	} else if(score1->prob < score2->prob) {
		return 1;
	}

	if(score1->delta_distance > score2->delta_distance) {
		return -1;
	} else if(score1->delta_distance < score2->delta_distance) {
		return 1;
	}

	return 0;
}

static void calc_eloc_score(rs_2d_handle_t handle, struct score_eloc *score, struct loc_est *eloc, rs_2d_fp_t distance)
{
	struct rs_2d_handle *lh = (struct rs_2d_handle *) handle;

	const rs_2d_fp_t eloc_dist = eloc->ave.nc_distance;
	const rs_2d_fp_t delta = (rs_2d_fp_t) fabs(eloc_dist - distance);

	init_eloc_score(score);

	if(delta < lh->dref) {
		score->prob = eloc->ave.all_input;
		score->delta_distance = lh->dref - delta;
	}
}

static void update_eloc_info(rs_2d_handle_t handle)
{
	struct rs_2d_handle *lh = (struct rs_2d_handle *) handle;

	struct loc_est *eloc = NULL;
	rs_size_t i;
	
	for(i=0; rs_list_get_ele(&(lh->data.loc_est.list), i, (void **) &eloc); i++) {
		eloc->num.all = rs_list_get_num(&(eloc->loc_dev.list));
		eloc->rank = i;

		{
			const rs_size_t num_dmax_input = RS_MIN(eloc->num.all, lh->dmax);
			const rs_size_t num_na_distance = RS_MIN(eloc->num.all, RS_MIN(lh->dmax, lh->algo_param.ntap_ave_distance));
			const rs_size_t num_na_angle = RS_MIN(eloc->num.all, RS_MIN(lh->dmax, lh->algo_param.ntap_ave_angle));
			const rs_size_t num_na_peak_level = RS_MIN(eloc->num.all, RS_MIN(lh->dmax, lh->algo_param.ntap_ave_peak_level));
			const rs_size_t num_nc_distance = RS_MIN(eloc->num.all, lh->nc);
			const rs_size_t num_dmax_angle = RS_MIN(eloc->num.all, lh->dmax);

			struct loc_dev *dloc = NULL;
			rs_size_t j;

			eloc->ave.all_input = 0.0;
			eloc->ave.na_distance = 0.0;
			eloc->ave.na_peak_level = 0.0;
			eloc->ave.nc_distance = 0.0;
			eloc->ave.dmax_angle = 0.0;
			
			for(j=0; rs_list_get_ele(&(eloc->loc_dev.list), j, (void **) &dloc); j++) {
				const rs_size_t ridx = eloc->num.all - 1 - j;

				if(num_dmax_input > ridx) {
					eloc->ave.all_input += ((rs_2d_fp_t) dloc->input);
				}
				if(num_na_distance > ridx) {
					eloc->ave.na_distance += ((rs_2d_fp_t) dloc->distance);
				}
				if(num_na_peak_level > ridx) {
					eloc->ave.na_peak_level += ((rs_2d_fp_t) dloc->peak_level);
				}
				if(num_nc_distance > ridx) {
					eloc->ave.nc_distance += ((rs_2d_fp_t) dloc->distance);
				}
				if (num_dmax_angle > ridx) {
					eloc->ave.dmax_angle += ((rs_2d_fp_t)dloc->angle);
				}
			}

			eloc->ave.all_input /= lh->dmax;
			eloc->ave.na_distance /= num_na_distance;
			eloc->ave.na_peak_level /= num_na_peak_level;
			eloc->ave.nc_distance /= num_nc_distance;
			eloc->ave.na_angle = dloc->angle;
			eloc->ave.dmax_angle /= num_dmax_angle;
			eloc->num.na_angle = num_na_angle;
			eloc->num.dmax_angle = num_dmax_angle;
			
		}
	}
}

static rs_bool_t add_loc_dev(rs_2d_handle_t handle, struct loc_est *eloc, int16_t distance, uint8_t peak_level, rs_2d_fp_t angle, int32_t phase_diff_real, int32_t phase_diff_imag, rs_bool_t input)
{
//	struct rs_2d_handle *lh = (struct rs_2d_handle *) handle;

	// if loc_dev is full, then delete the oldest value
	if(rs_list_get_num(&(eloc->loc_dev.list)) >= MAX_LOC_DEV) {
		RET_ORG(rs_bool_t, rs_list_pop_front(&(eloc->loc_dev.list)), RS_TRUE);
	}

	// add loc_dev
	{
		struct loc_dev *dloc = NULL;

		RET_ORG(rs_bool_t, rs_list_push_back(&(eloc->loc_dev.list)), RS_TRUE);
		RET_ORG(rs_bool_t, rs_list_get_ele_end(&(eloc->loc_dev.list), (void **) &dloc), RS_TRUE);

		dloc->input = input;
		dloc->distance = distance;
		dloc->peak_level = peak_level;
		dloc->angle = angle;
		dloc->phase_diff_real = phase_diff_real;
		dloc->phase_diff_imag = phase_diff_imag;
	}

	return RS_TRUE;
}

static __inline int compare_fp(rs_2d_fp_t fp1, rs_2d_fp_t fp2)
{
	if(fp1 > fp2) {
		return -1;
	}
	if(fp1 < fp2) {
		return 1;
	}

	return 0;
}

#ifdef WIN32
int __cdecl compare_sort_eloc(void *arg, const void *eloc1, const void *eloc2)
#else	/* WIN32 */
int compare_sort_eloc(const void *eloc1, const void *eloc2, void *arg)
#endif	/* WIN32 */
{
	struct rs_2d_handle *lh = (struct rs_2d_handle *) arg;

	const struct loc_est *leloc1 = (const struct loc_est *) *((void **) eloc1);
	const struct loc_est *leloc2 = (const struct loc_est *) *((void **) eloc2);

	const rs_2d_fp_t prob1 = leloc1->ave.all_input;
	const rs_2d_fp_t prob2 = leloc2->ave.all_input;

	const rs_2d_fp_t prob_c1 = (rs_2d_fp_t) (prob1 >= lh->target_threshold_prob ? 1.0 : prob1);
	const rs_2d_fp_t prob_c2 = (rs_2d_fp_t) (prob2 >= lh->target_threshold_prob ? 1.0 : prob2);

	int cmp = 0;

	if(prob_c1 != prob_c2) {
		cmp = compare_fp(prob1, prob2);
		rs24g2d("compare by prob!: %6.2f %6.2f (%d) <%6.2f %6.2f>", prob1, prob2, cmp, prob_c1, prob_c2);
	}

	if(cmp == 0) {
		if(lh->algo_param.sort_order == RS_2D_SORT_ORDER_PEAK_LEVEL) {
			rs_2d_fp_t level1 = get_level(leloc1->ave.na_peak_level, leloc1->ave.na_distance, lh->noise_floor);
			rs_2d_fp_t level2 = get_level(leloc2->ave.na_peak_level, leloc2->ave.na_distance, lh->noise_floor);

			if (leloc1->rank == 0){
				level1 += lh->la;
			}
			else if (leloc2->rank == 0){
				level2 += lh->la;
			}

			cmp = compare_fp(level1, level2);

			rs24g2d("compare by level!: %6.2f %6.2f (%d)", level1, level2, cmp);
		} else {
			const rs_2d_fp_t distance1 = leloc1->ave.na_distance;
			const rs_2d_fp_t distance2 = leloc2->ave.na_distance;

			cmp = compare_fp(distance2, distance1);

			rs24g2d("compare by distance!: %6.2f %6.2f (%d)", distance1, distance2, cmp);
		}
	}

	if(cmp == 0) {
		cmp = leloc1->rank - leloc2->rank;

		rs24g2d("compare by rank!: %u %u (%d)", leloc1->rank, leloc2->rank, cmp);
	}

	return cmp;
}

static rs_bool_t search_eloc(rs_2d_handle_t handle, struct loc_est **eloc, int16_t distance, rs_bool_t is_angle_valid)
{
	struct rs_2d_handle *lh = (struct rs_2d_handle *) handle;

	struct score_eloc high_score;
	struct loc_est *leloc = NULL;
	rs_size_t i;

	init_eloc_score(&high_score);

	/* search similar loc_est */
	for(i=0; rs_list_get_ele(&(lh->data.loc_est.list), i, (void **) &leloc); i++) {
		struct score_eloc score;

		if(leloc->flag_add_loc_dev == RS_TRUE) {
			continue;
		}

		calc_eloc_score(handle, &score, leloc, distance);
		if(compare_eloc_score(&high_score, &score) > 0) {
			high_score = score;
			*eloc = leloc;
		}
	}

	/* if no similar loc_est, then create new loc_est */
	if(*eloc == NULL && is_angle_valid) {
		if(rs_list_push_back(&(lh->data.loc_est.list))) {
			RET_ORG(rs_bool_t, rs_list_get_ele_end(&(lh->data.loc_est.list), (void **) eloc), RS_TRUE);
			rs_list_clear(&((*eloc)->loc_dev.list));
		}
	}

	return RS_TRUE;
}

rs_ret_t loc_input(rs_2d_handle_t handle, const struct rs_2rx_peak_data *data)
{
	struct rs_2d_handle *lh = (struct rs_2d_handle *) handle;

	struct loc_est *eloc = NULL;
	struct loc_dev *dloc = NULL;
	rs_size_t i;

	for(i=0; rs_list_get_ele(&(lh->data.loc_est.list), i, (void **) &eloc); i++) {
		eloc->flag_add_loc_dev = RS_FALSE;
	}

	for(i=0; i<RS_2D_ARRAY_SIZE(data->distance.distance); i++) {
		const int16_t distance = data->distance.distance[i];
		const uint8_t peak_level = data->distance.peak_level[i];
		int32_t phase_diff_real = 0;
		int32_t phase_diff_imag = 0;
		rs_2d_fp_t angle = 0.0;

		/* if distance is invalid, then do not store value */
		if((distance <= RS_2D_DISTANCE_MIN) || (distance > RS_2D_DISTANCE_MAX)) {
			rs24g2d("invalid distance: distance=%d", distance);
			continue;
		}

		rs_bool_t is_angle_valid;
		is_angle_valid = calc_phase_diff(data->distance.peak_real[i], data->distance.peak_imag[i], data->peak_rx2_real[i], data->peak_rx2_imag[i], &phase_diff_real, &phase_diff_imag)
			&& calc_angle(phase_diff_real, phase_diff_imag, &angle);

		/* if angle is invalid */
		if(!is_angle_valid) {
			rs24g2d("invalid angle");
			phase_diff_real = 0;
			phase_diff_imag = 0;
		}

		eloc = NULL;
		RET_CONV(rs_ret_t, search_eloc(handle, &eloc, distance, is_angle_valid), RS_TRUE, RS_EINV);
		
		if (eloc == NULL){
			continue;
		}

		int32_t sum_phase_diff_real = phase_diff_real;
		int32_t sum_phase_diff_imag = phase_diff_imag;
		const rs_size_t sum_num = RS_MIN(lh->dmax, lh->algo_param.ntap_ave_angle) - 1;
		const rs_size_t num_na_angle = RS_MIN(eloc->num.all, sum_num);
		for (rs_size_t j = 0; rs_list_get_ele(&(eloc->loc_dev.list), j, (void **)&dloc); j++) {
			const rs_size_t ridx = eloc->num.all - 1 - j;
			if (num_na_angle > ridx) {
				sum_phase_diff_real += dloc->phase_diff_real;
				sum_phase_diff_imag += dloc->phase_diff_imag;
			}
		}
		
		if (!calc_angle(sum_phase_diff_real, sum_phase_diff_imag, &angle)){
			rs_list_get_ele_end(&(eloc->loc_dev.list), (void **)&dloc);
			angle = dloc->angle;
		}
		
		eloc->flag_add_loc_dev = RS_TRUE;
		add_loc_dev(handle, eloc, distance, peak_level, angle, phase_diff_real, phase_diff_imag, RS_TRUE);
	}

	// if value is not updated, then use(copy) last value
	for(i=0; rs_list_get_ele(&(lh->data.loc_est.list), i, (void **) &eloc); i++) {
		if(eloc->flag_add_loc_dev != RS_TRUE) {
			rs_list_get_ele_end(&(eloc->loc_dev.list), (void **) &dloc);
			add_loc_dev(handle, eloc, dloc->distance, dloc->peak_level, dloc->angle, dloc->phase_diff_real, dloc->phase_diff_imag, RS_FALSE);
		}
	}

	// update info of loc_est
	update_eloc_info(handle);

	// if all_input is 0, then delete loc_est
	for(i=0; rs_list_get_ele(&(lh->data.loc_est.list), i, (void **) &eloc); i++) {
		if(eloc->ave.all_input <= 0.001) {
			rs_list_delete(&(lh->data.loc_est.list), i);
			i--;
		}
	}

	// sort loc_est
	rs_list_sort(&(lh->data.loc_est.list), compare_sort_eloc, lh);

//	loc_debug_dump(&(lh->data));

	return RS_OK;
}

static __inline rs_2d_fp_t calc_angle_variance(rs_2d_handle_t handle, struct loc_est *eloc)
{
	struct rs_2d_handle *lh = (struct rs_2d_handle *) handle;
	struct loc_dev *dloc = NULL;
	rs_size_t i;

	rs_2d_fp_t var_angle = 0.0;

	const rs_size_t num_dmax_angle = RS_MIN(eloc->num.all, lh->dmax);
	for(i=0; rs_list_get_ele(&(eloc->loc_dev.list), i, (void **) &dloc); i++) {
		const rs_size_t ridx = eloc->num.all - 1 - i;
		if (num_dmax_angle > ridx){
			var_angle += (rs_2d_fp_t)(pow((dloc->angle - eloc->ave.dmax_angle), 2.0) / num_dmax_angle);
		}
	}

//	rs24g2d("var angle:%f", var_angle);

	return var_angle;
}

rs_ret_t loc_get(rs_2d_handle_t handle, rs_size_t max_rank, struct rs_2d_location_data *data)
{
	struct rs_2d_handle *lh = (struct rs_2d_handle *) handle;
	struct loc_est *eloc = NULL;
	rs_size_t i;
	

	RET_CHECK(max_rank <= RS_2D_MAXRANK, RS_EINV);

	data->number = 0;

	for(i=0; i<RS_2D_MAXRANK; i++) {
		data->angle[i] = RS_2D_ANGLE_INVALID;
		data->distance[i] = RS_2D_DISTANCE_INVALID;
		data->peak_level[i] = RS_2D_PEAK_LEVEL_INVALID;
	}

	for(i=0; i<max_rank; i++) {
		if(rs_list_get_ele(&(lh->data.loc_est.list), i, (void **) &eloc)) {
			if (eloc->ave.all_input < lh->noise_threshold_prob){
				continue;
			}

			if ((calc_angle_variance(lh, eloc) < lh->algo_param.angle_threshold) &&
				(((int32_t)eloc->ave.na_angle >= lh->algo_param.lower_angle) &&
				((int32_t)eloc->ave.na_angle <= lh->algo_param.upper_angle))) {
				data->angle[i] = (int32_t) eloc->ave.na_angle;
			}

			data->number = i + 1;
			data->distance[i] = (int32_t) eloc->ave.na_distance;
			data->peak_level[i] = (uint32_t) eloc->ave.na_peak_level;

//			rs24g2d("angle:%3d distance:%4d peak_level:%3d", data->angle, data->distance, data->peak_level);
		}
	}

	return RS_OK;
}
