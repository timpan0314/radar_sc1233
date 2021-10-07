/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/**
 * @file rs24g_ctl_base.c
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "rs24g_types.h"
#include "rs_macro.h"
#include "rs_sleep.h"
#include "defs/rs_dev_addr.h"
#include "defs/rs_dev_opcode.h"
#include "defs/rs_dev_term.h"
#include "rs_ctl_dev.h"
#include "rs_ctl_cmd.h"
#include "rs_ctl_data.h"
#include "rs_ctl_op.h"

#include "rs24g_ctl_base.h"
#include "rs24g_ctl_param.h"
#include "rs_handle.h"
#include "chipboot.h"
#include "rs24g_ctl_sensor_ctl_type.h"

#define RS_RESUME (1)
#define RS_USE_AVERAGED_DIFFERENCE /* Ability to operate averaged difference */

/**
 * open Sensor
 *
 * @callgraph
 */
RS_IF rs_ret_t rs_open(rs_handle_t *handle)
{
	struct rs_handle *lh = NULL;

	rs_ret_t ret = RS_OK;

	RET_CHECK(handle != NULL, RS_EFAULT);

	*handle = NULL; /* on error */

	lh = (struct rs_handle *) calloc(1, sizeof(struct rs_handle));
	if(lh == NULL) {
		ret = RS_ENOMEM;
	}

	if(ret == RS_OK) {
		ret = rs_resource_alloc_holder(&lh->res_holder);

		if(ret == RS_OK) {
			ret = rs_ctl_dev_open(&(lh->dev), NULL);

			if(ret == RS_OK) {
				ret = rs24g_mutex_init(&(lh->mutex));

				if(ret == RS_OK) {
					ret = rs_ctl_dev_term_trig_clear(lh->dev, RS_TERM_OR);
					if(ret == RS_OK) {
						ret = rs_ctl_dev_term_trig_clear(lh->dev, RS_TERM_DETOUT);
					}
					if(ret == RS_OK) {
						ret = rs_ctl_dev_term_trig_set(lh->dev, RS_TERM_OR, RS_TERM_TRIGGER_RISING);
					}
					if(ret == RS_OK) {
						ret = rs_ctl_dev_term_trig_set(lh->dev, RS_TERM_DETOUT, RS_TERM_TRIGGER_RISING | RS_TERM_TRIGGER_FALLING);
					}

					if(ret == RS_OK) {
						lh->state = RS_STATE_SHUTDOWN;
						lh->mode = RS_MODE_INVALID;
						lh->peak_level_lower = RS_PEAK_LEVEL_LOWER_DEFAULT;
						memset(&(lh->code_ref), 0, sizeof(struct rs_code_ref_s));
					}

					if(ret == RS_OK) {
						ret = rs_ctl_cmd_shutdown(lh->dev);
					}
					if(ret == RS_OK) {
						ret = chipboot(lh->dev);
					}
					if(ret == RS_OK) {
						ret = rs_ctl_cmd_shutdown(lh->dev);
					}

					if(ret == RS_OK) {
						*handle = lh;
						return ret;
					}

					rs24g_mutex_destroy(lh->mutex);
				}

				rs_ctl_dev_close(lh->dev);
			}
		}

		free(lh);
	}

	return ret;
}

/**
 * close Sensor
 *
 * @callgraph
 */
RS_IF rs_ret_t rs_close(rs_handle_t handle)
{
	struct rs_handle *lh = (struct rs_handle *) handle;

	rs_ret_t ret = RS_OK;

	RET_CHECK(handle != NULL, RS_EINV);

	(void)rs_resource_free_holder(&lh->res_holder);

	ret = rs24g_mutex_destroy(lh->mutex);

	if(ret == RS_OK) {
		ret = rs_ctl_dev_close(lh->dev);
	}

	memset(lh, 0, sizeof(struct rs_handle));
	free(lh);

	return ret;
}

/**
 * shutdown Sensor
 *
 * @callgraph
 */
