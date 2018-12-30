#include "at.h"
#include "osal.h"
#include "Drv_Usart.h"
#include "Drv_power.h"
#include "Drv_rtc.h"
#include "Drv_SX127x_IOCTL.h"
#include "Drv_sx127x.h"
#include "lmicapi.h"
#include "lmiclowlevelapi.h"
#include "oslmic.h"
#include <string.h>
#include <stdarg.h>

typedef void(*at_func_t)(char *param, uint32_t len);

typedef struct {
    char *name;
    at_func_t func;
} at_cmd_t;

/* at command handle function */
static void ATZ_Handle(char *param, uint32_t len);
static void AT_IdQHandle(char *param, uint32_t len);
static void AT_IdSHandle(char *param, uint32_t len);
static void AT_NsHandle(char *param, uint32_t len);
static void AT_RxHandle(char *param, uint32_t len);
static void AT_AppKeySHandle(char *param, uint32_t len);
static void AT_AppEuiSHandle(char *param, uint32_t len);
static void AT_AppKeyQHandle(char *param, uint32_t len);
static void AT_AppEuiQHandle(char *param, uint32_t len);
static void AT_NetHandle(char *param, uint32_t len);
static void AT_TxHandle(char *param, uint32_t len);
static void AT_TsHandle(char *param, uint32_t len);
static void AT_BaseFreqHandle(char *param, uint32_t len);
static void AT_BandSHandle(char *param, uint32_t len);
static void AT_SfSHandle(char *param, uint32_t len);
static void AT_Rx2ParamSHandle(char *param, uint32_t len);
static void AT_Rx2ParamQHandle(char *param, uint32_t len);
static void AT_ElpHandle(char *param, uint32_t len);
static void AT_IOHandle(char *param, uint32_t len);
static void AT_CommTestHandle(char *param, uint32_t len);
static void AT_TxTestHandle(char *param, uint32_t len);
static void AT_RxTestHandle(char *param, uint32_t len);

/* at command handle list */
static const at_cmd_t at_short_cmds[] = {
    {"ATZ", ATZ_Handle},
};

static const at_cmd_t at_que_cmds[] = {
    {"AT+ID?",  AT_IdQHandle},
    {"AT+NS?",  AT_NsHandle},
    {"AT+RX?",  AT_RxHandle},
    {"AT+TS?",  AT_TsHandle},
    {"AT+AK?",  AT_AppKeyQHandle},
    {"AT+AEUI?",AT_AppEuiQHandle},
    {"AT+RX2P?",AT_Rx2ParamQHandle}
    
};

static const at_cmd_t at_equ_cmds[] = {
    {"AT+ID=",  AT_IdSHandle},
    {"AT+AK=",  AT_AppKeySHandle},
    {"AT+AEUI=",AT_AppEuiSHandle},
    {"AT+NET=", AT_NetHandle},
    {"AT+TX=",  AT_TxHandle},
    {"AT+BF=",  AT_BaseFreqHandle},
    {"AT+BAND=",AT_BandSHandle},
    {"AT+SF=",  AT_SfSHandle},
    {"AT+RX2P=",AT_Rx2ParamSHandle},
    {"AT+ELP=", AT_ElpHandle},
    {"AT+IO=",  AT_IOHandle},
    {"AT+TTST=",AT_TxTestHandle},
    {"AT+RTST=",AT_RxTestHandle},
    {"AT+COMTST=", AT_CommTestHandle}
};

/* at command ack print */
void AT_PrintAck(char *ack)
{
    Drv_Usart_Write((uint8_t *)ack, strlen(ack), PORT_USART1);
}

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

static int str2hex(uint8_t *byte, char *str, uint32_t len)
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

static void ATZ_Handle(char *param, uint32_t len)
{
    AT_PrintAck("\r\nOK\r\n");
}

static void AT_IdQHandle(char *param, uint32_t len)
{
    uint8_t buf[8];
    char ack[5] = {0};

    lmicapi_getid(buf);
    
    /* 小端转大端 */
    for (int i=0; i<4; i++) {
        uint8_t tmp=0;
        tmp=buf[i];
        buf[i]=buf[7-i];
        buf[7-i]=tmp;
    }

    AT_PrintAck("\r\n");
    for (uint8_t i=0; i<8; i++) {
        memset(ack, 0, sizeof(ack));
        snprintf(ack, sizeof(ack)-1, "%02X", buf[i]);
        AT_PrintAck(ack);
    }
    AT_PrintAck("\r\n");
}

