/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file rs_dev_term.h
 */

#ifndef __RS_DEV_TERM_H__
#define __RS_DEV_TERM_H__

#define RS_TERM_TYPE_IN							(0x00000000)
#define RS_TERM_TYPE_OUT						(0x00000010)
#define RS_TERM_TYPE_MASK						(0x000000f0)

/**
 * @defgroup rs_term_id Sensor terminal ID
 * @{
 */
/** ID for NRST pin */
#define RS_TERM_NRST							(RS_TERM_TYPE_IN | (0x00000000))
/** ID for CE pin */
#define RS_TERM_CE								(RS_TERM_TYPE_IN | (0x00000001))
/** ID for OR pin */
#define RS_TERM_OR								(RS_TERM_TYPE_OUT | (0x00000000))
/** ID for DETOUT pin */
#define RS_TERM_DETOUT							(RS_TERM_TYPE_OUT | (0x00000001))
#define RS_TERM_NONE							(0x000000ff)

#define RS_TERM_GPIO3							(0x00000006)
#define RS_TERM_LED								(0x000000f0)
#define RS_TERM_LD1								(0x000000f1)
#define RS_TERM_LD3								(0x000000f2)
/** @} */

/**
 * @defgroup rs_term_trigger type of trigger
 * @{
 */
/** Rising edge */
#define RS_TERM_TRIGGER_RISING					(0x00000001)
/** Falling edge */
#define RS_TERM_TRIGGER_FALLING					(0x00000002)
#define RS_TERM_TRIGGER_ALL						(RS_TERM_TRIGGER_RISING | RS_TERM_TRIGGER_FALLING)
/** @} */

#endif	/* __RS_DEV_TERM_H__ */
