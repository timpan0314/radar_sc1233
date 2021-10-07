/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 */

/*
 * rs24g_bin.c
 */

#include <stdlib.h>
#include "rs24g_bin_local.h"
#include "rs24g_bin_version.h"
#include "rs_macro.h"
#include "convbin.h"

#define VALID_MARKER 0xb174a11d

static void free_data_section(struct rs_bin_section_s *p)
{
	struct rs_bin_data_section_s *data_section = (struct rs_bin_data_section_s *)p;
	if(data_section->data != NULL) free(data_section->data);
	free(data_section);
}

static void free_general_section(struct rs_bin_section_s *p)
{
	struct rs_bin_general_section_s *general_section = (struct rs_bin_general_section_s *)p;
	if(general_section->data != NULL) free(general_section->data);
	free(general_section);
}

static void free_section(struct rs_bin_section_s *p)
{
	if(p->id == RS_BIN_SECTION_CODE ||
	   p->id == RS_BIN_SECTION_ATTR) {
		free_data_section(p);
	}
	else {
		free_general_section(p);
	}
}

static int check_header(struct rs_bin_header_s *header)
{
	RS_ASSERT(header->magic == header_magic, RS_EINV);
	RS_ASSERT(header->reserved1 == header_reserved1, RS_EINV);
	RS_ASSERT(header->reserved2 == header_reserved2, RS_EINV);
	RS_ASSERT(header->reserved3 == header_reserved3, RS_EINV);

	return RS_OK;
}

static uint32_t get_data_section_size(uint32_t size)
{
	uint32_t padding_size = size % sizeof(uint64_t);
	if(padding_size > 0) padding_size = sizeof(uint64_t) - padding_size;
	return data_section_header_size + size + padding_size;
}

static rs_ret_t save_data_section(void **pbuf, size_t *pres, struct rs_bin_data_section_s *p)
{
	uint32_t sec_sz = p->base.size;
	uint32_t size = p->size;
	uint8_t *data = p->data;
	uint32_t padding = sec_sz - (data_section_header_size + size);
	uint8_t dummy[8] = { 0 };

	RS_ASSERT(bsave32(pbuf, pres, size) == 0, RS_EINV);
	RS_ASSERT(bsave8a(pbuf, pres, data, size) == 0, RS_EINV);

	if(padding > 0) RS_ASSERT(bsave8a(pbuf, pres, dummy, padding) == 0, RS_EINV);

	return RS_OK;
}

static rs_ret_t load_data_section(const void **pbuf, size_t *pres, uint32_t *psize, uint8_t **pdata, uint32_t *prsz)
{
	*prsz = 0;

	*psize = 0;
	*pdata = NULL;

	RS_ASSERT(bload32(pbuf, pres, psize) == 0, RS_EINV);
	*prsz += sizeof(uint32_t);
	*pdata = (uint8_t *)malloc(*psize);
	RS_ASSERT(*pdata != NULL, RS_ENOMEM);
	RS_ASSERT(bload8a(pbuf, pres, *pdata, *psize) == 0, RS_EINV);
	*prsz += *psize;

	return RS_OK;
}

static rs_ret_t save_general_section(void **pbuf, size_t *pres, struct rs_bin_general_section_s *p)
{
	uint32_t sec_sz = p->base.size;
	uint8_t *data = p->data;
	uint32_t size = sec_sz;

	RS_ASSERT(bsave8a(pbuf, pres, data, size) == 0, RS_EINV);

	return RS_OK;
}

static rs_ret_t load_general_section(const void **pbuf, size_t *pres, uint32_t size, uint8_t **pdata, uint32_t *prsz)
{
	*prsz = 0;

	*pdata = NULL;

	*pdata = (uint8_t *)malloc(size);
	RS_ASSERT(*pdata != NULL, RS_ENOMEM);
	RS_ASSERT(bload8a(pbuf, pres, *pdata, size) == 0, RS_EINV);
	*prsz += size;

	return RS_OK;
}

