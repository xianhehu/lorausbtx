/*
 * Thanks to Mathias Panzenböck aka panzi
 * https://gist.github.com/panzi/6856583
 */
#ifndef PORTABLE_ENDIAN_H__
#define PORTABLE_ENDIAN_H__

#include "common.h"

#if 1
#define htobe16 ntohs
#define htole16(x) (x)
#define be16toh ntohs
#define le16toh(x) (x)
 
#define htobe32 ntohl
#define htole32(x) (x)
#define be32toh ntohl
#define le32toh(x) (x)

#define htobe64 ntohll
#define htole64(x) (x)
#define be64toh ntohll
#define le64toh(x) (x)
#endif

#endif //#ifndef PORTABLE_ENDIAN_H__
