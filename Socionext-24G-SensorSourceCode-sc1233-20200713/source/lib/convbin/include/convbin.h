/*
 * Copyright (c) 2018-2019 Socionext Inc.
 * All rights reserved.
 *
 */

#ifndef	__CONVBIN_H__
#define	__CONVBIN_H__

#include <stdint.h>
#include <string.h>

#define CONVBIN_SIZE_FP			32

//
// functions for C/C++
//

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

	//
	// bXXXXYY()	save/load unsigned value (big endian)
	// bXXXXYYs()	save/load signed value (big endian)
	// bXXXXYYl()	save/load signed value (little endian)
	//
	//   XXXX: save or load
	//   YY:   8/16/32/64/fp(float)
	//

	static int bsave8(void **buf, size_t *res, uint8_t data);
	static int bsave16(void **buf, size_t *res, uint16_t data);
	static int bsave32(void **buf, size_t *res, uint32_t data);
	static int bsave64(void **buf, size_t *res, uint64_t data);
	static int bsave8a(void **buf, size_t *res, const uint8_t *data, size_t size);
	static int bsave16a(void **buf, size_t *res, const uint16_t *data, size_t size);
	static int bsave32a(void **buf, size_t *res, const uint32_t *data, size_t size);
	static int bsave16s(void **buf, size_t *res, int16_t data);
	static int bsave32s(void **buf, size_t *res, int32_t data);
	static int bsave16l(void **buf, size_t *res, uint16_t data);
	static int bsave32l(void **buf, size_t *res, uint32_t data);

	static int bload8(const void **buf, size_t *res, uint8_t *data);
	static int bload16(const void **buf, size_t *res, uint16_t *data);
	static int bload32(const void **buf, size_t *res, uint32_t *data);
	static int bload64(const void **buf, size_t *res, uint64_t *data);
	static int bload8a(const void **buf, size_t *res, uint8_t *data, size_t size);
	static int bload16a(const void **buf, size_t *res, uint16_t *data, size_t size);
	static int bload32a(const void **buf, size_t *res, uint32_t *data, size_t size);
	static int bload16s(const void **buf, size_t *res, int16_t *data);
	static int bload32s(const void **buf, size_t *res, int32_t *data);
	static int bload16l(const void **buf, size_t *res, uint16_t *data);
	static int bload32l(const void **buf, size_t *res, uint32_t *data);

	extern int bsavefp(void **buf, size_t *res, float data);
	extern int bloadfp(const void **buf, size_t *res, float *data);

	static int bfill8(void **buf, size_t *res, uint8_t data, size_t size);
	static int bskip8(const void **buf, size_t *res, size_t size);

#ifdef __cplusplus
}
#endif  /* __cplusplus */


//
// body of functions
//
#define CONVBIN_CHECK_SIZE(size)		{if((*res) < (size)) return -1;}
#define CONVBIN_SET(pos, val)			{((uint8_t *) *buf)[pos] = val;}
#define CONVBIN_GET(pos)				((const uint8_t *) *buf)[pos]
#define CONVBIN_INC_SIZE(size)			{*buf = ((uint8_t *) *buf) + (size); *res -= (size);}

static __inline int bsave8(void **buf, size_t *res, uint8_t data)
{
	CONVBIN_CHECK_SIZE(sizeof(data));
	CONVBIN_SET(0, data);
	CONVBIN_INC_SIZE(sizeof(data));
	return 0;
}

static __inline int bsave16(void **buf, size_t *res, uint16_t data)
{
	CONVBIN_CHECK_SIZE(sizeof(data));
	CONVBIN_SET(0, (data >>  8) & 0xff);
	CONVBIN_SET(1, (data >>  0) & 0xff);
	CONVBIN_INC_SIZE(sizeof(data));
	return 0;
}

static __inline int bsave16l(void **buf, size_t *res, uint16_t data)
{
	CONVBIN_CHECK_SIZE(sizeof(data));
	CONVBIN_SET(1, (data >>  8) & 0xff);
	CONVBIN_SET(0, (data >>  0) & 0xff);
	CONVBIN_INC_SIZE(sizeof(data));
	return 0;
}

static __inline int bsave32(void **buf, size_t *res, uint32_t data)
{
	CONVBIN_CHECK_SIZE(sizeof(data));
	CONVBIN_SET(0, (data >> 24) & 0xff);
	CONVBIN_SET(1, (data >> 16) & 0xff);
	CONVBIN_SET(2, (data >>  8) & 0xff);
	CONVBIN_SET(3, (data >>  0) & 0xff);
	CONVBIN_INC_SIZE(sizeof(data));
	return 0;
}

static __inline int bsave32l(void **buf, size_t *res, uint32_t data)
{
	CONVBIN_CHECK_SIZE(sizeof(data));
	CONVBIN_SET(3, (data >> 24) & 0xff);
	CONVBIN_SET(2, (data >> 16) & 0xff);
	CONVBIN_SET(1, (data >>  8) & 0xff);
	CONVBIN_SET(0, (data >>  0) & 0xff);
	CONVBIN_INC_SIZE(sizeof(data));
	return 0;
}

static __inline int bsave64(void **buf, size_t *res, uint64_t data)
{
	if(bsave32(buf, res, (data >> 32) & 0xffffffff) != 0) {
		return -1;
	}
	if(bsave32(buf, res, (data >>  0) & 0xffffffff) != 0) {
		return -1;
	}
	return 0;
}