static void add_section(rs_bin_t bin, struct rs_bin_section_s *p)
{
	struct rs_bin_section_s *last;

	for(last = bin->body; last != NULL && last->next != NULL; last = last->next) ;
	if(last != NULL) {
		last->next = p;
	}
	else {
		bin->body = p;
	}
	p->next = NULL;
}

static rs_ret_t save_sections(void **pbuf, size_t *pres, rs_bin_t bin)
{
	struct rs_bin_section_s *section;

	for(section = bin->body; section != NULL; section = section->next) {
		RS_ASSERT(bsave32(pbuf, pres, section->id) == 0, RS_EINV);
		RS_ASSERT(bsave32(pbuf, pres, section->size) == 0, RS_EINV);

		if(section->id == RS_BIN_SECTION_CODE ||
		    section->id == RS_BIN_SECTION_ATTR) {
			struct rs_bin_data_section_s *data_section = (struct rs_bin_data_section_s *)section;
			RS_CALL(save_data_section(pbuf, pres, data_section));
		}
		else {
			struct rs_bin_general_section_s *general_section = (struct rs_bin_general_section_s *)section;
			RS_CALL(save_general_section(pbuf, pres, general_section));
		}
	}

	return RS_OK;
}

static rs_ret_t load_sections(const void **pbuf, size_t *pres, rs_bin_t bin)
{
	uint32_t sec_id;
	uint32_t sec_sz;
	uint32_t rsz;

	while (*pres > 0) {
		RS_ASSERT(bload32(pbuf, pres, &sec_id) == 0, RS_EINV);
		RS_ASSERT(bload32(pbuf, pres, &sec_sz) == 0, RS_EINV);

		if(sec_id == RS_BIN_SECTION_CODE ||
		    sec_id == RS_BIN_SECTION_ATTR) {
			uint32_t size;
			uint8_t *data;
			RS_CALL(load_data_section(pbuf, pres, &size, &data, &rsz));
			RS_CALL(rs_bin_add_data_section(bin, sec_id, sec_sz, size, data));
			RS_ASSERT(rsz <= sec_sz, RS_EINV);
			sec_sz -= rsz;
		}
		else {
			uint32_t size = sec_sz;
			uint8_t *data;
			RS_CALL(load_general_section(pbuf, pres, size, &data, &rsz));
			RS_CALL(rs_bin_add_general_section(bin, 0, size, data));
			RS_ASSERT(rsz == sec_sz, RS_EINV);
			sec_sz -= size;
		}

		RS_ASSERT(sec_sz <= *pres, RS_EINV);
		*pbuf = ((uint8_t *)*pbuf) + sec_sz;
		*pres -= sec_sz;
	}

	return RS_OK;
}

static void update_body_size(rs_bin_t bin)
{
	struct rs_bin_section_s *section;
	uint32_t body_size = 0;

	for(section = bin->body; section != NULL; section = section->next) {
		if(section->id == RS_BIN_SECTION_CODE ||
		    section->id == RS_BIN_SECTION_ATTR) {
			struct rs_bin_data_section_s *data_section = (struct rs_bin_data_section_s *)section;
			section->size = get_data_section_size(data_section->size);
		}
	}
	for(section = bin->body; section != NULL; section = section->next) {
		body_size += section_header_size + section->size;
	}
	bin->header.body_size = body_size;
}

RS_IF rs_ret_t rs_bin_alloc(rs_bin_t *pbin)
{
	rs_bin_t bin;

	RS_ASSERT(pbin != NULL, RS_EFAULT);

	*pbin = NULL;

	bin = (struct rs_bin_s *) calloc(1, sizeof(struct rs_bin_s));
	RS_ASSERT(bin != NULL, RS_ENOMEM);

	bin->header.magic = header_magic;
	bin->header.reserved1 = header_reserved1;
	bin->header.major_ver = RS_BIN_VERSION_MAJOR;
	bin->header.minor_ver = RS_BIN_VERSION_MINOR;
	bin->header.body_size = 0u;
	bin->header.reserved2 = header_reserved2;
	bin->header.reserved3 = header_reserved3;

	bin->body = NULL;

	bin->valid = VALID_MARKER;
	*pbin = bin;

	return RS_OK;
}

