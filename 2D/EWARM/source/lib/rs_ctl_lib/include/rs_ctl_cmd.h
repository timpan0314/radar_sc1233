/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs_ctl_cmd.h
 */

#ifndef __RS_CTL_CMD_H__
#define __RS_CTL_CMD_H__

#include "rs24g_types.h"
#include "rs_ctl_dev.h"
#include "rs_ctl_sensor_data.h"

#ifdef WIN32
/*

      The specification requires 30us setup time before reading any result registers
      from the OR (Output Ready) terminal is asserted. (chapter 4.5 "Distance
      detection" of "SC123xAR3 24GHz radar sensor LSI Datasheet").

      If this mandatory setup time can certainly be concealed by any other operation
      latency, it would be possible to skip that wait operation.

      In case of Socionext Sensor EVK connected to Windows Host PC, it can be
      concealed so that defining RS_SETUP_REGISTER_ACCESS_IS CONCEALED to skip the
      wait operation.

*/
#define RS_SETUP_REGISTER_ACCESS_IS_CONCEALED
#endif	/* WIN32 */

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
RS_IF rs_ret_t rs_ctl_cmd_boot(rs_ctl_dev_t dev);
RS_IF rs_ret_t rs_ctl_cmd_shutdown(rs_ctl_dev_t dev);
RS_IF rs_ret_t rs_ctl_cmd_disable_seq(rs_ctl_dev_t dev);
RS_IF rs_ret_t rs_ctl_cmd_enable_seq(rs_ctl_dev_t dev);

RS_IF rs_ret_t rs_ctl_cmd_get_sensor_data(rs_ctl_dev_t dev, const struct rs_ctl_sensor_data_set *set, struct rs_ctl_sensor_data *data);
RS_IF rs_ret_t rs_ctl_cmd_wait_and_get_sensor_data(rs_ctl_dev_t dev, uint32_t timeout, const struct rs_ctl_sensor_data_set *set, struct rs_ctl_sensor_data *data);

#endif	/* __RS_CTL_CMD_H__ */