static __inline int bsave8a(void **buf, size_t *res, const uint8_t *data, size_t size)
{
	CONVBIN_CHECK_SIZE(size);
	memcpy(*buf, data, size);
	CONVBIN_INC_SIZE(size);
	return 0;
}

static __inline int bsave16a(void **buf, size_t *res, const uint16_t *data, size_t size)
{
	{
		size_t i;
		for(i=0; i<size; i++) {
			int ret = bsave16(buf, res, data[i]);
			if(ret != 0) {
				return ret;
			}
		}
	}
	return 0;
}

static __inline int bsave32a(void **buf, size_t *res, const uint32_t *data, size_t size)
{
	{
		size_t i;
		for(i=0; i<size; i++) {
			int ret = bsave32(buf, res, data[i]);
			if(ret != 0) {
				return ret;
			}
		}
	}
	return 0;
}

static __inline int bload8(const void **buf, size_t *res, uint8_t *data)
{
	CONVBIN_CHECK_SIZE(sizeof(*data));
	*data = CONVBIN_GET(0);
	CONVBIN_INC_SIZE(sizeof(*data));
	return 0;
}

static __inline int bload16(const void **buf, size_t *res, uint16_t *data)
{
	CONVBIN_CHECK_SIZE(sizeof(*data));
	*data = 0;
	*data |= ((uint16_t) CONVBIN_GET(0)) <<  8;
	*data |= ((uint16_t) CONVBIN_GET(1)) <<  0;
	CONVBIN_INC_SIZE(sizeof(*data));
	return 0;
}

static __inline int bload16l(const void **buf, size_t *res, uint16_t *data)
{
	CONVBIN_CHECK_SIZE(sizeof(*data));
	*data = 0;
	*data |= ((uint16_t) CONVBIN_GET(1)) <<  8;
	*data |= ((uint16_t) CONVBIN_GET(0)) <<  0;
	CONVBIN_INC_SIZE(sizeof(*data));
	return 0;
}

static __inline int bload32(const void **buf, size_t *res, uint32_t *data)
{
	CONVBIN_CHECK_SIZE(sizeof(*data));
	*data = 0;
	*data |= ((uint32_t) CONVBIN_GET(0)) << 24;
	*data |= ((uint32_t) CONVBIN_GET(1)) << 16;
	*data |= ((uint32_t) CONVBIN_GET(2)) <<  8;
	*data |= ((uint32_t) CONVBIN_GET(3)) <<  0;
	CONVBIN_INC_SIZE(sizeof(*data));
	return 0;
}

static __inline int bload32l(const void **buf, size_t *res, uint32_t *data)
{
	CONVBIN_CHECK_SIZE(sizeof(*data));
	*data = 0;
	*data |= ((uint32_t) CONVBIN_GET(3)) << 24;
	*data |= ((uint32_t) CONVBIN_GET(2)) << 16;
	*data |= ((uint32_t) CONVBIN_GET(1)) <<  8;
	*data |= ((uint32_t) CONVBIN_GET(0)) <<  0;
	CONVBIN_INC_SIZE(sizeof(*data));
	return 0;
}

static __inline int bload64(const void **buf, size_t *res, uint64_t *data)
{
	uint32_t d0 = 0;
	uint32_t d1 = 0;
	if(bload32(buf, res, &d0) != 0) {
		return -1;
	}
	if(bload32(buf, res, &d1) != 0) {
		return -1;
	}
	*data = (((uint64_t) d0) << 32) | ((uint64_t) d1);
	return 0;
}

static __inline int bload8a(const void **buf, size_t *res, uint8_t *data, size_t size)
{
	CONVBIN_CHECK_SIZE(size);
	memcpy(data, *buf, size);
	CONVBIN_INC_SIZE(size);
	return 0;
}

static __inline int bload16a(const void **buf, size_t *res, uint16_t *data, size_t size)
{
	{
		size_t i;
		for(i=0; i<size; i++) {
			int ret = bload16(buf, res, &(data[i]));
			if(ret != 0) {
				return ret;
			}
		}
	}
	return 0;
}

static __inline int bload32a(const void **buf, size_t *res, uint32_t *data, size_t size)
{
	{
		size_t i;
		for(i=0; i<size; i++) {
			int ret = bload32(buf, res, &(data[i]));
			if(ret != 0) {
				return ret;
			}
		}
	}
	return 0;
}

static __inline int bsave16s(void **buf, size_t *res, int16_t data)
{
	return bsave16(buf, res, ((uint16_t) data));
}

static __inline int bload16s(const void **buf, size_t *res, int16_t *data)
{
	return bload16(buf, res, ((uint16_t *) data));
}

static __inline int bsave32s(void **buf, size_t *res, int32_t data)
{
	return bsave32(buf, res, ((uint32_t) data));
}

static __inline int bload32s(const void **buf, size_t *res, int32_t *data)
{
	return bload32(buf, res, ((uint32_t *) data));
}

static __inline int bfill8(void **buf, size_t *res, uint8_t data, size_t size)
{
	CONVBIN_CHECK_SIZE(size);
	memset(*buf, data, size);
	CONVBIN_INC_SIZE(size);
	return 0;
}

static __inline int bskip8(const void **buf, size_t *res, size_t size)
{
	CONVBIN_CHECK_SIZE(size);
	CONVBIN_INC_SIZE(size);
	return 0;
}

#endif	/* __CONVBIN_H__ */
