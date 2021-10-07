/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_sample_sc1233.c
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stm32l4xx_hal.h>

#include "rs24g_types.h"
#include "rs24g_ctl_sc1233.h"
#include "rs24g_ctl_setup_sc1233.h"
//#include "rs24g_2d_sc1233.h"   Tim 2D will be added later
static uint32_t motion_cal_sm_data[512];
static int motion_cal_sm_data_num=512;

//#define lprintf(fmt, ...) {fprintf(stdout, fmt, ## __VA_ARGS__); fflush(stdout);}  //Tim 
void lprintf(const char *fmt,...)  {printf(fmt);}     //_Tim
int sc1233_motion_calibration(rs_handle_t handle,struct rs_motion_param *param,int wait_time0, int wait_time1, int wait_time2, int repeat_times){
	int motion_cal_sm_wait_time = 0;
	uint32_t sm_data[512];
       // struct rs_motion_param param =
	if (rs_setup_param(handle, RS_MODE_MOTION_DETECT, param) != RS_OK) {
		printf("rs_setup_param failed\n");
		return -1;
	}
	printf("setup over\n");
        if(rs_setup_smoothed_level(handle, RS_SMOOTHED_LEVEL_UPDATE, 512, 0,
			motion_cal_sm_data, motion_cal_sm_data_num) != RS_OK){
		printf("rs_setup_smoothed_level failed\n");
		return -1;
	}
	printf("rs_setup_smoothed_level over\n");
        

	HAL_Delay(wait_time0 * 1000);

	if (rs_start(handle) != RS_OK){
		printf("rs_start failed\n");
		return -1;
	}
	printf("rs_start over\n");
	memset(motion_cal_sm_data, 0, sizeof(motion_cal_sm_data));
	for (int rep = 0; rep < repeat_times; rep++) {
		printf("repeat times = %d\n", rep);
		if (rep == 0) {
			motion_cal_sm_wait_time = wait_time1;
		}
		else {
			motion_cal_sm_wait_time = wait_time2;
			if (rs_resume(handle) != RS_OK){
				printf("rs_resume failed\n");
				return -1;
			}
		}

		HAL_Delay(motion_cal_sm_wait_time * 1000);

		if (rs_stop(handle) != RS_OK) {
			printf("rs_stop failed\n");
			return -1;
		}
		if(rs_read_smoothed_level(handle, 512, 0, sm_data, motion_cal_sm_data_num) != RS_OK){
			printf("rs_read_smoothed_level failed\n");
			return -1;
		}
		for (int i = 0; i < motion_cal_sm_data_num; i++) {
			motion_cal_sm_data[i] += sm_data[i];
		}
	}
        
        for (int i = 0; i < motion_cal_sm_data_num; i++) {
				motion_cal_sm_data[i] /= repeat_times;
				
			}        //average each bin's level
	

//	internal_flash_write_radar_cal_data(motion_cal_sm_data, motion_cal_sm_data_num);

	


	if(rs_shutdown(handle) != RS_OK){
		return -1;
	}

	return 0;
}

/*
 * sample code of presence detection operation
 */