RS_IF rs_ret_t rs_shutdown(rs_handle_t handle)
{
	struct rs_handle *lh = (struct rs_handle *) handle;

	rs_ret_t ret = RS_OK;

	RET_CHECK(handle != NULL, RS_EINV);

	ret = rs24g_mutex_lock(lh->mutex);

	if(ret == RS_OK) {
		if(lh->state != RS_STATE_READY) {
			ret = RS_ESTATE;
		}

		if(ret == RS_OK) {
			ret = rs_ctl_cmd_shutdown(lh->dev);
		}

		if(ret == RS_OK) {
			lh->state = RS_STATE_SHUTDOWN;
			lh->mode = RS_MODE_INVALID;
		}

		if(ret == RS_OK) {
			ret = rs24g_mutex_unlock(lh->mutex);
		} else {
			rs24g_mutex_unlock(lh->mutex);
		}
	}

	return ret;
}

static rs_ret_t rs_start_body(rs_handle_t handle, uint8_t resume) {
	struct rs_handle *lh = (struct rs_handle *) handle;

	rs_ret_t ret = RS_OK;

	RET_CHECK(handle != NULL, RS_EINV);

	ret = rs24g_mutex_lock(lh->mutex);

	if(ret == RS_OK) {
		if(lh->state != RS_STATE_READY) {
			ret = RS_ESTATE;
		}

		if(ret == RS_OK) {
			ret = rs_ctl_dev_term_trig_clear(lh->dev, RS_TERM_OR);
		}
		if(ret == RS_OK) {
			ret = rs_ctl_dev_term_trig_clear(lh->dev, RS_TERM_DETOUT);
		}

		if(ret == RS_OK) {
			ret = rs_ctl_op_SRST(lh->dev, 1);
		}
		rs_usleep(5 * 1000);

		if(ret == RS_OK) {
			uint32_t value;
			if (resume) {
				value = 0x00000100;
			} else {
				value = 0x000009F0;				
			}
			ret = rs_ctl_write_reg(lh->dev, RS_CTL_REG_SELDIG_06, value);
		}
		if(ret == RS_OK) {
			ret = rs_ctl_write_reg(lh->dev, RS_CTL_REG_SELDIG_06, 0x00000000);
		}

		if(ret == RS_OK) {
			uint8_t status = 0;
			ret = rs_ctl_op_RDSR(lh->dev, &status);
			if(ret == RS_OK) {
				if((status & RS_CTL_OP_STATUS_SEQ_BUSY) != 0x00) {
					ret = RS_EDEV;
				}
			}
		}
                 rs_set_sensor_ctl_type(handle,RS_SENSOR_CTL_CPU); //_Tim
		// can't write registers in deep sleep state on I2C
		if(lh->sensor_ctl_type == RS_SENSOR_CTL_TIMER) {
			if(ret == RS_OK) {
				ret = rs_ctl_write_reg(lh->dev, RS_CTL_REG_CLKGEN,
						       (RS_CTL_REG_CLKGEN_SEQ_ENABLE |
							RS_CTL_REG_CLKGEN_TIMER_ENABLE));
			}
			if(ret == RS_OK) {
				ret = rs_ctl_op_DSLEEP(lh->dev);
			}

			if(ret == RS_OK) {
				ret = rs_ctl_op_ENATM(lh->dev);
			}
			if(ret == RS_OK) {
				ret = rs_ctl_op_RUNTM(lh->dev);
			}
		} else if(lh->sensor_ctl_type == RS_SENSOR_CTL_CPU) {
			if(ret == RS_OK) {
				ret = rs_ctl_op_WRSR(lh->dev, RS_CTL_MODE_CONT_START);
			}
		}

		if(ret == RS_OK) {
			lh->state = RS_STATE_SENSING;
		}

		if(ret == RS_OK) {
			ret = rs24g_mutex_unlock(lh->mutex);
		} else {
			rs24g_mutex_unlock(lh->mutex);
		}
	}

	return ret;
}

/**
 * start sensing
 *
 * @callgraph
 */

RS_IF rs_ret_t rs_start(rs_handle_t handle)
{
	return rs_start_body(handle, !RS_RESUME);
}

