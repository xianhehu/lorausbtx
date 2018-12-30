#include "verify.h"
#include "app.h"
#include "base64.h"
#include <string.h>

#define ID_LEN  12

char    verify_passwd[100] = {0};
uint8_t verify_passwdlen   = 0;

int VERIFY_ReadId(uint8_t *data, uint8_t size)
{
    if (size < 12) {
        return -1;
    }

    uint32_t stm32Id;

    stm32Id=*(volatile uint32_t*)(0x1ffff7e8);
    
    memcpy(data, &stm32Id, 4);
    
    stm32Id=*(volatile uint32_t*)(0x1ffff7ec);
    
    memcpy(data+4, &stm32Id, 4);
    
    stm32Id=*(volatile uint32_t*)(0x1ffff7f0);
    
    memcpy(data+8, &stm32Id, 4);

    return 12;
}

int VERIFY_SetPasswd(char* pass, uint8_t len)
{
    if (len > sizeof(verify_passwd)) {
        return -1;
    }

    memcpy(verify_passwd, pass, len);

    verify_passwdlen = len;

    return 0;
}

inline int VERIFY_Check(void)
{
    const char* userinfo = "solarcdc1234";
    const char* passhead = "solar000";
    uint8_t     hlen     = strlen(passhead);
    char*       password = Malloc(hlen+ID_LEN);
    char*       base64   = Malloc(verify_passwdlen+1);
    int         ret      = 0;

    memcpy(password, passhead, hlen);
    VERIFY_ReadId((uint8_t *)&password[hlen], ID_LEN);

    for (int i = 0; i < ID_LEN; i++) {
        password[hlen+i] += userinfo[i];
    }

    bin_to_b64((const uint8_t *)password, hlen+ID_LEN, base64, verify_passwdlen+1);

    for (int i = 0; i < verify_passwdlen; i++) {
        if (verify_passwd[i] != base64[i]) {
            ret = -1;

            break;
        }
    }
    
    MFree(base64);
    MFree(password);

    return ret;
}