int presence_mode(rs_handle_t handle)
{
	lprintf("[Presence detection mode]\n");

	/* The Sensor LSI can be set up the sensor setting parameters */
	{
		struct rs_motion_param param;

		/* 
		 * The sensor setting parameters structure variable is set up with their initial values
		 * of the Entry Motion detection mode by calling the API, rs_param_init, specified that mode.
		 */
		{
			rs_ret_t ret;

			ret = rs_param_init(RS_MODE_MOTION_DETECT, &param);
			if(ret != RS_OK) {
				lprintf("rs_param_init: error ret=%d\n", ret);
				return -1;
			}

			lprintf("rs_param_init:<ok>\n");
		}

		/* 
		 * The sensor setting parameters structure variable is updated with the desired sensor
		 * setting parameters.
		 */
		param.rx_gain = 60;
		param.interval = 20;
		param.alpha = 153;
		param.motion_threshold = 400;
		param.startup_count = 20;
		param.lower_distance = 10;
		param.upper_distance = 200;
		param.chirp_time = 1100;
		param.digital_beam_shaper = RS_DIGITAL_BEAM_SHAPER_WIDE;
		param.high_pass_filter = RS_HPF_FIRST_ORDER;
                
                /****calibration*********/
                printf("start calibration...\n");
                if(sc1233_motion_calibration(handle,&param,5,2,1,20)!=0) {
                    return -1;
                }

		/* 
		 * The Sensor LSI is set up with Entry Motion detection mode and sensor setting parameters
		 * by calling the API, rs_setup_param, specified that mode and sensor setting
		 * parameters structure variable.
		 */
		{
			rs_ret_t ret;

			ret = rs_setup_param(handle, RS_MODE_MOTION_DETECT, &param);
			if(ret != RS_OK) {
				lprintf("rs_setup_param: error ret=%d\n", ret);
				return -1;
			}

			lprintf("rs_setup_param:<ok>\n");
		}
                
                 if(rs_setup_smoothed_level(handle, RS_SMOOTHED_LEVEL_HOLD, 512, 0,
			motion_cal_sm_data, motion_cal_sm_data_num) != RS_OK){
		printf("rs_setup_smoothed_level failed\n");
		return -1;
	         }
	}
	/* 
	 * Sensor LSI is activate and ready to start sensing.
	 * The library state will be "READY".
	 */

	/* 
	 * Entry Motion detection operation can be started by calling the API, rs_start.
	 * The library state will be "SENSING".
	 */
	{
		rs_ret_t ret;

		ret = rs_start(handle);
		if(ret != RS_OK) {
			lprintf("rs_start: error ret=%d\n", ret);
			return -1;
		}

		lprintf("rs_start:<ok>\n");
	}

	{
		/* 
		 * The current status of DETOUT pin as Entry Motion detection result is acquired 
		 * by calling the API, rs_get_motion.
		 */
		{
			rs_bool_t motion;
			rs_ret_t ret;
                        do{
			ret = rs_get_motion(handle, &motion);
                        } while(1);
			if(ret != RS_OK) {
				lprintf("rs_get_motion: error ret=%d\n", ret);
				return -1;
			}

			lprintf("rs_get_motion:<ok> motion=%d\n", motion);
		}

		/* 
		 * The status of DETOUT pin when the status of DETOUT pin changes is acquired
		 * by calling the API, rs_wait_motion_change. This API blocks the program
		 * until the status of DETOUT pin changes.
		 */
		{
			int i;

			/* In the loop, the status of DETOUT pin is continuously acquired */
			for(i=0; i<5; i++) {
				rs_bool_t motion;
				rs_ret_t ret;

				ret = rs_wait_motion_change(handle, 10000, &motion);
				if(ret != RS_OK) {
					lprintf("rs_wait_motion_change: error ret=%d\n", ret);
					return -1;
				}

				lprintf("rs_wait_motion_change:<ok> motion=%d\n", motion);
			}
		}
	}

	/*
	 * The detection operation can be stopped by calling the API, rs_stop.
	 * The library state will be "READY".
	 */
	{
		rs_ret_t ret;

		ret = rs_stop(handle);
		if(ret != RS_OK) {
			lprintf("rs_stop: error ret=%d\n", ret);
			return -1;
		}

		lprintf("rs_stop:<ok>\n");
	}

	/* 
	 * The Sensor LSI can be deactivated by calling the API, rs_shutdown.
	 * The library state will be "SHUTDOWN".
	 */
	{
		rs_ret_t ret;

		ret = rs_shutdown(handle);
		if(ret != RS_OK) {
			lprintf("rs_shutdown: error ret=%d\n", ret);
			return -1;
		}

		lprintf("rs_shutdown:<ok>\n");
	}

	return 0;
}

