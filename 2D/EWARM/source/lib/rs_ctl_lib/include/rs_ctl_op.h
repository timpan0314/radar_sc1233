/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs_ctl_op.h
 */

#ifndef __RS_CTL_OP_H__
#define __RS_CTL_OP_H__

#include "rs24g_types.h"
#include "defs/rs_dev_opcode.h"
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
RS_IF rs_ret_t rs_ctl_op_HRST(rs_ctl_dev_t dev);
RS_IF rs_ret_t rs_ctl_op_SRST(rs_ctl_dev_t dev, int with_extra);
RS_IF rs_ret_t rs_ctl_op_DSLEEP(rs_ctl_dev_t dev);
RS_IF rs_ret_t rs_ctl_op_WRSR(rs_ctl_dev_t dev, uint8_t data);
RS_IF rs_ret_t rs_ctl_op_RDSR(rs_ctl_dev_t dev, uint8_t *data);
RS_IF rs_ret_t rs_ctl_op_WRITE(rs_ctl_dev_t dev, uint32_t address, const uint8_t *data, rs_size_t size);
RS_IF rs_ret_t rs_ctl_op_READ(rs_ctl_dev_t dev, uint32_t address, uint8_t *data, rs_size_t size);
RS_IF rs_ret_t rs_ctl_op_RDSR2(rs_ctl_dev_t dev, uint8_t *data);
RS_IF rs_ret_t rs_ctl_op_ENATM(rs_ctl_dev_t dev);
RS_IF rs_ret_t rs_ctl_op_DISTM(rs_ctl_dev_t dev);
RS_IF rs_ret_t rs_ctl_op_RUNTM(rs_ctl_dev_t dev);
RS_IF rs_ret_t rs_ctl_op_STPTM(rs_ctl_dev_t dev);
RS_IF rs_ret_t rs_ctl_op_HLDDT(rs_ctl_dev_t dev, int with_timer, int in_deep_sleep);
RS_IF rs_ret_t rs_ctl_op_UPDDT(rs_ctl_dev_t dev, int with_timer);

#endif	/* __RS_CTL_OP_H__ */
