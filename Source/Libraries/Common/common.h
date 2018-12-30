#ifndef __COMMON_H__
#define __COMMON_H__

#include "types.h"

int32_t  str2hex(uint8_t *byte, char *str, uint32_t len);
uint16_t ntohs(uint16_t i);
uint32_t ntohl(uint32_t i);
uint64_t ntohll(uint64_t i);
uint8_t  cheksum_exclusive_or1(uint8_t *buf, uint16_t len);

void     lock(void);
void     unlock(void);

uint16_t rbuf2(uint8_t *buf);
uint32_t rbuf4(uint8_t *buf);
uint64_t rbuf8(uint8_t *buf);

#endif