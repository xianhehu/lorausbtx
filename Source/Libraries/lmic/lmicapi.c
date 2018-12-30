#include "lmicapi.h"
#include "lmic.h"
#include "osal.h"
#include "stm32f0xx_flash.h"

#define FLASH_PARAM_ADDR            0x0803F800
#define FLASH_DEVID_ADDR            0x0803FC00

flash_param_t  flash_param={0};

static void FLASH_Write(uint32_t addr, uint32_t *buf, uint32_t len)
{
    for (int i=0; i<len; i++) {
        FLASH_ProgramWord(addr+(i<<2), buf[i]);
    }
}


void lmicapi_init(void)
{
    lmic_init();
    os_init();
}

void lmicapi_start(void)
{
    os_init();
    lmic_start();
}

void lmicapi_stop(void)
{
    lmic_stop();
}

void lmicapi_reset(void)
{
    lmicapi_init();
    lmic_stop();
}

int lmicapi_getid(uint8_t *id)
{
    os_getDevEui(id);
    return 0;
}

int lmicapi_setid(uint8_t *id)
{
    FLASH_Unlock();
    FLASH_Write(FLASH_DEVID_ADDR, (uint32_t *)id, 2);
    os_setDevEui(id);
    
    return 0;
}

int lmicapi_setappkey(uint8_t *key)
{
    os_setDevKey(key);
    memcpy(flash_param.appkey, key, 16);
    lmicapi_saveparam();
    return 0;
}

int lmicapi_getappkey(uint8_t *key)
{
    os_getDevKey(key);
    return 0;
}

int lmicapi_setappeui(uint8_t *eui)
{
    os_setArtEui(eui);
    memcpy(flash_param.appeui, eui, 8);
    lmicapi_saveparam();
    return 0;
}

int lmicapi_getappeui(uint8_t *eui)
{
    os_getArtEui(eui);
    return 0;
}

int lmicapi_setbasefreq(uint32_t txfreq, uint32_t rxfreq)
{
    return lmic_setbasefreq(txfreq, rxfreq);
}

int lmicapi_setband(uint8_t band)
{
    return lmic_setband(band);
}

int lmicapi_setsf(uint8_t sf)
{
    return lmic_setsf(sf);
}

int lmicapi_setrx2param(uint32_t freq, uint8_t dr)
{
    return lmic_setrx2param(freq, dr);
}

void lmicapi_getrx2param(uint32_t *freq, uint8_t *dr)
{
    lmic_getrx2param(freq, dr);
}

int lmicapi_send(uint8_t port, uint8_t *data, uint8_t len, uint8_t mode)
{
    if (!lmic_isconnect()) {
        return -1; /* net break */
    }

    return lmic_send(port, data, len, mode);
}

int lmicapi_transstatus(void)
{
    if (!lmic_isconnect()) {
        return -1;
    }
    if (!lmic_idle()) {
        return -2;
    }
    return lmic_gettrasstatus();
}

int lmicapi_receive(uint8_t *data, uint8_t *len)
{
    if (!lmic_isconnect()) {
        return -1; /* net break */
    }
    *len = lmic_receive(data);
    return *len;
}

int lmicapi_netstatus(void)
{
    if (!lmic_isconnect()) {
        return -1; /* net break */
    }
    if (!lmic_idle()) {
        return 0;  /* net busy */
    }
    return 1;  /* net connect and idle */
}

void lmicapi_saveparam(void)
{
    FLASH_Unlock();
    FLASH_ErasePage(FLASH_PARAM_ADDR);
    FLASH_ProgramWord(FLASH_PARAM_ADDR, 0xaa);
    flash_param.valid=1;
    FLASH_Write(FLASH_PARAM_ADDR+4, (uint32_t *)&flash_param, sizeof(flash_param_t)>>2);
}

void lmicapi_readparam(void)
{
    FLASH_Unlock();
    uint32_t magic=*(uint32_t *)FLASH_PARAM_ADDR;
    if (magic != 0xaa) {
        flash_param.valid=0;
        return;
    }
    memcpy(&flash_param, (const char *)FLASH_PARAM_ADDR+4, sizeof(flash_param_t));
}

void lmicapi_readid(void)
{
    int64_t id=*(int64_t *)FLASH_DEVID_ADDR;
    
    if (id==-1) {
        return;
    }
    FLASH_Unlock();
    os_setDevEui((uint8_t *)FLASH_DEVID_ADDR);
}

#if 0
void flashparam_test()
{
    flash_param.active=1;
    flash_param.lclass=2;
    flash_param.devaddr=6000;
    flash_param.gaddr=7000;
    
    for (int i=0; i<8; i++) {
        flash_param.appeui[i]=i+127;
    }
    for (int i=0; i<16; i++) {
        flash_param.appkey[i]=i+128;
    }
    
    flash_param.rxdroffset=5;
    
    lmicapi_saveparam();
    
}
#endif