/*
 * sample code of Entry Motion detection operation
 */
int motion_mode(rs_handle_t handle)
{
	lprintf("[Motion mode]\n");

	/* The Sensor LSI can be set up the sensor setting parameters */
	{
		struct rs_motion_param param;

		/* 
		 * The sensor setting parameters structure variable is set up with their initial values
		 * of the Entry Motion detection mode by calling the API, rs_param_init, specified that mode.
		 */
		{
			rs_ret_t ret;

			ret = rs_param_init(RS_MODE_MOTION_DETECT, &param);
			if(ret != RS_OK) {
				lprintf("rs_param_init: error ret=%d\n", ret);
				return -1;
			}

			lprintf("rs_param_init:<ok>\n");
		}

		/* 
		 * The sensor setting parameters structure variable is updated with the desired sensor
		 * setting parameters.
		 */
		param.rx_gain = 60;
		param.interval = 20;
		param.alpha = 153;
		param.motion_threshold = 200;
		param.startup_count = 20;
		param.lower_distance = 10;
		param.upper_distance = 200;
		param.chirp_time = 1100;
		param.digital_beam_shaper = RS_DIGITAL_BEAM_SHAPER_WIDE;
		param.high_pass_filter = RS_HPF_FIRST_ORDER;

		/* 
		 * The Sensor LSI is set up with Entry Motion detection mode and sensor setting parameters
		 * by calling the API, rs_setup_param, specified that mode and sensor setting
		 * parameters structure variable.
		 */
		{
			rs_ret_t ret;

			ret = rs_setup_param(handle, RS_MODE_MOTION_DETECT, &param);
			if(ret != RS_OK) {
				lprintf("rs_setup_param: error ret=%d\n", ret);
				return -1;
			}

			lprintf("rs_setup_param:<ok>\n");
		}
	}
	/* 
	 * Sensor LSI is activate and ready to start sensing.
	 * The library state will be "READY".
	 */

	/* 
	 * Entry Motion detection operation can be started by calling the API, rs_start.
	 * The library state will be "SENSING".
	 */
	{
		rs_ret_t ret;

		ret = rs_start(handle);
		if(ret != RS_OK) {
			lprintf("rs_start: error ret=%d\n", ret);
			return -1;
		}

		lprintf("rs_start:<ok>\n");
	}

	{
		/* 
		 * The current status of DETOUT pin as Entry Motion detection result is acquired 
		 * by calling the API, rs_get_motion.
		 */
		{
			rs_bool_t motion;
			rs_ret_t ret;
                        do{
			ret = rs_get_motion(handle, &motion);
                        } while(1);
			if(ret != RS_OK) {
				lprintf("rs_get_motion: error ret=%d\n", ret);
				return -1;
			}

			lprintf("rs_get_motion:<ok> motion=%d\n", motion);
		}

		/* 
		 * The status of DETOUT pin when the status of DETOUT pin changes is acquired
		 * by calling the API, rs_wait_motion_change. This API blocks the program
		 * until the status of DETOUT pin changes.
		 */
		{
			int i;

			/* In the loop, the status of DETOUT pin is continuously acquired */
			for(i=0; i<5; i++) {
				rs_bool_t motion;
				rs_ret_t ret;

				ret = rs_wait_motion_change(handle, 10000, &motion);
				if(ret != RS_OK) {
					lprintf("rs_wait_motion_change: error ret=%d\n", ret);
					return -1;
				}

				lprintf("rs_wait_motion_change:<ok> motion=%d\n", motion);
			}
		}
	}

	/*
	 * The detection operation can be stopped by calling the API, rs_stop.
	 * The library state will be "READY".
	 */
	{
		rs_ret_t ret;

		ret = rs_stop(handle);
		if(ret != RS_OK) {
			lprintf("rs_stop: error ret=%d\n", ret);
			return -1;
		}

		lprintf("rs_stop:<ok>\n");
	}

	/* 
	 * The Sensor LSI can be deactivated by calling the API, rs_shutdown.
	 * The library state will be "SHUTDOWN".
	 */
	{
		rs_ret_t ret;

		ret = rs_shutdown(handle);
		if(ret != RS_OK) {
			lprintf("rs_shutdown: error ret=%d\n", ret);
			return -1;
		}

		lprintf("rs_shutdown:<ok>\n");
	}

	return 0;
}