static void AT_IdSHandle(char *param, uint32_t len)
{
    if (len!=16) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }
    
    uint8_t id[8]={0};
    if (str2hex(id, param, 16) != 0) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }
    
    /* 大端转小端 */
    for (int i=0; i<4; i++) {
        uint8_t tmp=0;
        tmp=id[i];
        id[i]=id[7-i];
        id[7-i]=tmp;
    }
    
    lmicapi_setid(id);
    AT_PrintAck("\r\nOK\r\n");
}

static void AT_NsHandle(char *param, uint32_t len)
{
    int ret = lmicapi_netstatus();
    if (ret < 0) {
        AT_PrintAck("\r\nER03\r\n");
        return;
    }
    if (ret == 0) {
        AT_PrintAck("\r\nER04\r\n");
        return;
    }
    AT_PrintAck("\r\nOK\r\n");
}

static void AT_RxHandle(char *param, uint32_t len)
{
    uint8_t buf[221] = {0};
    uint8_t rxlen = 0;
    char ack[10] = {0};
    int ret = lmicapi_receive(buf, &rxlen);

    if (ret < 0) {
        AT_PrintAck("\r\nER03\r\n");
        return;
    }
    if (ret == 0) {
        AT_PrintAck("\r\nER05\r\n");
        return;
    }
    
    ret = snprintf(ack, sizeof(ack)-1, "\r\n%02X+", rxlen);
    AT_PrintAck(ack);
    for (uint8_t i=0; i<rxlen; i++) {
        memset(ack, 0, sizeof(ack));
        snprintf(ack, sizeof(ack)-1, "%02X", buf[i]);
        AT_PrintAck(ack);
    }
    AT_PrintAck("\r\n");
}

static void AT_BaseFreqHandle(char *param, uint32_t len)
{
    uint8_t tmp[4];
    uint32_t tx_freq, rx_freq;

    if (len != 17 || param[8] != '+'
        || str2hex(tmp, param, 8) < 0) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }
    
    tx_freq = tmp[0] << 24;
    tx_freq += tmp[1] << 16;
    tx_freq += tmp[2] << 8;
    tx_freq += tmp[3];
    
    if (str2hex(tmp, &param[9], 8) < 0) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }
    
    rx_freq = tmp[0] << 24;
    rx_freq += tmp[1] << 16;
    rx_freq += tmp[2] << 8;
    rx_freq += tmp[3];

    if (lmicapi_setbasefreq(tx_freq, rx_freq) < 0) {
        AT_PrintAck("\r\nER08\r\n");
        return;
    }

    AT_PrintAck("\r\nOK\r\n"); 
}

static void AT_NetHandle(char *param, uint32_t len)
{
    uint8_t stop = 0;
    if (len != 2) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }

    if (str2hex(&stop, param, 2) < 0 || stop > 2) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }
    
    switch(stop) {
    case 0:
        lmicapi_start();
        break;
    case 1:    
        lmicapi_stop();
        break;
    case 2:
        lmicapi_reset();
        break;
    default:break;
    }
    
    AT_PrintAck("\r\nOK\r\n");
}

static void AT_TxHandle(char *param, uint32_t len)
{
    int ret = 0;
    
    if (len < 8) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }
    
    uint8_t port, txlen, mode;
    if (str2hex(&port, param, 2) < 0 || port==0) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }
    
    if (str2hex(&mode, &param[3], 2) < 0 || mode>1) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }
    
    if (str2hex(&txlen, &param[6], 2) < 0) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }
    
    if (txlen == 0 && len != 8) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }
    
    if (txlen > 0) {
        if (str2hex((uint8_t *)param, &param[9], txlen<<1) < 0) {
            AT_PrintAck("\r\nER01\r\n");
            return;
        }
    }
    ret = lmicapi_send(port, (uint8_t *)param, txlen, mode);
    if (ret == -1) {
        AT_PrintAck("\r\nER03\r\n");
        return;
    }
    if (ret == -2) {
        AT_PrintAck("\r\nER01\r\n"); /* transfer data is too long */
        return;
    }
    if (ret == -3) {
        AT_PrintAck("\r\nER04\r\n"); /* network is busy */
        return;
    }
    AT_PrintAck("\r\nOK\r\n");
}

static void AT_TsHandle(char *param, uint32_t len)
{
    int ret = 0;
    if (len != 0) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }
    ret = lmicapi_transstatus();
    if (ret == -1) {
        AT_PrintAck("\r\nER03\r\n");
        return;
    }
    if (ret == -2) {
        AT_PrintAck("\r\nER04\r\n");
        return;
    }
    if (ret == -3) {
        AT_PrintAck("\r\nER07\r\n");
        return;
    }
    AT_PrintAck("\r\nOK\r\n");
}

