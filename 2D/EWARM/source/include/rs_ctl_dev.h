/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file rs_ctl_dev.h
 * @brief device level adaptation layer for Sensor
 * @version (PRELIMINARY)
 * @warning This is a part of sensor library source code for Evaluation Kit.
 */

#ifndef __RS_CTL_DEV_H__
#define __RS_CTL_DEV_H__

#include <stdint.h>

#include "rs24g_types.h"


/*
 * Type Definition
 */
typedef void *rs_ctl_dev_t;

/*
 * Constant Definition
 */
#define RS_CTL_DEV_FIFOMODE_ENABLE_MASK			(0x01)
#define RS_CTL_DEV_FIFOMODE_TYPE_MASK			(0x02)
#define RS_CTL_DEV_FIFOMODE_TYPE_IQ				(0x00)
#define RS_CTL_DEV_FIFOMODE_TYPE_FFT			(0x02)
#define RS_CTL_DEV_FIFOMODE_USEDATA_MASK		(0x04)

/*
 * Struct Definition
 */

/*
 * Function Definition
 */
/**
 * open device to communicate with Sensor
 *
 * @param[out] dev pointer of device handle (handle should be allocated at upper level)
 * @param[in] attr attributes (not used now)
 * @retval RS_OK everything is OK
 * @retval others something wrong
 * @callergraph
 * @sa rs_dev_open()
 */
RS_IF rs_ret_t rs_ctl_dev_open(rs_ctl_dev_t *dev, const void *attr);
/**
 * close device to communicate with Sensor
 *
 * @param[in] dev device handle
 * @retval RS_OK everything is OK, 
 * @retval others something wrong
 * @sa rs_dev_close()
 */
RS_IF rs_ret_t rs_ctl_dev_close(rs_ctl_dev_t dev);

RS_IF rs_ret_t rs_ctl_dev_frame_read(rs_ctl_dev_t dev, uint8_t *rdata, rs_size_t size);

/**
 * write Fast Control, Status Register, Sensor Register and Sensor RAM \cite WriteRegTr \cite WriteSeqTr, \cite WriteFFTTr
 *
 * Sensor RAM contains Sequencer program code and FFT window function
 * @param[in] dev device handle
 * @param[in] wdata pointer of write data
 * @param[in] size number of write data in bytes
 * @retval RS_OK everything is OK, 
 * @retval others something wrong
 * @sa rs_dev_write()
 */
RS_IF rs_ret_t rs_ctl_dev_write(rs_ctl_dev_t dev, const uint8_t *wdata, rs_size_t size);
/**
 * read Status Register, Sensor Register and Sensor RAM \cite ReadRegTr \cite ReadFIFOTr
 *
 * Sensor RAM contains FIFO
 * @param[in] dev device handle
 * @param[in] wdata pointer of write data
 * @param[in] wsize number of write data in bytes
 * @param[out] rdata pointer of read data
 * @param[in] rsize number of read data in bytes
 * @retval RS_OK everything is OK, 
 * @retval others something wrong
 * @sa rs_dev_read()
 */
RS_IF rs_ret_t rs_ctl_dev_read(rs_ctl_dev_t dev, const uint8_t *wdata, rs_size_t wsize, uint8_t *rdata, rs_size_t rsize);

RS_IF rs_ret_t rs_ctl_dev_spi_read(rs_ctl_dev_t dev, const uint8_t *wdata, uint8_t *rdata, rs_size_t size);

/**
 * set Sensor Terminal(GPIO)
 *
 * @param[in] dev device handle
 * @param[in] term @ref rs_term_id
 * @param[in] val setting bool value
 * @retval RS_OK everything is OK, 
 * @retval others something wrong
 * @sa rs_dev_term_set()
 */
RS_IF rs_ret_t rs_ctl_dev_term_set(rs_ctl_dev_t dev, uint32_t term, rs_bool_t val);
/**
 * get Sensor Terminal(GPIO)
 *
 * @param[in] dev device handle
 * @param[in] term @ref rs_term_id
 * @param[out] val pointer of getting bool value
 * @retval RS_OK everything is OK, 
 * @retval others something wrong
 * @sa rs_dev_term_get()
 */
RS_IF rs_ret_t rs_ctl_dev_term_get(rs_ctl_dev_t dev, uint32_t term, rs_bool_t *val);

/**
 * clear Trigger flag for Sensor Terminal(GPIO)
 *
 * @param[in] dev device handle
 * @param[in] term @ref rs_term_id
 * @retval RS_OK everything is OK, 
 * @retval others something wrong
 * @sa rs_dev_term_clear_trigger()
 */
RS_IF rs_ret_t rs_ctl_dev_term_trig_clear(rs_ctl_dev_t dev, uint32_t term);
/**
 * setup Trigger for Sensor Terminal(GPIO)
 *
 * @param[in] dev device handle
 * @param[in] term @ref rs_term_id
 * @param[in] trigger @ref rs_term_trigger as RISING or FALLING edge
 * @retval RS_OK everything is OK, 
 * @retval others something wrong
 * @sa rs_dev_term_set_trigger()
 */
RS_IF rs_ret_t rs_ctl_dev_term_trig_set(rs_ctl_dev_t dev, uint32_t term, uint32_t trigger);
RS_IF rs_ret_t rs_ctl_dev_term_trig_get(rs_ctl_dev_t dev, uint32_t term, uint32_t trigger, rs_bool_t *val);
/**
 * wait Trigger is fired for Sensor Terminal(GPIO)
 *
 * @param[in] dev device handle
 * @param[in] timeout timeout in msec
 * @param[in] term @ref rs_term_id
 * @param[in] trigger @ref rs_term_trigger to wait (RISE or FALL)
 * @param[out] val pointer of getting bool value at fire
 * @retval RS_OK everything is OK, 
 * @retval RS_ETOUT when timeout,
 * @retval others something wrong
 * @sa rs_dev_term_wait_trigger()
 */
RS_IF rs_ret_t rs_ctl_dev_term_trig_wait(rs_ctl_dev_t dev, uint32_t timeout, uint32_t term, uint32_t trigger, rs_bool_t *val);

#endif	/* __RS_CTL_DEV_H__ */
