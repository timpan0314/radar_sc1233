/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs_ctl_data.h
 */

#ifndef __RS_CTL_DATA_H__
#define __RS_CTL_DATA_H__

#include "rs24g_types.h"
#include "defs/rs_dev_addr.h"
#include "rs_ctl_dev.h"


/*
 * Type Definition
 */

/*
 * Constant Definition
 */

/*
 * Struct Definition
 */

/*
 * Function Definition
 */
RS_IF uint16_t rs_calc_crc16(uint16_t crc16, const uint8_t *data, rs_size_t size);

RS_IF rs_ret_t rs_ctl_write_mem(rs_ctl_dev_t dev, uint32_t addr, const uint8_t *data, rs_size_t size);
RS_IF rs_ret_t rs_ctl_read_mem(rs_ctl_dev_t dev, uint32_t addr, uint8_t *data, rs_size_t size);
RS_IF rs_ret_t rs_ctl_write_reg(rs_ctl_dev_t dev, uint32_t addr, uint32_t data);
RS_IF rs_ret_t rs_ctl_read_reg(rs_ctl_dev_t dev, uint32_t addr, uint32_t *data);
RS_IF rs_ret_t rs_ctl_write_regs(rs_ctl_dev_t dev, uint32_t addr, const uint32_t *data, rs_size_t num);
RS_IF rs_ret_t rs_ctl_read_regs(rs_ctl_dev_t dev, uint32_t addr, uint32_t *data, rs_size_t num);

#endif	/* __RS_CTL_DATA_H__ */