static void AT_AppKeySHandle(char *param, uint32_t len)
{
    int ret = 0;
    if (len != 32) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }

    if (str2hex((uint8_t *)param, param, 32) < 0) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }

    ret = lmicapi_setappkey((uint8_t *)param);
    if (ret < 0) {
        AT_PrintAck("\r\nER08\r\n");
        return;
    }
    AT_PrintAck("\r\nOK\r\n");
}

static void AT_AppKeyQHandle(char *param, uint32_t len)
{
    uint8_t buf[16];
    char ack[5] = {0};

    lmicapi_getappkey(buf);

    AT_PrintAck("\r\n");
    for (uint8_t i=0; i<16; i++) {
        memset(ack, 0, sizeof(ack));
        snprintf(ack, sizeof(ack)-1, "%02X", buf[i]);
        AT_PrintAck(ack);
    }
    AT_PrintAck("\r\n");
}

static void AT_AppEuiSHandle(char *param, uint32_t len)
{
    int ret = 0;
    if (len != 16) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }

    if (str2hex((uint8_t *)param, param, 16) < 0) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }

    ret = lmicapi_setappeui((uint8_t *)param);
    if (ret < 0) {
        AT_PrintAck("\r\nER08\r\n");
        return;
    }
    AT_PrintAck("\r\nOK\r\n");
}

static void AT_AppEuiQHandle(char *param, uint32_t len)
{
    uint8_t buf[8];
    char ack[5] = {0};

    lmicapi_getappeui(buf);

    AT_PrintAck("\r\n");
    for (uint8_t i=0; i<8; i++) {
        memset(ack, 0, sizeof(ack));
        snprintf(ack, sizeof(ack)-1, "%02X", buf[i]);
        AT_PrintAck(ack);
    }
    AT_PrintAck("\r\n");
}

static void AT_BandSHandle(char *param, uint32_t len)
{
    uint8_t band;

    if (str2hex(&band, param, 2) < 0 || len != 2 
      || band > 7) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }
    if (lmicapi_setband(band) < 0) {
        AT_PrintAck("\r\nER08\r\n");
        return;
    }
    AT_PrintAck("\r\nOK\r\n");
}

static void AT_SfSHandle(char *param, uint32_t len)
{
    uint8_t sf;

    if (str2hex(&sf, param, 2) < 0 || len != 2 
        || sf < 7 || sf > 12) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }
    if (lmicapi_setsf(sf) < 0) {
        AT_PrintAck("\r\nER08\r\n");
        return;
    }
    AT_PrintAck("\r\nOK\r\n");
}

static void AT_Rx2ParamSHandle(char *param, uint32_t len)
{
    uint32_t freq;
    uint8_t dr;

    if (len != 11 || str2hex((uint8_t *)param, param, 8) < 0
        || str2hex(&dr, &param[9], 2) < 0
        || dr > 5) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }
    
    freq = param[0] << 24;
    freq += param[1] << 16;
    freq += param[2] << 8;
    freq += param[3];
    
    if (lmicapi_setrx2param(freq, dr) < 0) {
        AT_PrintAck("\r\nER08\r\n");
        return;
    }
    
    AT_PrintAck("\r\nOK\r\n");
}

static void AT_Rx2ParamQHandle(char *param, uint32_t len)
{
    uint32_t freq;
    uint8_t dr;
    char ack[10];

    if (len != 0) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }
    
    lmicapi_getrx2param(&freq, &dr);
    
    AT_PrintAck("\r\n");
    memset(ack, 0, sizeof(ack));
    snprintf(ack, sizeof(ack)-1, "%08X", freq);
    AT_PrintAck(ack);
    memset(ack, 0, sizeof(ack));
    snprintf(ack, sizeof(ack)-1, "+%02X", dr);
    AT_PrintAck(ack);
    AT_PrintAck("\r\n");
}

#if 1
static uint32_t rtctime=0;
#endif

static void AT_ElpHandle(char *param, uint32_t len)
{
    uint16_t time=0;
    uint8_t  mode=0;
    uint8_t  tmp[2]={0};
    
    if (len!=7 || str2hex(&mode, param, 2)!=0 || str2hex(tmp, param+3, 4)!=0) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }
    
    time=tmp[0]<<8;
    time+=tmp[1];
    AT_PrintAck("\r\nOK\r\n");
#if 1
    uint32_t start=osal_getTime();
    RTC_Start(time);
    extern uint8_t rtcstart;
    while(rtcstart!=0);
    rtctime=osal_getTime()-start;
    char ack[20]={0};
    snprintf(ack, sizeof(ack)-1, "%d %d", rtctime, time);
    AT_PrintAck(ack);