/*
 * sample code of Distance detection operation
 */
int distance_mode(rs_handle_t handle)
{
	lprintf("[Distance mode]\n");

	/* The Sensor LSI can be set up the sensor setting parameters */
	{
		struct rs_distance_param param;

		/* 
		 * The sensor setting parameters structure variable is set up with their initial values
		 * of the Distance detection mode by calling the API, rs_param_init, specified that mode.
		 */
		{
			rs_ret_t ret;

			ret = rs_param_init(RS_MODE_DISTANCE_DETECT, &param);
			if(ret != RS_OK) {
				lprintf("rs_param_init: error ret=%d\n", ret);
				return -1;
			}

			lprintf("rs_param_init:<ok>\n");
		}

		/* 
		 * The sensor setting parameters structure variable can be updated with the desired sensor
		 * setting parameters.
		 */
		param.rx_gain = 60;
		param.interval = 25;
		param.beta = 205;
		param.lower_distance = 10;
		param.upper_distance = 1000;
		param.chirp_time = 1100;
		param.digital_beam_shaper = RS_DIGITAL_BEAM_SHAPER_WIDE;
		param.high_pass_filter = RS_HPF_FIRST_ORDER;
		param.distance_step = RS_DISTANCE_STEP_NORMAL;

		/* 
		 * The Sensor LSI is set up with that sensor setting parameters structure variable
		 * containing sensor setting parameters by calling the API, rs_setup_param,
		 * specified with that mode.
		 */
		{
			rs_ret_t ret;

			ret = rs_setup_param(handle, RS_MODE_DISTANCE_DETECT, &param);
			if(ret != RS_OK) {
				lprintf("rs_setup_param: error ret=%d\n", ret);
				return -1;
			}

			lprintf("rs_setup_param:<ok>\n");
		}

		/* 
		 * The lower limit of the peak level corresponding to the distance can be configured
		 * by calling the API, rs_set_peak_level_lower. If the peak level corresponding to 
		 * the distance returned by calling the API, rs_get_distance, is less than that lower
		 * limit, an invalid value will be stored in that distance.
		 */
		{
			rs_ret_t ret;

			ret = rs_set_peak_level_lower(handle, 20);
			if(ret != RS_OK) {
				lprintf("rs_set_peak_level_lower: error ret=%d\n", ret);
				return -1;
			}

			lprintf("rs_set_peak_level_lower:<ok>\n");
		}
	}

	/* 
	 * The Distance detection operation can be started by calling the API, rs_start.
	 * The library state will be "SENSING".
	 */
	{
		rs_ret_t ret;

		ret = rs_start(handle);
		if(ret != RS_OK) {
			lprintf("rs_start: error ret=%d\n", ret);
			return -1;
		}

		lprintf("rs_start:<ok>\n");
	}

	{
		int i;
		int j;

		uint8_t level;

		/*
		 * The current lower limit of the peak level is acquired by calling the API,
		 * rs_get_peak_level_lower.
		 */
		{
			rs_ret_t ret;

			ret = rs_get_peak_level_lower(handle, &level);
			if(ret != RS_OK) {
				lprintf("rs_get_peak_level_lower: error ret=%d\n", ret);
				return -1;
			}

			lprintf("rs_get_peak_level_lower:<ok> level=%d\n", level);
		}

		/* 
		 * The distance data that are distance, peak level and complex vector of the rank no.1 to no.5
		 * are acquired by calling the API, rs_get_distance. This API blocks the program
		 * until the distance data can be read out from the Sensor LSI.
		 */

		/* In the loop, the Distance data are continuously acquired */
		for(i=0; i<100; i++) {
			struct rs_distance_data data;
			rs_ret_t ret;

			ret = rs_get_distance(handle, 1000, &data);
			if(ret != RS_OK) {
				lprintf("rs_get_distance: error ret=%d\n", ret);
				return -1;
			}

			lprintf("rs_get_distance:<ok>\n");
			lprintf("  distance={");
			for(j=0; j<5; j++) {
				if(data.distance[j] != 0x7fff) {
					lprintf("(%04d,%02d)", data.distance[j], data.peak_level[j]);
				} else {
					lprintf("(----,--)");
				}
			}
			lprintf("}\n");
			lprintf("  complex={");
			for(j=0; j<5; j++) {
				if(data.peak_level[j] >= level) {
					lprintf("(%+06d,%+06d)", data.peak_real[j], data.peak_imag[j]);
				} else {
					lprintf("(------,------)");
				}
			}
			lprintf("}\n");
		}
	}

	/*
	 * The detection operation can be stopped by calling the API, rs_stop.
	 * The library state will be "READY".
	 */
	{
		rs_ret_t ret;

		ret = rs_stop(handle);
		if(ret != RS_OK) {
			lprintf("rs_stop: error ret=%d\n", ret);
			return -1;
		}

		lprintf("rs_stop:<ok>\n");
	}

	/* 
	 * The Sensor LSI can be deactivated by calling the API, rs_shutdown.
	 * The library state will be "SHUTDOWN".
	 */
	{
		rs_ret_t ret;

		ret = rs_shutdown(handle);
		if(ret != RS_OK) {
			lprintf("rs_shutdown: error ret=%d\n", ret);
			return -1;
		}

		lprintf("rs_shutdown:<ok>\n");
	}

	return 0;
}

