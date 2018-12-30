#include "common.h"

static int char2byte(char c, uint8_t *byte)
{
    if (c >= 'A' && c <= 'F') {
        *byte = c-'A'+0xA;
        return 0;
    }
    if (c >= 'a' && c <= 'f') {
        *byte = c-'a'+0xA;
        return 0;
    }
    if (c >= '0' && c <= '9') {
        *byte = c-'0';
        return 0;
    }
    return -1;
}

int str2hex(uint8_t *byte, char *str, uint32_t len)
{
    if (len % 2 != 0) {
        return -1;
    }
    
    for (uint32_t i=0; i<len; i++) {
        uint8_t tmp = 0;
        if (char2byte(str[i], &tmp) < 0) {
            return -1;
        }
        if ((i&1) == 0) {
            byte[i>>1] = tmp << 4;
        }
        else {
            byte[i>>1] += tmp;
        }
    }

    return 0;
}

uint16_t ntohs(uint16_t i)
{
    uint16_t tmp;
    
    tmp=i>>8;
    tmp+=(i&0xff)<<8;
    
    return tmp;
}

uint32_t ntohl(uint32_t i)
{
    return (ntohs(i&0xffff)<<16)+ntohs(i>>16);
}

uint64_t ntohll(uint64_t i)
{
    uint64_t tmp=0;
    
    tmp=ntohl(i&0xffffffff);
    tmp<<=32;
    tmp+=ntohs(i>>32);
    
    return tmp;
}

uint8_t cheksum_exclusive_or1(uint8_t *buf, uint16_t len)
{
    uint8_t sum=0;
    
    for (int i=0; i<len; i++) {
        sum ^= buf[i];
    }
    
    return sum;
}

uint16_t rbuf2(uint8_t *buf)
{
    uint16_t tmp = {0};
    
    memcpy(&tmp, buf, sizeof(tmp));
    
    return tmp;
}

uint32_t rbuf4(uint8_t *buf)
{
    uint32_t tmp = {0};
    
    memcpy(&tmp, buf, sizeof(tmp));
    
    return tmp;
}

uint64_t rbuf8(uint8_t *buf)
{
    uint64_t tmp = {0};
    
    memcpy(&tmp, buf, sizeof(tmp));
    
    return tmp;
}