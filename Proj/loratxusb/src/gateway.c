#include "app.h"
#include "gateway.h"
#include "ringbuf.h"
#include "radio.h"
#include "os.h"
#include "verify.h"

gw_cfg_t  gw_cfg = {
    .swd = 0x34,
};

static ringbuf_t rb_tx  = {0};
static uint8_t   buftx[1024] = {0};
static uint8_t   txdata[255+sizeof(gw_pkt_rx_t)] = {0};
static uint8_t   gw_tx_pkts = 0;
static uint8_t   task_done = 1;

static void initRingbuf(void)
{
    RINGBUF_Init(&rb_tx, buftx, sizeof(buftx));
}

void GW_Lock(void)
{
    os_wait(OS_SEM_GW_LOCK, 0);
}

void GW_UnLock(void)
{
    os_post(OS_SEM_GW_LOCK);
}

int GW_PutTx(gw_pkt_tx_t *p)
{
    GW_Lock();
    
    if (RINGBUF_Put(&rb_tx, (uint8_t*)p, SIZEOFPKTTX(p)) < 0) {
        GW_UnLock();
        
        return -1;
    }

    gw_tx_pkts++;
    
    GW_UnLock();
 
    return 0;
}

int GW_PollTx(gw_pkt_tx_t *p)
{
    GW_Lock();
    
    if (RINGBUF_Poll(&rb_tx, (uint8_t*)p, sizeof(gw_pkt_tx_t))<0) {
        GW_UnLock();
        
        return -1;
    }
    
    if (RINGBUF_Poll(&rb_tx, (uint8_t*)p+sizeof(gw_pkt_tx_t), p->size)<0) {
        GW_UnLock();
        
        return -1;
    }

    gw_tx_pkts--;
    
    GW_UnLock();

    return 0;
}

void GW_DropTx(void)
{
    GW_Lock();
    
    gw_pkt_tx_t *p = Malloc(sizeof(gw_pkt_tx_t));
    
    if (RINGBUF_Poll(&rb_tx, (uint8_t*)p, sizeof(gw_pkt_tx_t))<0) {
        MFree(p);
        GW_UnLock();

        return;
    }
    
    uint8_t size  = p->size;
    
    MFree(p);

    uint8_t *data = Malloc(size);
    
    if (RINGBUF_Poll(&rb_tx, data, size)<0) {
        MFree(data);
        GW_UnLock();
        
        return;
    }

    MFree(data);
    gw_tx_pkts--;
    
    GW_UnLock();

    return;
}

int GW_SendPkt(gw_pkt_tx_t *txpkt)
{
    static bool txstate = false;

    os_lock(OS_SEM_GWTX_LOCK, 0);
    if (txstate) {
        os_unlock(OS_SEM_GWTX_LOCK);

        return -1;
    }
    txstate = true;
    os_unlock(OS_SEM_GWTX_LOCK);
    
    radio_cfg_t txrxcfg = {0};
    
    txrxcfg.bw       = txpkt->bw;
    txrxcfg.sf       = txpkt->sf;
    txrxcfg.ncrc     = txpkt->ncrc;
    txrxcfg.powe     = txpkt->powe;
    txrxcfg.lora     = txpkt->lora;
    txrxcfg.freq     = txpkt->freq;
    txrxcfg.ipol     = txpkt->ipol;
    txrxcfg.prea     = txpkt->prea;
    txrxcfg.codr     = txpkt->codr;
    txrxcfg.data     = txpkt->data;
    txrxcfg.size     = txpkt->size;
    
    RADIO_StartTx(&txrxcfg);

    os_lock(OS_SEM_GWTX_LOCK, 0);
    txstate = false;
    os_unlock(OS_SEM_GWTX_LOCK);
    
    return 0;
}

static void GW_SendPkt1(void)
{
    radio_cfg_t txrxcfg = {0};
    uint8_t data[8] = {0};
    
    txrxcfg.bw       = 0;
    txrxcfg.sf       = 7;
    txrxcfg.ncrc     = 1;
    txrxcfg.powe     = 10;
    txrxcfg.lora     = 1;
    txrxcfg.freq     = 470500000+5000;
    txrxcfg.ipol     = 0;
    txrxcfg.prea     = 8;
    txrxcfg.codr     = 0;
    txrxcfg.data     = data;
    txrxcfg.size     = 8;
    
    RADIO_StartTx(&txrxcfg);
}

static void GW_SendPkt2(void)
{
    radio_cfg_t txrxcfg = {0};
    uint8_t data[8] = {0};
    
    txrxcfg.bw       = 0;
    txrxcfg.sf       = 12;
    txrxcfg.ncrc     = 1;
    txrxcfg.powe     = 20;
    txrxcfg.lora     = 0;
    txrxcfg.freq     = 470100000;
    txrxcfg.ipol     = 1;
    txrxcfg.prea     = 8;
    txrxcfg.codr     = 0;
    txrxcfg.data     = data;
    txrxcfg.size     = 8;

    RADIO_StartTx(&txrxcfg);
}

//static uint8_t  gw_task_id = 0;
//static uint32_t rx_time    = 0;
uint32_t tx_time    = 0;
//static uint32_t tx_len     = 0;
static gw_pkt_tx_t *txpkt = NULL;

int PollTxPkt(gw_pkt_tx_t* pkt)
{
    uint32_t now = os_getticks();

    for (int i=0; i<gw_tx_pkts; i++) {
        GW_PollTx(pkt);

        int diff = now-(txpkt->tmst/1000);

        if (diff > 1 && pkt->imme==0) { /* timeout */
            continue;
        }

        if (pkt->imme == 0 && diff < -1) {
            GW_PutTx(pkt);
            
            continue;
        }

        return 0;
    }

    return -1;
}

void GW_Task(void)
{
    //GW_SendPkt1();
    //osal_delay(gw_task_id, 4000);
#if 0
    GW_SendPkt2();
    
    os_delay(100);
    
    return;
#else
    if (task_done) {
        //osal_delay(task.id, 1);

        return;
    }
    
    if (VERIFY_Check() < 0) {

        return;
    }

    uint32_t now = os_getticks();
    
    txpkt = (gw_pkt_tx_t *)txdata;

    if (PollTxPkt(txpkt) != 0)
        return;

#if 1
    txpkt->freq+=6000;
    GW_SendPkt(txpkt);
    
    if (txpkt->size>10) {
        //uint8_t ack[] = {0xff, 5, 0};
        //Drv_Usart_Write(ack, sizeof(ack), PORT_USART1);
    }
#else
    GW_SendPkt1();
#endif

    tx_time = now;
#endif
}

void GW_Init(void)
{
    initRingbuf();

    os_initevent(OS_SEM_GW_LOCK, 1);
    os_initevent(OS_SEM_GWTX_LOCK, 1);
}

void GW_Start(void)
{
    task_done = 0;
}

void GW_Stop(void)
{
    task_done = 1;
}