/*
 * sample code of 2D detection operation
 */
/*************** Tim 2D will be added later
int two_dimensions_mode(rs_handle_t handle)
{
	rs_2d_handle_t handle_2d;

	lprintf("[2D mode]\n");
**************************/

	/* The Sensor LSI can be set up the sensor setting parameters */
/***********************
	{
		struct rs_2rx_param param;
****************************/
		/* 
		 * The sensor setting parameters structure variable is set up with their initial values
		 * of the 2D detection mode by calling the API, rs_param_init, specified that mode.
		 */
/********************************
		{
			rs_ret_t ret;

			ret = rs_param_init(RS_MODE_2D_DETECT, &param);
			if(ret != RS_OK) {
				lprintf("rs_param_init: error ret=%d\n", ret);
				return -1;
			}

			lprintf("rs_param_init:<ok>\n");
		}
**************************/
		/* 
		 * The sensor setting parameters structure variable can be updated with the desired sensor
		 * setting parameters.
		 */
/*****************************
		param.rx_gain = 60;
		param.interval = 25;
		param.beta = 205;
		param.lower_distance = 10;
		param.upper_distance = 1000;
		param.chirp_time = 1100;
		param.high_pass_filter = RS_HPF_FIRST_ORDER;
		param.distance_step = RS_DISTANCE_STEP_NORMAL;
***************************/
		/* 
		 * The Sensor LSI is set up with that sensor setting parameters structure variable
		 * containing sensor setting parameters by calling the API, rs_setup_param,
		 * specified with that mode.
		 */
/*********************************
		{
			rs_ret_t ret;

			ret = rs_setup_param(handle, RS_MODE_2D_DETECT, &param);
			if(ret != RS_OK) {
				lprintf("rs_setup_param: error ret=%d\n", ret);
				return -1;
			}

			lprintf("rs_setup_param:<ok>\n");
		}
************************************/
		/* 
		 * The lower limit of the peak level corresponding to the distance can be configured
		 * by calling the API, rs_set_peak_level_lower. If the peak level corresponding to 
		 * the distance returned by calling the API, rs_get_2rx_peak, is less than that lower
		 * limit, an invalid value will be stored in that distance.
                  */