#else
    POW_EnterLowPow(mode, time);
#endif
}

static void AT_IOHandle(char *param, uint32_t len)
{
    if (len!=20 || str2hex((uint8_t *)param, param, 20) != 0) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }
    
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4
         |GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_11|GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2
        |GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12
        |GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    uint16_t tmp=(param[0]<<8)+param[1];
    GPIO_SetBits(GPIOA, tmp);
    GPIO_ResetBits(GPIOA, ~tmp);
    tmp=(param[2]<<8)+param[3];
    GPIO_SetBits(GPIOB, tmp);
    GPIO_ResetBits(GPIOB, ~tmp);
    tmp=(param[4]<<8)+param[5];
    GPIO_SetBits(GPIOC, tmp);
    GPIO_ResetBits(GPIOC, ~tmp);
    tmp=(param[6]<<8)+param[7];
    GPIO_SetBits(GPIOD, tmp);
    GPIO_ResetBits(GPIOD, ~tmp);
    tmp=(param[8]<<8)+param[9];
    GPIO_SetBits(GPIOF, tmp);
    GPIO_ResetBits(GPIOF, ~tmp);
    
    AT_PrintAck("\r\nOK\r\n");
}

//static uint8_t test_buf[50];
static radio_txrx_cfg_t cfg;

static void AT_CommTestHandle(char *param, uint32_t len)
{
    if ((len&1)!=0  || len>(sizeof(LMIC.frame)<<1)
        || str2hex(LMIC.frame, param, len) != 0) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }

    memset(&cfg, 0, sizeof(cfg));
    cfg.mode = RADIO_TXRX_CFG_MODE_LORA;
    cfg.pow = 20;
    cfg.freq = 470300000;
    cfg.sf = 12;
    cfg.low_speed = 1;
    cfg.bw = 0;
    cfg.crc = 1;
    cfg.invert = 0;
    cfg.premble = 8;
    cfg.syncword = 0x34;

    RADIO_StartSend(cfg, LMIC.frame, len>>1);
    /* wait send complete */
    uint32_t start=osal_getTime();
    while(!SX127x_IOCtl_IRQPend() && osal_getTime()-start<3000);
    if (osal_getTime()-start>=3000) { /* send timeout */
        AT_PrintAck("\r\nER07\r\n");
        return;
    }
    
    start=osal_getTime();
    /* sleep 1s */
    while(osal_getTime()-start<1000);
    /* enter receive state */
    cfg.rxmode = RADIO_TXRX_CFG_RXMODE_SINGLE;
    cfg.symtimeout = 8;
    LMIC.dataLen=0;
    RADIO_StartReceive(cfg, LMIC.frame, sizeof(LMIC.frame));
    while(!SX127x_IOCtl_IRQPend() && !Drv_SX127x_Radio_SymTimeout(SX127x_CHIP1) 
          && osal_getTime()-start<3000);
    Drv_SX127x_Radio_DioIntHandle(SX127x_CHIP1, &LMIC.dataLen);
    if (osal_getTime()-start>=3000) { /* receive timeout */
        AT_PrintAck("\r\nER07\r\n");
        return;
    }
    
    if (LMIC.dataLen==0) { /* receive failed */
        AT_PrintAck("\r\nER05\r\n");
        return;
    }
    
    AT_PrintAck("\r\n");
    for (int i=0; i<LMIC.dataLen; i++) {
        char str[5]={0};
        snprintf(str, 4, "%02X", LMIC.frame[i]);
        AT_PrintAck(str);
    }
    AT_PrintAck("\r\n");
}

static void AT_TxTestHandle(char *param, uint32_t len)
{
    uint8_t mode=0;
    
    if (len!=2 || str2hex(&mode, param, 2) != 0 || mode > 1) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }
    
    if (mode==0) {
        lmic_txstart();
    }
    else {
        lmic_txstop();
    }
    
    AT_PrintAck("\r\nOK\r\n");
}