#if defined(RS_USE_AVERAGED_DIFFERENCE)
static rs_ret_t rs_stop_wait(struct rs_handle *lh) {
	rs_ret_t ret = RS_OK;

	RET_CHECK(lh != NULL, RS_EINV);

	if(lh->sensor_ctl_type == RS_SENSOR_CTL_TIMER) {
		/* wait interval time */
#if defined(RS_CPUIF_USE_I2C)
		if(ret == RS_OK) {
			ret = rs_ctl_op_HLDDT(lh->dev, 1, 1);
		}
#endif			
		if(ret == RS_OK) {
			uint32_t rdata;
			ret = rs_ctl_read_reg(lh->dev, RS_CTL_REG_CPUIF_4, &rdata);
			if (ret == RS_OK) {
				rs_usleep((rdata / 0x40 + 1) * 1000);
			}
		}
#if defined(RS_CPUIF_USE_I2C)
		if(ret == RS_OK) {
			ret = rs_ctl_op_UPDDT(lh->dev, 1);
		}
#endif
	} else if(lh->sensor_ctl_type == RS_SENSOR_CTL_CPU) {
		/* wait not SEQ_BUSY */
		if(ret == RS_OK) {
			uint8_t status = 0;
			time_t start, current;
			time(&start);
			for(;;) {
				ret = rs_ctl_op_RDSR(lh->dev, &status);
				if((status & RS_CTL_OP_STATUS_SEQ_BUSY) == 0x00) {
					break;
				}
				time(&current);
				if(current > start + RS_CONTINUOUS_STOP_WAIT_SEC) {
					ret = RS_EDEV;
					break;
				}
				rs_usleep(100);
			}
		}
	}
	return ret;
}
#endif

/**
 * stop sensing
 *
 * @callgraph
 */
RS_IF rs_ret_t rs_stop(rs_handle_t handle)
{
	struct rs_handle *lh = (struct rs_handle *) handle;

	rs_ret_t ret = RS_OK;

	RET_CHECK(handle != NULL, RS_EINV);

	ret = rs24g_mutex_lock(lh->mutex);

	if(ret == RS_OK) {
		if(lh->state != RS_STATE_SENSING) {
			ret = RS_ESTATE;
		}

		if(lh->sensor_ctl_type == RS_SENSOR_CTL_TIMER) {
			if(ret == RS_OK) {
				ret = rs_ctl_op_STPTM(lh->dev);
			}
#if defined(RS_USE_AVERAGED_DIFFERENCE)
			if (ret == RS_OK) {
				ret = rs_stop_wait(lh);
			}
#endif			
			if(ret == RS_OK) {
				ret = rs_ctl_op_DISTM(lh->dev);
			}
		} else if(lh->sensor_ctl_type == RS_SENSOR_CTL_CPU) {
			if(ret == RS_OK) {
				ret = rs_ctl_op_WRSR(lh->dev, RS_CTL_MODE_CONT_STOP);
			}
#if defined(RS_USE_AVERAGED_DIFFERENCE)
			if (ret == RS_OK) {
				ret = rs_stop_wait(lh);
			}
#endif			
			if(ret == RS_OK) {
				ret = rs_ctl_op_DSLEEP(lh->dev);
			}
		}

		if(ret == RS_OK) {
			lh->state = RS_STATE_READY;
		}

		if(ret == RS_OK) {
			ret = rs24g_mutex_unlock(lh->mutex);
		} else {
			rs24g_mutex_unlock(lh->mutex);
		}
	}

	return ret;
}

RS_IF rs_ret_t rs_get_state(rs_handle_t handle, rs_state_t *state)
{
	struct rs_handle *lh = (struct rs_handle *) handle;

	rs_ret_t ret = RS_OK;

	RET_CHECK(handle != NULL, RS_EINV);
	RET_CHECK(state != NULL, RS_EFAULT);

	ret = rs24g_mutex_lock(lh->mutex);

	if(ret == RS_OK) {
		*state = lh->state;

		ret = rs24g_mutex_unlock(lh->mutex);
	}

	return ret;
}

/**
 * resume sensing
 *
 * @callgraph
 */
RS_IF rs_ret_t rs_resume(rs_handle_t handle)
{
	return rs_start_body(handle, RS_RESUME);
}