/*************************
		{
			rs_ret_t ret;

			ret = rs_set_peak_level_lower(handle, 20);
			if(ret != RS_OK) {
				lprintf("rs_set_peak_level_lower: error ret=%d\n", ret);
				return -1;
			}

			lprintf("rs_set_peak_level_lower:<ok>\n");
		}
******************************/
		/* 
		 * All interval variables in 2D library are initialized by calling the API, rs_2d_init.
		 */
/*******************************
		{
			rs_ret_t ret;

			ret = rs_2d_init(&handle_2d, &param);
			if(ret != RS_OK) {
				lprintf("rs_2d_init: error ret=%d\n", ret);
				return -1;
			}

			lprintf("rs_2d_init:<ok>\n");
		}
	}
******************************/
	/* The 2D library can be set up the 2D algorithm parameters */
/****************************
	{
		struct rs_2d_algo_param param;
***************************/
		/* 
		 * The 2D algorithm parameters structure variable is set up with their initial values
		 * by calling the API, rs_2d_get_init_algo_param.
		 */
 /************************
		{
			rs_ret_t ret;

			ret = rs_2d_get_init_algo_param(&param);
			if(ret != RS_OK) {
				lprintf("rs_2d_get_init_algo_param: error ret=%d\n", ret);
				return -1;
			}

			lprintf("rs_2d_get_init_algo_param:<ok>\n");
		}
*************************/
		/* 
		 * The 2D algorithm parameters structure variable is set up with the desired
		 * 2D algorithm parameters.
		 */
 /******************
		param.mode = RS_2D_MODE_MOVING_AVERAGE;
		param.ntap_ave_distance = 1;
		param.ntap_ave_angle = 10;
		param.ntap_ave_peak_level = 10;
		param.sort_order = RS_2D_SORT_ORDER_PEAK_LEVEL;
		param.lower_angle = -80;
		param.upper_angle = 80;
		param.angle_threshold = 400;
		param.stationary = RS_2D_STATIONARY_OFF;
*********************/
		/* 
		 * The 2D library is set up with that 2D algorithm parameters structure variable
		 * containing 2D algorithm parameters by calling the API, rs_2d_set_algo_param.
		 */
 /******************
		{
			rs_ret_t ret;

			ret = rs_2d_set_algo_param(handle_2d, &param);
			if(ret != RS_OK) {
				lprintf("rs_2d_set_algo_param: error ret=%d\n", ret);
				return -1;
			}

			lprintf("rs_2d_set_algo_param:<ok>\n");
		}
	}
*************************/
	/* 
	 * The 2D detection operation can be started by calling the API, rs_start.
	 * The library state will be "SENSING".
	 */
/******************
	{
		rs_ret_t ret;

		ret = rs_start(handle);
		if(ret != RS_OK) {
			lprintf("rs_start: error ret=%d\n", ret);
			return -1;
		}

		lprintf("rs_start:<ok>\n");
	}

	{
		int i;
		unsigned int j;

		uint8_t level;
**********************/
		/*
		 * The current lower limit of the peak level is acquired by calling the API,
		 * rs_get_peak_level_lower.
		 */
 /*************
		{
			rs_ret_t ret;

			ret = rs_get_peak_level_lower(handle, &level);
			if(ret != RS_OK) {
				lprintf("rs_get_peak_level_lower: error ret=%d\n", ret);
				return -1;
			}

			lprintf("rs_get_peak_level_lower:<ok> level=%d\n", level);
		}
**************/
		/* In the loop, the 2D location data are continuously acquired */
 /********** 
		for(i=0; i<100; i++) {
			struct rs_2rx_peak_data data;
			struct rs_2d_location_data location_data;
*************/
			/*
			 * The 2RX peak data that consists of distance, peak level, RX1 complex vector
			 * and RX2 complex vector of the rank no.1 to no.5 are acquired by the calling
			 * the API, rs_get_2rx_peak.
			 */
