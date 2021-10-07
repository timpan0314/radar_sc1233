/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * setup_common.h
 */

#ifndef __SETUP_COMMON_H__
#define __SETUP_COMMON_H__

#include <stdint.h>

#include "rs24g_types.h"
#include "rs24g_ctl_setup_base.h"
#include "rs24g_initial_dataset.h"
#include "rs24g_ctl_code.h"
#include "rs24g_code2.h"


//#define rs24gsetup_dp(fmt, ...)					{fprintf(stdout, "[rs24g_ctl_setup] %s:%d: " fmt "\n", __FUNCTION__, __LINE__, ## __VA_ARGS__); fflush(stdout);}
#define rs24gsetup_dp(...)


extern rs_ret_t read_modify_write_reg(rs_handle_t handle, uint16_t addr, uint32_t mask, uint32_t data);
extern rs_ret_t table_read_reg(rs_code_ref_t code, uint16_t addr, uint32_t *data);

extern rs_ret_t rs_setup_getcode(rs_code_ref_t code, const struct initial_dataset *setup);
extern rs_ret_t rs_setup_getcode_ext(rs_code_ref_t code, const struct rs_code2_s *setup);
extern rs_ret_t rs_setup_getmode_ext(rs_mode_t *p_mode, const struct rs_code2_s *setup);

extern double effective_chirp_bandwidth(const uint32_t fft_point);
extern double point_per_chirp(rs_handle_t handle, const uint32_t fft_point);
extern rs_ret_t rs_setup_range(rs_handle_t handle, uint16_t addr, uint32_t upper, uint32_t lower, int32_t conv_offset);
extern rs_ret_t rs_setup_interval(rs_handle_t handle, uint32_t interval);
extern rs_ret_t rs_setup_hpf(rs_handle_t handle, rs_hpf_t hpf);

#endif	/* __SETUP_COMMON_H__ */