RS_IF rs_ret_t rs_bin_free(rs_bin_t bin)
{
	struct rs_bin_section_s *p;

	RS_ASSERT(bin != NULL, RS_EINV);
	RS_ASSERT(bin->valid == VALID_MARKER, RS_EINV);

	for(p = bin->body; p != NULL; /* */) {
		struct rs_bin_section_s *next = p->next;
		free_section(p);
		p = next;
	}

	bin->valid = 0;

	free(bin);

	return RS_OK;
}

RS_IF rs_ret_t rs_bin_add_code_section(rs_bin_t bin, uint8_t *data, uint32_t size)
{
	RS_ASSERT(bin != NULL, RS_EINV);
	RS_ASSERT(bin->valid == VALID_MARKER, RS_EINV);
	RS_ASSERT(data != NULL, RS_EFAULT);

	return rs_bin_add_data_section(bin, RS_BIN_SECTION_CODE, get_data_section_size(size), size, data);
}

RS_IF rs_ret_t rs_bin_add_attr_section(rs_bin_t bin, uint8_t *data, uint32_t size)
{
	RS_ASSERT(bin != NULL, RS_EINV);
	RS_ASSERT(bin->valid == VALID_MARKER, RS_EINV);
	RS_ASSERT(data != NULL, RS_EFAULT);

	return rs_bin_add_data_section(bin, RS_BIN_SECTION_ATTR, get_data_section_size(size), size, data);
}

RS_IF rs_ret_t rs_bin_add_data_section(rs_bin_t bin, uint32_t id, uint32_t sz, uint32_t size, uint8_t *data)
{
	struct rs_bin_data_section_s *p;

	RS_ASSERT(bin != NULL, RS_EINV);
	RS_ASSERT(bin->valid == VALID_MARKER, RS_EINV);
	RS_ASSERT(data != NULL, RS_EFAULT);

	p = (struct rs_bin_data_section_s *)calloc(1, sizeof(struct rs_bin_data_section_s));
	RS_ASSERT(p != NULL, RS_ENOMEM);
	p->base.id = id;
	p->base.size = sz;
	p->size = size;
	p->data = data;
	add_section(bin, &p->base);

	return RS_OK;
}

RS_IF rs_ret_t rs_bin_add_general_section(rs_bin_t bin, uint32_t id, uint32_t sz, uint8_t *data)
{
	struct rs_bin_general_section_s *p;

	RS_ASSERT(bin != NULL, RS_EINV);
	RS_ASSERT(bin->valid == VALID_MARKER, RS_EINV);
	RS_ASSERT(data != NULL, RS_EFAULT);

	p = (struct rs_bin_general_section_s *)calloc(1, sizeof(struct rs_bin_general_section_s));
	RS_ASSERT(p != NULL, RS_ENOMEM);
	p->base.id = id;
	p->base.size = sz;
	p->data = data;
	add_section(bin, &p->base);

	return RS_OK;
}