/*************************
			{
				rs_ret_t ret;

				ret = rs_get_2rx_peak(handle, 1000, &data);
				if(ret != RS_OK) {
					lprintf("rs_get_2rx_peak: error ret=%d\n", ret);
					return -1;
				}

				lprintf("rs_get_2rx_peak:<ok>\n");
			}

			lprintf("  distance={");
			for(j=0; j<5; j++) {
				if(data.distance.distance[j] != 0x7fff) {
					lprintf("(%04d,%02d)", data.distance.distance[j], data.distance.peak_level[j]);
				} else {
					lprintf("(----,--)");
				}
			}
			lprintf("}\n");
			lprintf("  rx1={");
			for(j=0; j<5; j++) {
				if(data.distance.peak_level[j] >= level) {
					lprintf("(%+06d,%+06d)", data.distance.peak_real[j], data.distance.peak_imag[j]);
				} else {
					lprintf("(------,------)");
				}
			}
			lprintf("}\n");
			lprintf("  rx2={");
			for(j=0; j<5; j++) {
				if(data.distance.peak_level[j] >= level) {
					lprintf("(%+06d,%+06d)", data.peak_rx2_real[j], data.peak_rx2_imag[j]);
				} else {
					lprintf("(------,------)");
				}
			}
			lprintf("}\n");
*****************/
			/* 
			 * The 2D location data that consists of one distance, angle and peak level
			 * are calculated on the basis of the 2RX peak data by calling the API, rs_2d_exec.
			 */
/********************
			{
				rs_ret_t ret;

				ret = rs_2d_exec(handle_2d, &data);
				if(ret != RS_OK) {
					lprintf("rs_2d_exec: error ret=%d\n", ret);
					return -1;
				}

				lprintf("rs_2d_exec:<ok>\n");
			}
********************/
			/* 
			 * The 2D location data are acquired by calling tha API, rs_2d_get_location.
			 */
/***********************
			{
				rs_ret_t ret;

				ret = rs_2d_get_location(handle_2d, &location_data);
				if(ret != RS_OK) {
					lprintf("rs_2d_get_location: error ret=%d\n", ret);
					return -1;
				}

				lprintf("rs_2d_get_location:<ok>\n");
			}

			for(j=0; j<location_data.number; j++) {
				lprintf("  angle[%d]=", j);
				if(location_data.angle[j] != 0x7fffffff) {
					lprintf("%+04d", location_data.angle[j]);
				} else {
					lprintf("----");
				}
				lprintf("  distance[%d]=", j);
				if(location_data.distance[j] != 0x7fffffff) {
					lprintf("%04d", location_data.distance[j]);
				} else {
					lprintf("----");
				}
				lprintf("  peak_level[%d]=", j);
				if(location_data.peak_level[j] != 0xffffffff) {
					lprintf("%02d", location_data.peak_level[j]);
				} else {
					lprintf("--");
				}
				lprintf("\n");
			}
		}
	}
***********/
	/*
	 * The detection operation can be stopped by calling the API, rs_stop.
	 * The library state will be "READY".
	 */
/**********
	{
		rs_ret_t ret;

		ret = rs_stop(handle);
		if(ret != RS_OK) {
			lprintf("rs_stop: error ret=%d\n", ret);
			return -1;
		}

		lprintf("rs_stop:<ok>\n");
	}
***********/
	/* 
	 * The 2D library is deinitialized by calling the API, rs_2d_deinit.
	 */
/*******
	{
		rs_ret_t ret;

		ret = rs_2d_deinit(handle_2d);
		if(ret != RS_OK) {
			lprintf("rs_2d_deinit: error ret=%d\n", ret);
			return -1;
		}

		lprintf("rs_2d_deinit:<ok>\n");
	}
***********/
	/* 
	 * The Sensor LSI can be deactivated by calling the API, rs_shutdown.
	 * The library state will be "SHUTDOWN".
	 */
