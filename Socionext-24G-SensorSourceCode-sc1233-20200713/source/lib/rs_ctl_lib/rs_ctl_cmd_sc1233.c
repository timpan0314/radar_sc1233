/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file rs_ctl_cmd_sc1233.c
 */

#include <stdint.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "rs_sleep.h"
#include "defs/rs_dev_addr.h"
#include "defs/rs_dev_opcode.h"
#include "defs/rs_dev_term.h"
#include "rs_ctl_dev.h"
#include "rs_ctl_op.h"
#include "rs_ctl_data.h"

uint32_t rvalue = 0;
/**
 * boot ("Start-Up") command \cite Boot
 * @callgraph
 */
RS_IF rs_ret_t rs_ctl_cmd_chipboot_sc1233(rs_ctl_dev_t dev)
{
	RET_ORG(rs_ret_t, rs_ctl_dev_term_set(dev, RS_TERM_CE, RS_TRUE), RS_OK);
	rs_usleep(100);
	RET_ORG(rs_ret_t, rs_ctl_dev_term_set(dev, RS_TERM_NRST, RS_TRUE), RS_OK);
	RET_ORG(rs_ret_t, rs_ctl_op_SRST(dev, 1), RS_OK);
	rs_usleep(5000);
	RET_ORG(rs_ret_t, rs_ctl_write_reg(dev, RS_CTL_REG_RD_START_EFUSE, 0x00000001), RS_OK);
	rs_usleep(54);
	RET_ORG(rs_ret_t, rs_ctl_write_reg(dev, RS_CTL_REG_CLKGEN, 0x001FFC44), RS_OK);
	{
		
		RET_ORG(rs_ret_t, rs_ctl_read_reg(dev, RS_CTL_REG_EFUSE37, &rvalue), RS_OK);
		if ((rvalue & 0x0000ff00) == 0x00000200) {
			return RS_ENOSUPPORT;
		}
		RET_CHECK((rvalue & 0x0000ff00) == 0x00000300, RS_EDEV);
	}
	RET_ORG(rs_ret_t, rs_ctl_write_reg(dev, RS_CTL_REG_SELSET_F_4, 0x00000000), RS_OK);

	return RS_OK;
}
