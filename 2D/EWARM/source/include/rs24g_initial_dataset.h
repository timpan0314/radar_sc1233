/*
 * Copyright (c) 2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_initial_dataset.h
 */

#ifndef __RS24G_INITIAL_DATASET_H__
#define __RS24G_INITIAL_DATASET_H__

#include <stdint.h>
#include "rs24g_types.h"

/**
 * initial dataset to setup SC123x
 */
struct initial_dataset
{
	/** sequencer program code */
	const uint8_t *seq;
	const rs_size_t seq_size;
	/** initial register addresses */
	const uint16_t *reg_addr;
	/** initial register values */
	const uint32_t *reg_val;
	const rs_size_t reg_size;
	/** FFT window function */
	const uint8_t *fftwin;
	const rs_size_t fftwin_size;
	/** Timer Operation */
	const uint8_t timer_operation;
};

#endif	/* __RS24G_INITIAL_DATASET_H__ */