RS_IF rs_ret_t rs_bin_load(rs_bin_t bin, const void *buf, uint32_t bufsize)
{
	size_t res = bufsize;
	struct rs_bin_section_s *section;
	uint32_t body_size = 0;

	RS_ASSERT(bin != NULL, RS_EINV);
	RS_ASSERT(bin->valid == VALID_MARKER, RS_EINV);
	RS_ASSERT(buf != NULL, RS_EFAULT);

	RS_ASSERT(bload8(&buf, &res, &bin->header.magic) == 0, RS_EFORM);
	RS_ASSERT(bload8(&buf, &res, &bin->header.reserved1) == 0, RS_EFORM);
	RS_ASSERT(bload8(&buf, &res, &bin->header.major_ver) == 0, RS_EFORM);
	RS_ASSERT(bload8(&buf, &res, &bin->header.minor_ver) == 0, RS_EFORM);
	RS_ASSERT(bload32(&buf, &res, &bin->header.body_size) == 0, RS_EFORM);
	RS_ASSERT(bload8(&buf, &res, &bin->header.gen_major_ver) == 0, RS_EFORM);
	RS_ASSERT(bload8(&buf, &res, &bin->header.gen_minor_ver) == 0, RS_EFORM);
	RS_ASSERT(bload8(&buf, &res, &bin->header.gen_revision_ver) == 0, RS_EFORM);
	RS_ASSERT(bload8(&buf, &res, &bin->header.gen_hotfix_ver) == 0, RS_EFORM);
	RS_ASSERT(bload32(&buf, &res, &bin->header.reserved2) == 0, RS_EFORM);
	RS_ASSERT(bload16(&buf, &res, &bin->header.hw_type) == 0, RS_EFORM);
	RS_ASSERT(bload16(&buf, &res, &bin->header.hw_major_ver) == 0, RS_EFORM);
	RS_ASSERT(bload16(&buf, &res, &bin->header.hw_minor_ver) == 0, RS_EFORM);
	RS_ASSERT(bload16(&buf, &res, &bin->header.reserved3) == 0, RS_EFORM);

	RS_ASSERT(check_header(&bin->header) == RS_OK, RS_EFORM);

	RS_CALL(load_sections(&buf, &res, bin));

	RS_ASSERT(res == 0, RS_EFORM);

	for(section = bin->body; section != NULL; section = section->next) {
		body_size += section_header_size + section->size;
	}
	RS_ASSERT(bin->header.body_size == body_size, RS_EFORM);

	return RS_OK;
}

RS_IF rs_ret_t rs_bin_save(rs_bin_t bin, void *buf, uint32_t bufsize)
{
	size_t res = bufsize;
	uint32_t size;

	RS_ASSERT(bin != NULL, RS_EINV);
	RS_ASSERT(bin->valid == VALID_MARKER, RS_EINV);
	RS_ASSERT(buf != NULL, RS_EFAULT);

	RS_CALL(rs_bin_get_size(bin, &size));
	RS_ASSERT(bufsize == size, RS_EINV);

	update_body_size(bin);

	RS_CALL(check_header(&bin->header));

	RS_ASSERT(bsave8(&buf, &res, bin->header.magic) == 0, RS_EINV);
	RS_ASSERT(bsave8(&buf, &res, bin->header.reserved1) == 0, RS_EINV);
	RS_ASSERT(bsave8(&buf, &res, bin->header.major_ver) == 0, RS_EINV);
	RS_ASSERT(bsave8(&buf, &res, bin->header.minor_ver) == 0, RS_EINV);
	RS_ASSERT(bsave32(&buf, &res, bin->header.body_size) == 0, RS_EINV);
	RS_ASSERT(bsave8(&buf, &res, bin->header.gen_major_ver) == 0, RS_EINV);
	RS_ASSERT(bsave8(&buf, &res, bin->header.gen_minor_ver) == 0, RS_EINV);
	RS_ASSERT(bsave8(&buf, &res, bin->header.gen_revision_ver) == 0, RS_EINV);
	RS_ASSERT(bsave8(&buf, &res, bin->header.gen_hotfix_ver) == 0, RS_EINV);
	RS_ASSERT(bsave32(&buf, &res, bin->header.reserved2) == 0, RS_EINV);
	RS_ASSERT(bsave16(&buf, &res, bin->header.hw_type) == 0, RS_EINV);
	RS_ASSERT(bsave16(&buf, &res, bin->header.hw_major_ver) == 0, RS_EINV);
	RS_ASSERT(bsave16(&buf, &res, bin->header.hw_minor_ver) == 0, RS_EINV);
	RS_ASSERT(bsave16(&buf, &res, bin->header.reserved3) == 0, RS_EINV);

	RS_CALL(save_sections(&buf, &res, bin));

	return RS_OK;
}

