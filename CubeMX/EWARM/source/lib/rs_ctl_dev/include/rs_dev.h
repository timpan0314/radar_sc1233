/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file rs_dev.h
 */

#include "rs24g_types.h"
#include "rs_macro.h"
#include "rs_ctl_dev.h"

RS_IF rs_ret_t rs_dev_open(rs_ctl_dev_t *dev);
RS_IF rs_ret_t rs_dev_close(rs_ctl_dev_t dev);
RS_IF rs_ret_t rs_dev_write(rs_ctl_dev_t dev, const uint8_t *wdata, size_t size);
RS_IF rs_ret_t rs_dev_write_low_memory(rs_ctl_dev_t dev, const uint8_t *hdata, size_t hsize, const uint8_t *wdata, size_t wsize);
RS_IF rs_ret_t rs_dev_read(rs_ctl_dev_t dev, const uint8_t *wdata, size_t wsize, uint8_t *rdata, size_t rsize);
RS_IF rs_ret_t rs_dev_spi_read(rs_ctl_dev_t dev, const uint8_t *wdata, uint8_t *rdata, size_t size);
RS_IF rs_ret_t rs_dev_term_get(rs_ctl_dev_t dev, uint32_t term, rs_bool_t *val);
RS_IF rs_ret_t rs_dev_term_set(rs_ctl_dev_t dev, uint32_t term, rs_bool_t val);
RS_IF rs_ret_t rs_dev_term_set_trigger(rs_ctl_dev_t dev, uint32_t term, uint32_t trigger);
RS_IF rs_ret_t rs_dev_term_clear_trigger(rs_ctl_dev_t dev, uint32_t term);
RS_IF rs_ret_t rs_dev_term_wait_trigger(rs_ctl_dev_t dev, uint32_t term, uint32_t timeout /*ms */, uint32_t trigger, rs_bool_t *val);