/*********
	{
		rs_ret_t ret;

		ret = rs_shutdown(handle);
		if(ret != RS_OK) {
			lprintf("rs_shutdown: error ret=%d\n", ret);
			return -1;
		}

		lprintf("rs_shutdown:<ok>\n");
	}

	return 0;
}
**********************/
int _Main(void)
{
	rs_handle_t handle;

	/* The Sensor Library version is acquired by calling the API, rs_get_sensor_lib_ver. */
	{
		rs_ret_t ret;
		uint16_t major_ver;
		uint16_t minor_ver;
		uint16_t revision;
		uint16_t hotfix;

		ret = rs_get_sensor_lib_ver(&major_ver, &minor_ver, &revision, &hotfix);
               	if(ret != RS_OK) {
			lprintf("rs_get_sensor_lib_ver: error ret=%d\n", ret);
			return -1;
		}

		lprintf("rs_get_sensor_lib_ver:<ok> %d.%d.%d.%d\n", major_ver, minor_ver, revision, hotfix);
	}
	/* The Parameter setting Library version is acquired by calling the API, rs_get_setup_lib_ver. */
	{
		rs_ret_t ret;
		uint16_t major_ver;
		uint16_t minor_ver;
		uint16_t revision;
		uint16_t hotfix;

		ret = rs_get_setup_lib_ver(&major_ver, &minor_ver, &revision, &hotfix);
		if(ret != RS_OK) {
			lprintf("rs_get_setup_lib_ver: error ret=%d\n", ret);
			return -1;
		}

		lprintf("rs_get_setup_lib_ver:<ok> %d.%d.%d.%d\n", major_ver, minor_ver, revision, hotfix);
	}
	/* The 2D Library version is acquired by calling the API, rs_2d_get_lib_ver. */
 /************************  Tim 2D will be added later
	{
		rs_ret_t ret;
		uint16_t major_ver;
		uint16_t minor_ver;
		uint16_t revision;
		uint16_t hotfix;

		ret = rs_2d_get_lib_ver(&major_ver, &minor_ver, &revision, &hotfix);
		if(ret != RS_OK) {
			lprintf("rs_2d_get_lib_ver: error ret=%d\n", ret);
			return -1;
		}

		lprintf("rs_2d_get_lib_ver:<ok> %d.%d.%d.%d\n", major_ver, minor_ver, revision, hotfix);
	}
********************************/
	/* The EVK device is opened by calling the API, rs_open. */
	{
		rs_ret_t ret;

		ret = rs_open(&handle);
		if(ret != RS_OK) {
			lprintf("rs_open: error ret=%d\n", ret);
			return -1;
		}

		lprintf("rs_open:<ok>\n");
	}
	/* The library state will be "SHUTDOWN" */
       
        {
		int ret;

		ret = presence_mode(handle);
		if(ret != 0) {
			rs_close(handle);
			return -1;
		}
	}
       
        
/***	
	{
		int ret;

		ret = motion_mode(handle);
		if(ret != 0) {
			rs_close(handle);
			return -1;
		}
	}
 *****/      
	/* Distance detection operation is executed by calling the function, distance_mode. */
/***
	{
		int ret;

		ret = distance_mode(handle);
		if(ret != 0) {
			rs_close(handle);
			return -1;
		}
	}
****/

	/* 2D detection operation is executed by calling the function, two_dimensions_mode. */
/********************* Tim 2D will be added later
	{
		int ret;

		ret = two_dimensions_mode(handle);
		if(ret != 0) {
			rs_close(handle);
			return -1;
		}
	}
********************************/
	/* The EVK device is closed by calling the API, rs_close. */
	{
		rs_ret_t ret;

		ret = rs_close(handle);
		if(ret != RS_OK) {
			lprintf("rs_close: error ret=%d\n", ret);
			return -1;
		}

		lprintf("rs_close:<ok>\n");
	}

	return 0;
}

