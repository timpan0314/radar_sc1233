/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_bin_local.h
 */

#ifndef __RS24G_BIN_LOCAL_H__
#define __RS24G_BIN_LOCAL_H__

#include "rs24g_types.h"
#include "rs24g_bin.h"

/*
 * Type Definition
 */

/*
 * Constant Definition
 */
#define RS_BIN_SECTION_CODE 1
#define RS_BIN_SECTION_ATTR 2

/*
 * Struct Definition
 */
struct rs_bin_header_s
{
	uint8_t magic;
	uint8_t reserved1;
	uint8_t major_ver;
	uint8_t minor_ver;
	uint32_t body_size;
	uint8_t gen_major_ver;
	uint8_t gen_minor_ver;
	uint8_t gen_revision_ver;
	uint8_t gen_hotfix_ver;
	uint32_t reserved2;
	uint16_t hw_type;
	uint16_t hw_major_ver;
	uint16_t hw_minor_ver;
	uint16_t reserved3;
};

static const uint32_t header_size = sizeof(struct rs_bin_header_s);
static const uint8_t header_magic = 0xa5;
static const uint8_t header_reserved1 = 1;
static const uint32_t header_reserved2 = 0;
static const uint16_t header_reserved3 = 0;
static const uint32_t section_header_size = sizeof(uint32_t) + sizeof(uint32_t);

struct rs_bin_section_s
{
	struct rs_bin_section_s *next;
	uint32_t id;
	uint32_t size;
};

struct rs_bin_data_section_s
{
	struct rs_bin_section_s base;
	uint32_t size;
	uint8_t *data;
};

static const uint32_t data_section_header_size = sizeof(uint32_t);

struct rs_bin_general_section_s
{
	struct rs_bin_section_s base;
	uint8_t *data;
};

struct rs_bin_s
{
	uint32_t valid;
	struct rs_bin_header_s header;
	struct rs_bin_section_s *body;
};

/*
 * Function Definition
 */
RS_IF rs_ret_t rs_bin_add_code_section(rs_bin_t bin, uint8_t *data, uint32_t size);
RS_IF rs_ret_t rs_bin_add_attr_section(rs_bin_t bin, uint8_t *data, uint32_t size);
RS_IF rs_ret_t rs_bin_add_data_section(rs_bin_t bin, uint32_t id, uint32_t sz, uint32_t size, uint8_t *data);
RS_IF rs_ret_t rs_bin_add_general_section(rs_bin_t bin, uint32_t id, uint32_t sz, uint8_t *data);
RS_IF rs_ret_t rs_bin_save(rs_bin_t bin, void *buf, uint32_t bufsize);
RS_IF rs_ret_t rs_bin_get_size(rs_bin_t bin, uint32_t *size);
RS_IF rs_ret_t rs_bin_set_gen(rs_bin_t bin, uint8_t gen_major_ver, uint8_t gen_minor_ver, uint8_t gen_revision_ver, uint8_t gen_hotfix_ver);
RS_IF rs_ret_t rs_bin_get_gen(rs_bin_t bin, uint8_t *gen_major_ver, uint8_t *gen_minor_ver, uint8_t *gen_revision_ver, uint8_t *gen_hotfix_ver);
RS_IF rs_ret_t rs_bin_set_hw(rs_bin_t bin, uint16_t hw_type, uint16_t hw_major_ver, uint16_t hw_minor_ver);
RS_IF rs_ret_t rs_bin_get_hw(rs_bin_t bin, uint16_t *hw_type, uint16_t *hw_major_ver, uint16_t *hw_minor_ver);
RS_IF rs_ret_t rs_bin_get_code_section(rs_bin_t bin, struct rs_bin_data_section_s **p);
RS_IF rs_ret_t rs_bin_get_attr_section(rs_bin_t bin, struct rs_bin_data_section_s **p);
RS_IF rs_ret_t rs_bin_get_data_section(rs_bin_t bin, uint32_t id, struct rs_bin_data_section_s **p);

#endif /* __RS24G_BIN_LOCAL_H__ */