RS_IF rs_ret_t rs_bin_get_size(rs_bin_t bin, uint32_t *size)
{
	struct rs_bin_section_s *section;

	RS_ASSERT(bin != NULL, RS_EINV);
	RS_ASSERT(size != NULL, RS_EFAULT);

	*size = header_size;
	for(section = bin->body; section != NULL; section = section->next) {
		*size += section_header_size + section->size;
	}

	return RS_OK;
}

RS_IF rs_ret_t rs_bin_set_gen(rs_bin_t bin, uint8_t gen_major_ver, uint8_t gen_minor_ver, uint8_t gen_revision_ver, uint8_t gen_hotfix_ver)
{
	RS_ASSERT(bin != NULL, RS_EINV);

	bin->header.gen_major_ver = gen_major_ver;
	bin->header.gen_minor_ver = gen_minor_ver;
	bin->header.gen_revision_ver = gen_revision_ver;
	bin->header.gen_hotfix_ver = gen_hotfix_ver;

	return RS_OK;
}

RS_IF rs_ret_t rs_bin_get_gen(rs_bin_t bin, uint8_t *gen_major_ver, uint8_t *gen_minor_ver, uint8_t *gen_revision_ver, uint8_t *gen_hotfix_ver)
{
	RS_ASSERT(bin != NULL, RS_EINV);

	if(gen_major_ver != NULL) *gen_major_ver = bin->header.gen_major_ver;
	if(gen_minor_ver != NULL) *gen_minor_ver = bin->header.gen_minor_ver;
	if(gen_revision_ver != NULL) *gen_revision_ver = bin->header.gen_revision_ver;
	if(gen_hotfix_ver != NULL) *gen_hotfix_ver = bin->header.gen_hotfix_ver;

	return RS_OK;
}

RS_IF rs_ret_t rs_bin_set_hw(rs_bin_t bin, uint16_t hw_type, uint16_t hw_major_ver, uint16_t hw_minor_ver)
{
	RS_ASSERT(bin != NULL, RS_EINV);

	bin->header.hw_type = hw_type;
	bin->header.hw_major_ver = hw_major_ver;
	bin->header.hw_minor_ver = hw_minor_ver;

	return RS_OK;
}

RS_IF rs_ret_t rs_bin_get_hw(rs_bin_t bin, uint16_t *hw_type, uint16_t *hw_major_ver, uint16_t *hw_minor_ver)
{
	RS_ASSERT(bin != NULL, RS_EINV);

	if(hw_type != NULL) *hw_type = bin->header.hw_type;
	if(hw_major_ver != NULL) *hw_major_ver = bin->header.hw_major_ver;
	if(hw_minor_ver != NULL) *hw_minor_ver = bin->header.hw_minor_ver;

	return RS_OK;
}

RS_IF rs_ret_t rs_bin_get_code_section(rs_bin_t bin, struct rs_bin_data_section_s **p)
{
	RS_ASSERT(bin != NULL, RS_EINV);
	RS_ASSERT(p != NULL, RS_EFAULT);

	return rs_bin_get_data_section(bin, RS_BIN_SECTION_CODE, p);
}

RS_IF rs_ret_t rs_bin_get_attr_section(rs_bin_t bin, struct rs_bin_data_section_s **p)
{
	RS_ASSERT(bin != NULL, RS_EINV);
	RS_ASSERT(p != NULL, RS_EFAULT);

	return rs_bin_get_data_section(bin, RS_BIN_SECTION_ATTR, p);
}

RS_IF rs_ret_t rs_bin_get_data_section(rs_bin_t bin, uint32_t id, struct rs_bin_data_section_s **p)
{
	struct rs_bin_section_s *section;

	RS_ASSERT(bin != NULL, RS_EINV);
	RS_ASSERT(p != NULL, RS_EFAULT);

	for(section = bin->body; section != NULL; section = section->next) {
		if(section->id == id) {
			*p = (struct rs_bin_data_section_s *)section;
			return RS_OK;
		}
	}

	return RS_EINV;
}
