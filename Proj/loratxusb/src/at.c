#include "at.h"
#include "gateway.h"
#include "os.h"
#include "app.h"
#include "verify.h"
#include "common.h"
#include "crc.h"
#include "ringbuf.h"
#include <string.h>
#include <stdarg.h>

typedef void(*at_func_t)(char *param, uint32_t len);

#pragma pack(1)
typedef struct {
    uint8_t  cmd;
    char     pack[];
} at_cmd_t;
#pragma pack()

/* at command handle function */
static void AT_ZHandle(char *param, uint32_t len);
static void AT_IdHandle(char *param, uint32_t len);
static void AT_CHandle(char *param, uint32_t len);
static void AT_VHandle(char *param, uint32_t len);
static void AT_SyncHandle(char *param, uint32_t len);
static void AT_TxHandle(char *param, uint32_t len);
static void AT_TimeHandle(char *param, uint32_t len);

/* at command handle list */
static const at_func_t at_cmds[] = {
    [0] = AT_ZHandle,
    [1] = AT_IdHandle,
    [2] = AT_TxHandle,
    [4] = AT_CHandle,
    [5] = NULL,
    [6] = NULL,
    [7] = NULL,
    [8] = AT_TimeHandle,
    [9] = AT_SyncHandle,
    [10] = AT_VHandle,
};

static uint8_t at_buf[512] = {0};
static ringbuf_t at_rbuf = {0};

/* at command ack print */
static void AT_PrintStr(char *ack)
{
    //AT_PrintAck((uint8_t *)ack, strlen(ack));
}

static void AT_PrintAck(uint8_t *ack, uint16_t len)
{
    for (int i = 0; i < len; i++)
    {
        USB_Send_Data(ack[i]);
    }
}

static void AT_ZHandle(char *param, uint32_t len)
{
    uint8_t  ack[3]  = {0xff, 0, 0};

    AT_PrintAck(ack, sizeof(ack));
}

static void AT_IdHandle(char *param, uint32_t len)
{
    uint8_t  ack[14]  = {0xff, 1, 0};

    VERIFY_ReadId(&ack[2], 12);

    AT_PrintAck(ack, sizeof(ack));
}

//鉴权
static void AT_CHandle(char *param, uint32_t len)
{
    uint8_t  ack[3]  = {0xff, 4, 0};

    GW_Start();

    AT_PrintAck(ack, sizeof(ack));
}

//鉴权
static void AT_VHandle(char *param, uint32_t len)
{
    uint8_t  ack[3]  = {0xff, 10, 0};

    VERIFY_SetPasswd(param, len);

    AT_PrintAck(ack, sizeof(ack));
}

//发送FSK进行同步
static void AT_SyncHandle(char *param, uint32_t len)
{
    uint8_t  ack[3]  = {0xff, 9, 0};
    
    if (len < 4) {
        ack[2] = 1;
        
        AT_PrintAck(ack, sizeof(ack));
        
        return;
    }

    gw_pkt_tx_t *pkt = Malloc(sizeof(gw_pkt_tx_t)+10);

    pkt->lora = 0;
    pkt->powe = 5;
    pkt->freq = ntohl(rbuf4((uint8_t *)param));
    pkt->size = 8;       
    memset(pkt->data, 0, 8);
    pkt->data[0] = 0xff;
    
    uint32_t now = os_getticks();

    memcpy(&pkt->data[2], &now, 4);

    /* buffer is full */
    if (GW_SendPkt(pkt) != 0) {
        ack[2] = 1;
    }

    MFree(pkt);

    AT_PrintAck(ack, sizeof(ack));
}

static void AT_TimeHandle(char *param, uint32_t len)
{
    uint8_t  ack[6]  = {0xff, 8};

    uint32_t time = os_getticks();
    memcpy(&ack[2], &time, 4);

    AT_PrintAck(ack, sizeof(ack));
}

static void AT_TxHandle(char *param, uint32_t len)
{
    int          i   = 0;
    gw_pkt_tx_t *p   = (gw_pkt_tx_t *)param;
    uint8_t  ack[3]  = {0xff, 2, 0};

    if (VERIFY_Check() < 0) {
        AT_PrintAck(ack, sizeof(ack));
        
        return;
    }

    while(i+sizeof(gw_pkt_tx_t) < len) {
        if (i+SIZEOFPKTTX(p) > len) {
            ack[2] = 1;
            
            break;
        }

        if (GW_PutTx(p)!=0) {
            ack[2] = 1;

            break;
        }

        i+=SIZEOFPKTTX(p);
        p=(gw_pkt_tx_t *)(&param[i]);
    }

    AT_PrintAck(ack, sizeof(ack));
}

void AT_Init(void)
{
    RINGBUF_Init(&at_rbuf, at_buf, sizeof(at_buf));
    
    os_initevent(OS_SEM_AT_RX, 0);
}

void AT_Run(void)
{
    while (true) 
    {
        os_wait(OS_SEM_AT_RX, 0);
        
        while(os_wait(OS_SEM_AT_RX, 3) == 0);
 
        uint8_t tmp = 0;
 
        while (RINGBUF_Poll(&at_rbuf, &tmp, 1) == 0 && tmp != 0xff);
        
        if (tmp != 0xff)
            continue;

        uint16_t len = RINGBUF_GetLen(&at_rbuf);
        uint8_t *cmd = Malloc(len);

        if (RINGBUF_Poll(&at_rbuf, cmd, len) != 0) {
            MFree(cmd);
            
            continue;
        }

        //AT_CmdsDecode(cmd, len);
        at_cmd_t *c = (at_cmd_t *)cmd;
        
        if (c->cmd < sizeof(at_cmds)/sizeof(at_cmds[0]) && at_cmds[c->cmd] != NULL) {
            at_cmds[c->cmd](c->pack, len-1);
        }
        
        MFree(cmd);
    }
}

/*******************************************************************************
* Function Name  : USB_To_USART_Send_Data.
* Description    : send the received data from USB to the UART 0.
* Input          : data_buffer: data address.
                   Nb_bytes: number of bytes to send.
* Return         : none.
*******************************************************************************/
void USB_To_AT_Data(uint8_t* data_buffer, uint8_t Nb_bytes)
{
    RINGBUF_Put(&at_rbuf, data_buffer, Nb_bytes);
    
    os_post(OS_SEM_AT_RX);
}