static void AT_RxTestHandle(char *param, uint32_t len)
{
    uint8_t mode=0;
    uint8_t tmp[4]=0;
    uint32_t freq=0;
    uint8_t sf=0;

    if (len!=2&&len!=14 || str2hex(&mode, param, 2) != 0 || mode > 1) {
        AT_PrintAck("\r\nER01\r\n");
        return;
    }

#if 0
    if (mode==0) {
        lmic_rxstart();
    }
    else {
        lmic_rxstop();
    }
    AT_PrintAck("\r\OK\r\n");
#else
    if (mode==0) {
#if 0
        if (len != 14 || param[2] != '+' || param[11] != '+' 
        || str2hex(tmp, param+3, 8) != 0
            || str2hex(&sf, param+12, 2) != 0 || sf>12) {
            AT_PrintAck("\r\nER01\r\n");
            return;
        }
#endif
        freq=tmp[0]<<24;
        freq+=tmp[1]<<16;
        freq+=tmp[2]<<8;
        freq+=tmp[3]<<0;
        
        memset(&cfg, 0, sizeof(cfg));
        cfg.mode = RADIO_TXRX_CFG_MODE_LORA;
        cfg.freq = 470300000;
        cfg.sf = 12;
        cfg.low_speed = 0;
        cfg.bw = 0;
        cfg.crc = 0;
        cfg.invert = 0;
        cfg.premble = 8;
        cfg.syncword = 0x12;

        /* enter receive state */
        cfg.rxmode = RADIO_TXRX_CFG_RXMODE_SCAN;
        //cfg.symtimeout = 8;
        LMIC.dataLen=0;
        uint32_t start=osal_getTime();
        
        RADIO_StartReceive(cfg, LMIC.frame, sizeof(LMIC.frame));
        while(!SX127x_IOCtl_IRQPend()
              && osal_getTime()-start<5000);
        Drv_SX127x_Radio_DioIntHandle(SX127x_CHIP1, &LMIC.dataLen);
        if (LMIC.dataLen==0) { /* receive failed */
            AT_PrintAck("\r\nER05\r\n");
            return;
        }
        
        char str[10]={0};
#if 1
        for (int i=0; i<10; i++) {
            memset(str, 0, sizeof(str));
            snprintf(str, 3, "%02X", LMIC.frame[i]);
            AT_PrintAck(str);
        }
#endif
        RADIO_ReadPacketRssi(&LMIC.rssi, &LMIC.snr);
        
        AT_PrintAck("\r\n");
        
        if (LMIC.rssi>=0) {
            LMIC.rssi=-1;
        }
        
        memset(str, 0, sizeof(str));
        snprintf(str, 5, "%02X+", -LMIC.rssi);
        snprintf(str+3, 4, "%2X", LMIC.snr);
        AT_PrintAck(str);
        AT_PrintAck("\r\n");
    }
#endif
}

static uint32_t AT_ParamLenGet(char *cmd, uint32_t len)
{
    uint32_t i = 0;
    while(i<len) {
        if (strncmp(&cmd[i], "\r\n", 2) == 0) {
            return i;
        }
        i++;
    }
    return 0;
}

void AT_CmdsDecode(char *cmd, uint32_t len)
{
    uint32_t i = 0;
    uint8_t ret = 0;
    
    while(i+4<len) {
        uint8_t j = 0;
        if (strncmp(&cmd[i], "AT", 2)) {
            i++;
            continue;
        }
        for (j = 0; j<sizeof(at_short_cmds)/sizeof(at_cmd_t); j++) {
            uint8_t cmd_name_len = strlen(at_short_cmds[j].name);
            if (!strncmp(&cmd[i], at_short_cmds[j].name, cmd_name_len)) {
                i += cmd_name_len;
                if (AT_ParamLenGet(&cmd[i], len-i) == 0) {
                    ret = 1;
                    at_short_cmds[j].func(NULL, 0);
                    i += 2;
                }
            }
        }
        for (j = 0; j<sizeof(at_que_cmds)/sizeof(at_cmd_t); j++) {
            uint8_t cmd_name_len = strlen(at_que_cmds[j].name);
            if (!strncmp(&cmd[i], at_que_cmds[j].name, cmd_name_len)) {
                i += cmd_name_len;
                if (AT_ParamLenGet(&cmd[i], len-i) == 0) {
                    ret = 1;
                    at_que_cmds[j].func(NULL, 0);
                    i += 2;
                }
            }
        }
        for (j = 0; j<sizeof(at_equ_cmds)/sizeof(at_cmd_t); j++) {
            uint8_t cmd_name_len = strlen(at_equ_cmds[j].name);
            if (!strncmp(&cmd[i], at_equ_cmds[j].name, cmd_name_len)) {
                i += cmd_name_len;
                uint32_t param_len = AT_ParamLenGet(&cmd[i], len-i);
                if (param_len > 0) {
                    ret = 1;
                    at_equ_cmds[j].func(&cmd[i], param_len);
                    i += param_len;
                }
            }
        }
    }
    
    if (ret == 0) {
        AT_PrintAck("\r\nER00\r\n");
    }
}