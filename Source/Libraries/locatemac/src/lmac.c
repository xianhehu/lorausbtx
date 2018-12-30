#include "lmac.h"
#include "lowlevel.h"
#include <string.h>

enum {
    SYM_TIME_SF7  =  1,    /* 1ms */
    SYM_TIME_SF8  =  2,
    SYM_TIME_SF9  =  4,
    SYM_TIME_SF10 =  8,
    SYM_TIME_SF11 = 16,
    SYM_TIME_SF12 = 32,
};

typedef void(*state_cb_func_t)(void);

static lmac_t              lmac={0};
static lmac_statistic_t    lmac_statistic={0};
static const uint32_t lmac_channel_freq_up[]=LMAC_CHANNEL_FREQS_UP;
static const uint32_t lmac_channel_freq_dn[]=LMAC_CHANNEL_FREQS_DN;
static const uint32_t lmac_channel_freq_adv[]=LMAC_ADV_CHANNEL_FREQS;

static const uint8_t sf_symtim[]={SYM_TIME_SF7, SYM_TIME_SF8, SYM_TIME_SF9, 
                                  SYM_TIME_SF10, SYM_TIME_SF11, SYM_TIME_SF12};

static state_cb_func_t  state_cb_func_list[LMAC_STATE_DATARECV+1]={NULL};

/* lmac stack task */
static lmac_task_time_t lmac_task_time={0};
static lmac_task_imme_t lmac_task_imme={0};

static uint8_t lmac_adv_times=0;

/* lmac timer */
static lmac_timer_t lmac_timer;

/* lmac stack event from outside */
static bool    has_event=false;
static uint8_t event_id=false;


static void TASK_SendSync(void *arg);
static void TASK_SendComplete(void *arg);
static void TASK_ProcessAdvSanResult(void *arg);
static void TASK_AdvSync(void *arg);
static void TASK_ProcessAdvSyncResult(void *arg);
static void TASK_AckListen(void *arg);
static void TASK_AckProcess(void *arg);


static uint32_t rand32(void)
{
    return LMAC_LL_Rand();
}

static uint16_t rand16(void)
{
    static uint32_t rand;
    static uint32_t i=0;
    
    i++;
    
    if (i&1) {
        rand=rand32();
        return rand;
    }
    
    return rand>>16;
}

static uint8_t rand8(void)
{
    static uint16_t rand;
    static uint32_t i=0;
    
    i++;
    if (i&1) {
        rand=rand16();
        return rand;
    }
    
    return rand>>8;
}

/* lmac stack task control functions */
/**
  * @brief  clear lmac timing task.
  * @param  None
  * @retval None
  * @note   this function should be called after timing task was run
  */
static void clearTimeTask(void)
{
    lmac_task_time.func=NULL;
    lmac_task_time.func=0;
}

static void clearImmeTask(void)
{
    lmac_task_imme.func=0;
}

/**
  * @brief  set timing task.
  * @param  func: timing task function
  * @param  time: time when func will be called
  * @retval None
  * @note   func function will be called when time be reached
  */
static void setTimeTask(lmac_task_func_t func, uint32_t time)
{
    lmac_task_time.func=func;
    lmac_task_time.time=time;
}

static void setImmeTask(lmac_task_func_t func)
{
    lmac_task_imme.func=func;
    lmac_task_imme.enable=false;
}

static void enableImmeTask(void)
{
    lmac_task_imme.enable=true;
}

static void switchState(uint8_t state)
{
    if (state!=lmac.state) {
        LMAC_LL_Lock();
        lmac.state=state;
        LMAC_LL_Unlock();
        if (state_cb_func_list[state]!=NULL)
            state_cb_func_list[state]();
    }
}

/* lmac stack core functions */
static void buildMacFrame(uplink_data_t *data)
{
    uint8_t i=0;

    lmac.send_buf[i]=0x40;
    if (data->confirm)
        lmac.send_buf[i]|=0x01;
    i++;

    lmac.send_buf[i++]=lmac.devid>>24;
    lmac.send_buf[i++]=lmac.devid>>16;
    lmac.send_buf[i++]=lmac.devid>>8;
    lmac.send_buf[i++]=lmac.devid;

    lmac.send_buf[i++]=lmac.send_slot>>8;
    lmac.send_buf[i++]=lmac.send_slot;

    lmac.send_buf[i++]=data->fcnt;

    memcpy(lmac.send_buf+i, data->buf, data->len);
    i+=data->len;

    lmac.send_len=i;
}

static void rebuildMacFrame(void)
{
    lmac.send_buf[5]=lmac.send_slot>>8;
    lmac.send_buf[6]=lmac.send_slot;
}

static int32_t decodeMacFrame(void)
{
    return -1;
}

static bool isChannelFree(uint32_t freq)
{
    /* start listening */
    lmac_ll_cfg_rx_t cfg={0};
    
    cfg.scanRssi=1;
    cfg.buf=lmac.recv_buf;
    cfg.size=sizeof(lmac.recv_buf);
    cfg.bw=0;
    cfg.freq=freq;
    cfg.sf=7;
    
    LMAC_LL_RadioListen(&cfg);
    
    /* read rssi and snr */
    int16_t rssi;
    int8_t snr;
    LMAC_LL_RadioReadPacketRssi(&rssi, &snr);
    
    if (rssi>LMAC_SEND_CSMA_RSSI) {
        return false;
    }
    
    return true;
}

static void LMAC_TimerCB(void*arg1, void *arg2)
{
    LMAC_LL_RadioStop();
    lmac.recv_len=0;

    switch(lmac.state) {
    case LMAC_STATE_SCANING:
    case LMAC_STATE_SYNCING:
    case LMAC_STATE_DATARECV:
        
        break;
    case LMAC_STATE_DATASEND:
        if (lmac.send_confirm==0) {
            //lmac.state=LMAC_STATE_SYNCED;
            switchState(LMAC_STATE_SYNCED);
            lmac.send_hassyncdata=0;
        }
        break;
    }

    if (lmac_task_imme.func!=NULL) {
        enableImmeTask();
    }
}

static uint32_t getAdvTime(uint32_t now)
{
    int i=0;

    while(true) {
        uint32_t time=lmac.adv_txtime+lmac.adv_period*lmac.slot_time*i;  // ÓÐ¿ÉÄÜnow<lmac.adv_txtime
        uint32_t error=(now-lmac.adv_txtime)/lmac.time_err_rate;

        if ((int32_t)(time-now-error) > 0) {
            return time-error;
        }

        i++;
    }
}

static uint32_t calcAirTime(uint8_t plen, uint8_t sf)
{
    uint8_t bw = 0;  // 0,1,2 = 125,250,500kHz
    
    sf = sf-6;  // 0=FSK, 1..6 = SF7..12
    
    if( sf == 0 ) {
        return (plen+/*preamble*/5+/*syncword*/3+/*len*/1+/*crc*/2) * /*bits/byte*/8
            * (int)1000 / /*kbit/s*/50000;
    }

    uint8_t sfx = 4*(sf+(7-1));
    uint8_t q = sfx - (sf >= 5 ? 8 : 0);
    //int tmp = 8*plen - sfx + 28 + (getNocrc(rps)?0:16) - (getIh(rps)?20:0);
    int tmp = 8*plen - sfx + 28 + 16;

    if( tmp > 0 ) {
        tmp = (tmp + q - 1) / q;
        //tmp *= getCr(rps)+5;
        tmp *= 5;
        tmp += 8;
    } else {
        tmp = 8;
    }
    tmp = (tmp<<2) + /*preamble*/49 /* 4 * (8 + 4.25) */;
    // bw = 125000 = 15625 * 2^3
    //      250000 = 15625 * 2^4
    //      500000 = 15625 * 2^5
    // sf = 7..12
    //
    // osticks =  tmp * OSTICKS_PER_SEC * 1<<sf / bw
    //
    // 3 => counter reduced divisor 125000/8 => 15625
    // 2 => counter 2 shift on tmp
    sfx = sf+6 - (3+2) - bw;
    int div = 15625;
    
    if( sfx > 4 ) {
        // prevent 32bit signed int overflow in last step
        div >>= sfx-4;
        sfx = 4;
    }
    
    // Need 32bit arithmetic for this last step
    return (((uint32_t)tmp << sfx) * OSTICKS_PER_SEC + div/2) / div;
}

static bool canNowSend(uint32_t now, uint32_t txtime)
{
    uint32_t airtime=calcAirTime(lmac.send_len, lmac.channel_sf);
    uint32_t slottime=0;
    
    uint8_t shift=0;

    if (lmac.channel_sf>=11)
        shift=4;
    else
        shift=lmac.channel_sf>7?lmac.channel_sf-7:0;

    slottime=lmac.slot_time<<shift;

    if ((int32_t)(txtime-now)>0) {
        return false;
    }

    if ((int32_t)(now+airtime-(txtime+slottime))>=0) {
        return false;
    }

    return true;
}

static uint32_t getSendTime(uint32_t now)
{
    int i=0;

    while(true) {
        uint32_t time=lmac.adv_txtime+lmac.slot_time*
                      (lmac.channel_solts*i-lmac.adv_txslot+lmac.send_slot);
        uint32_t err=(time-lmac.adv_txtime)/lmac.time_err_rate;
        uint32_t txtime=time+err;

        if (canNowSend(now, txtime)) {
            return now;
        }

        if ((int32_t)(time+err-now+3)>=0)
            return time+err;

        i++;
    }
}

static int32_t syncSendData(void)
{
    /* send data */
    lmac_ll_cfg_tx_t cfg={0};
    cfg.buf=lmac.send_buf;
    cfg.len=lmac.send_len;
    cfg.sf=lmac.channel_sf;
    cfg.bw=0;
    cfg.freq=lmac_channel_freq_up[lmac.channel_num];
    
    if (isChannelFree(cfg.freq)) {
        LMAC_LL_RadioSend(&cfg);
        
        return 0;
    }
    else {
        lmac_statistic.send_csma+=1;
    }
    
    return -1;
}

static int32_t processDnData(uint8_t *buf, uint8_t len)
{
    uint8_t ack=0;
    uint8_t i=0;

    if (len<LMAC_FRAME_MIN_LEN_DATA) {
        lmac.recv_len=0;
        return -1;
    }

    if (buf[i]&0xC0 != 0x40) {
        lmac.recv_len=0;
        return -1;
    }

    if (buf[i++]&0x02) {
        ack=1;
    }

    uint32_t id=buf[i++]<<24;
    id+=buf[i++]<<16;
    id+=buf[i++]<<8;
    id+=buf[i++];

    if (id!=lmac.devid) {
        lmac.recv_len=0;
        return -1;
    }

    if (ack && lmac.send_confirm) {
        lmac.send_acked=1;
    }

    if (len>i) {
        lmac.recv_len=len-i;
        memcpy(lmac.recv_buf, buf+i, lmac.recv_len);
    }
    else {
        lmac.recv_len=0;
    }

    return 0;
}

static uint8_t getAvailSf(void)
{
    if (lmac.rssi<LMAC_CHANNEL_MINRSSI_SF7) {
        return 1; /* sf7 */
    }
    
    if (lmac.rssi<LMAC_CHANNEL_MINRSSI_SF8) {
        return 2; /* sf8 */
    }
    
    if (lmac.rssi<LMAC_CHANNEL_MINRSSI_SF9) {
        return 3; /* sf9 */
    }
    
    if (lmac.rssi<LMAC_CHANNEL_MINRSSI_SF10) {
        return 1; /* sf10 */
    }
    
    if (lmac.rssi<LMAC_CHANNEL_MINRSSI_SF11) {
        return 1; /* sf11 */
    }

    return 6; /* sf12 */
}

static int16_t getChannel(lmac_channel_t *list, uint8_t num)
{
    uint8_t avail[8]={0};
    uint8_t avails=0;
    uint8_t sf=getAvailSf();
    uint8_t maxsf=0;
    
    for (int i=0; i<num; i++) {
        if (maxsf<list[i].sf) {
            maxsf=list[i].sf;
        }
    }
    
    if (maxsf<sf) {
        sf=maxsf;
    }

    for (int i=0; i<num; i++) {
        if (list[i].rate<=LMAC_CHANNEL_USERATE_LOW && list[i].sf>=sf)
            avail[avails++]=i;
    }

    if (avails>0) {
        return avail[rand8()%avails];
    }

    uint8_t min=0xff;
    for (int i=0; i<num; i++) {
        if ((min==0xff||list[i].rate<list[min].rate) && list[i].sf>=sf)
            min=i;
    }

    if (min==0xff) {
        return -1;
    }

    return min;
}

static uint16_t getSendSlot(void)
{
    uint8_t shift=0;
    uint16_t slot;
    uint16_t slots=lmac.channel_solts;

    if (lmac.channel_sf>=11)
        shift=4;
    else
        shift=lmac.channel_sf>7?lmac.channel_sf-7:0;

    slots>>=shift;
    slot=rand16()%slots;
    slot<<=shift;

    return slot;
}

static int32_t processChannelAdv(uint8_t *buf, uint8_t len)
{
    static uint8_t advs_in_samebs=0; /* times of revieved adve */
    if (len<9) {
        return -1;
    }

    int i=0;
    uint16_t bsid;
    uint8_t  seq=0;
    uint16_t slots=0;
    uint16_t slot_time=0;
    uint8_t  adv_period=0;
#if 1
    /* check data */
    if ((buf[i++]&0xC0)!=0) {
        return -1;
    }

    /* save channal param */
    bsid=buf[i++]<<8;
    bsid+=buf[i++];
    seq=buf[i++];

    adv_period=buf[i++];
    slots=buf[i++]*10;
    slot_time=buf[i++]*100;
    uint8_t channel_num=(len-i)>>1;
    
    if (adv_period<1 || slots<10 || slot_time<100) {
        return -1;
    }
    
    lmac.adv_period=adv_period;
    
    lmac_channel_t *channel=(lmac_channel_t *)&buf[i];

    uint16_t adv_txslot=seq*lmac.adv_period;
    lmac.adv_txslot=adv_txslot%slots;

    if (lmac.state==LMAC_STATE_SCANING 
     || bsid!=lmac.adv_bsid
     || getAvailSf()!=lmac.channel_sf) {
        int16_t c=getChannel(channel, channel_num);
        if (c<0) {
            return -1;
        }
 
        lmac.channel_solts=slots;
        lmac.slot_time=slot_time;
        lmac.channel_num=channel[c].num;
        lmac.channel_sf=channel[c].sf+6;
        lmac.send_slot=getSendSlot();
        lmac.scan_peroid=lmac.adv_period*slot_time*10;

#ifdef LMAC_DEBUG
        LMAC_DEBUG("bsid:%d\r\n", lmac.adv_bsid);
        LMAC_DEBUG("channel num:%d, sf:%d, slot:%d, time:%d, period:%d\r\n", 
                   lmac.channel_num, lmac.channel_sf, lmac.send_slot, 
                   lmac.slot_time, lmac.adv_period);
#endif
    }
#endif

    return 0;
}

static void TASK_SendSync(void *arg)
{
    uint32_t now=LMAC_LL_GetTime();
    uint32_t txtime=getSendTime(now);
    int32_t adv_time_off =txtime - lmac.adv_txtime;

    switchState(LMAC_STATE_DATASEND);

    if (adv_time_off<LMAC_ADV_MAX_INTERVAL) {
        if (txtime<=now) {
            rebuildMacFrame();
            if (syncSendData()==0) {
#ifdef LMAC_DEBUG
                LMAC_DEBUG("lmac success to send\r\n");
#endif
                lmac_timer.time=3000; /* avoid to drop txdone signal */
                LMAC_LL_StartTimer(&lmac_timer);
                setImmeTask(TASK_SendComplete);
            }
            else {
#if 0
                /* get new slot for moving device */
                lmac.send_slot=getSendSlot();
#ifdef LMAC_DEBUG
                LMAC_DEBUG("csma change slot:%d\r\n", lmac.send_slot);
#endif
#endif
                //setTimeTask(TASK_SendSync, (rand16()%10000)+LMAC_LL_GetTime());
                switchState(LMAC_STATE_SYNCED);
            }
        }
        else {
#ifdef LMAC_DEBUG
            LMAC_DEBUG("send at %lu\r\n", txtime);
#endif
            setTimeTask(TASK_SendSync, txtime);
        }
    }
    else {
        //lmac.state=LMAC_STATE_SYNCING;
#ifdef LMAC_DEBUG
        LMAC_DEBUG("resync\r\n");
#endif
        switchState(LMAC_STATE_SYNCING);
        setImmeTask(TASK_AdvSync);
        enableImmeTask();
    }
}

static void TASK_SendComplete(void *arg)
{
    /* stop timer */
    LMAC_LL_StopTimer();

    if (lmac.send_confirm) {
        setImmeTask(TASK_AckListen);
        enableImmeTask();
    }
    else {
        switchState(LMAC_STATE_SYNCED);
        lmac.send_hassyncdata=0;
    }
}

static void TASK_AckListen(void *arg)
{
    lmac_ll_cfg_rx_t cfg={0};

    //lmac.state=LMAC_STATE_DATARECV;
    switchState(LMAC_STATE_DATARECV);
    cfg.freq=lmac_channel_freq_dn[lmac.channel_num];
    cfg.sf=lmac.channel_sf;
    cfg.buf=lmac.recv_buf;
    cfg.size=sizeof(lmac.recv_buf);
    uint8_t symtim=sf_symtim[lmac.channel_sf-7];
    cfg.sym_timout=(20+symtim-1)/symtim+5;                    /* 5symbols+20ms */
    LMAC_LL_RadioListen(&cfg);

    /* start timeout timer */
    lmac_timer.time=3000;
    LMAC_LL_StartTimer(&lmac_timer);

    setImmeTask(TASK_AckProcess);
}

static void TASK_AckProcess(void *arg)
{
    /* stop timer */
    LMAC_LL_StopTimer();
    
    if (lmac.recv_len>0) {
        /* read data */
        lmac.recv_len=LMAC_LL_RadioRecv(lmac.recv_buf);
        /* read rssi and snr */
        int16_t rssi;
        int8_t  snr;
        LMAC_LL_RadioReadPacketRssi(&rssi, &snr);
        lmac.rssi=-rssi;
        lmac.snr =snr;
    }

    if (lmac.recv_len>0 && processDnData(lmac.recv_buf, lmac.recv_len)==0) {
        if (lmac.send_confirm) {
            lmac.send_acked=1;
        }
    }

    //lmac.state=LMAC_STATE_SYNCED;
    switchState(LMAC_STATE_SYNCED);
    lmac.send_hassyncdata=0;
}

static void TASK_AdvScan(void *arg)
{
    //lmac.state=LMAC_STATE_SCANING;
    switchState(LMAC_STATE_SCANING);
    
    lmac_ll_cfg_rx_t cfg={0};
    cfg.freq=lmac_channel_freq_adv[lmac_adv_times];
    cfg.sf=12;
    cfg.buf=lmac.recv_buf;
    cfg.size=sizeof(lmac.recv_buf);
    LMAC_LL_RadioListen(&cfg);
    
    setImmeTask(TASK_ProcessAdvSanResult);
    /* start timeout timer */
    lmac_timer.time=lmac.scan_peroid;
    LMAC_LL_StartTimer(&lmac_timer);
}

static void TASK_ProcessAdvSanResult(void *arg)
{
    /* stop timer */
    LMAC_LL_StopTimer();

    if (lmac.recv_len!=0) {
        /* read data */
        lmac.recv_len=LMAC_LL_RadioRecv(lmac.recv_buf);
        /* read rssi and snr */
        int16_t rssi;
        int8_t  snr;
        LMAC_LL_RadioReadPacketRssi(&rssi, &snr);
        lmac.rssi=-rssi;
        lmac.snr=snr;
    }

    if (processChannelAdv(lmac.recv_buf, lmac.recv_len)<0) {
        lmac_adv_times++;
        lmac_statistic.scan_failed++;

        if (lmac_adv_times>=GET_ARRY_SIZE(lmac_channel_freq_adv)) {
            lmac_adv_times=0;
            //lmac.state=LMAC_STATE_NOSCAN;
            switchState(LMAC_STATE_NOSCAN);
        }
        else {
            if (lmac.scan_interval==0) {
                setImmeTask(TASK_AdvScan);
                enableImmeTask();
            }
            else {
                setTimeTask(TASK_AdvScan, lmac.scan_interval+LMAC_LL_GetTime());
            }
        }
    }
    else {
        lmac_statistic.scan_success++;
        lmac.adv_channel=lmac_adv_times;
        lmac_adv_times=0;
        lmac.sync_times=0;
        switchState(LMAC_STATE_SYNCED);
    }

#ifdef LMAC_DEBUG
    LMAC_DEBUG("adv scan, success:%d, failed:%d\r\n", 
               lmac_statistic.scan_success, lmac_statistic.scan_failed);
    LMAC_DEBUG("adv tx time:%d\r\n", lmac.adv_txtime);
#endif
}

static void TASK_AdvSync(void *arg)
{
    int i=0;
    uint32_t now=LMAC_LL_GetTime();
    uint32_t time=getAdvTime(now);

    //lmac.state=LMAC_STATE_SYNCING;
    switchState(LMAC_STATE_SYNCING);

    if (time-now<3) {
        uint32_t terr=(now-lmac.adv_txtime)/lmac.time_err_rate;
        /* start adv listening */
        lmac_ll_cfg_rx_t cfg={0};
        cfg.freq=lmac_channel_freq_adv[lmac.adv_channel];
        cfg.sf=12;
        cfg.buf=lmac.recv_buf;
        cfg.size=sizeof(lmac.recv_buf);
        cfg.sym_timout=5+((terr<<1)+5+SYM_TIME_SF12-1)/SYM_TIME_SF12;
        LMAC_LL_RadioListen(&cfg);
        setImmeTask(TASK_ProcessAdvSyncResult);
        /* start timeout timer */
        lmac_timer.time=3000;
        LMAC_LL_StartTimer(&lmac_timer);
#ifdef LMAC_DEBUG
        LMAC_DEBUG("adv sync time:%d, error:%d, sym:%d\r\n", now, terr, cfg.sym_timout);
#endif
    }
    else {
#ifdef LMAC_DEBUG
        LMAC_DEBUG("sync delay at %lu\r\n", time);
#endif
        setTimeTask(TASK_AdvSync, time-2);
    }
}

static void TASK_ProcessAdvSyncResult(void *arg)
{
    /* stop timer */
    LMAC_LL_StopTimer();

    if (lmac.recv_len!=0) {
        /* read data */
        lmac.recv_len=LMAC_LL_RadioRecv(lmac.recv_buf);
        /* read rssi and snr */
        int16_t rssi;
        int8_t  snr;
        LMAC_LL_RadioReadPacketRssi(&rssi, &snr);
        lmac.rssi=-rssi;
        lmac.snr=snr;
    }

    if (processChannelAdv(lmac.recv_buf, lmac.recv_len)<0) {
        lmac_statistic.adv_failed++;
        lmac.sync_times++;
#ifdef LMAC_DEBUG
        LMAC_DEBUG("adv sync, success:%d, failed:%d\r\n", lmac_statistic.adv_success, lmac_statistic.adv_failed);
        LMAC_DEBUG("adv tx time:%d\r\n", lmac.adv_txtime);
#endif
        if (lmac.sync_times<LMAC_ADV_MAX_TIMES) {
            setImmeTask(TASK_AdvSync);
            enableImmeTask();
        }
        else {
            lmac.sync_times=0;
            //lmac.state=LMAC_STATE_NOSCAN;
            switchState(LMAC_STATE_NOSCAN);
        }
    }
    else {
        lmac_statistic.adv_success++;
        lmac.sync_times=0;
#ifdef LMAC_DEBUG
        LMAC_DEBUG("bsid:%d\r\n", lmac.adv_bsid);
        LMAC_DEBUG("adv sync, success:%d, failed:%d\r\n", lmac_statistic.adv_success, lmac_statistic.adv_failed);
        LMAC_DEBUG("adv tx time:%d\r\n", lmac.adv_txtime);
#endif
        /* check if send data */
        if (lmac.send_hassyncdata) {
#ifdef LMAC_DEBUG
            LMAC_DEBUG("resend\r\n");
#endif
            //lmac.state=LMAC_STATE_DATASEND;
            switchState(LMAC_STATE_DATASEND);
            setImmeTask(TASK_SendSync);
            enableImmeTask();
        }
        else {
            switchState(LMAC_STATE_SYNCED);
        }
    }
}

static void handleEvent(uint8_t e)
{
    switch(e) {
    case 0:
        setImmeTask(TASK_AdvScan);
        enableImmeTask();
        break;
    case 1:
        setImmeTask(TASK_AdvSync);
        enableImmeTask();
        break;
    case 2:
        setImmeTask(TASK_SendSync);
        enableImmeTask();
        break;
    }
}

static void LMAC_Task(void *arg)
{
    lmac.scan_interval=0;
    lmac.scan_peroid=24000;
    lmac.slot_time=100;
    lmac.channel_solts=100;
    lmac.adv_period=50;
    lmac.state=LMAC_STATE_NOSCAN;
    lmac_timer.func=LMAC_TimerCB;

    while(true) {
        if (lmac_task_time.func==NULL 
         && lmac_task_imme.func==NULL
         && !has_event) {
            LMAC_LL_WaitEvent();
#ifdef LMAC_DEBUG
            LMAC_DEBUG("lmac wakeup, has_event:%d, event_id:%d\r\n", has_event, event_id);
#endif
        }

        if (has_event) {
            handleEvent(event_id);
            has_event=false;
        }

        if (lmac_task_time.func!=NULL) {
            uint32_t now=LMAC_LL_GetTime();
            int32_t delay = (int32_t)(lmac_task_time.time-now);

            if (delay>=600000) {
#ifdef LMAC_DEBUG
                LMAC_DEBUG("delay time %d error\r\n", delay);
#endif          
                bug();
            }

            if (delay<0) {
#ifdef LMAC_DEBUG
                LMAC_DEBUG("send timeout\r\n");
#endif
            }

            if (delay>20 ) {
                delay=lmac_task_time.time-now-20;
#ifdef LMAC_DEBUG
                //LMAC_DEBUG("sleep to %u, cur:%u\r\n", lmac_task_time.time, now);
#endif
                /* sleep some time */
                LMAC_LL_Sleep(delay);
            }
            else {
                //osal_delay(NULL, lmac_task_time.time-now);
                if ((int32_t)(lmac_task_time.time-now)>0)
                    LMAC_LL_Delay(lmac_task_time.time-now);

                lmac_task_func_t func=lmac_task_time.func;
                lmac_task_time.func=NULL;
                func(NULL);
            }
        }

        if (lmac_task_imme.enable&&lmac_task_imme.func!=NULL) {
            lmac_task_func_t func=lmac_task_imme.func;
            lmac_task_imme.func=NULL;
            lmac_task_imme.enable=0;
            func(NULL);
        }

        LMAC_LL_Delay(1);
    }
}

static void LMAC_IntCB(void)
{
    switch(lmac.state) {
    case LMAC_STATE_SCANING:
    case LMAC_STATE_SYNCING:
        lmac.adv_txtime=LMAC_LL_GetTime()-LMAC_ADV_RXTIME_FIXUP;
        lmac.recv_len=10;
#if 0
        /* read data */
        lmac.recv_len=LMAC_LL_RadioRecv(lmac.recv_buf);
        /* read rssi and snr */
        int16_t rssi;
        int8_t  snr;
        LMAC_LL_RadioReadPacketRssi(&rssi, &snr);
        lmac.adv_rssi=-rssi;
        lmac.adv_snr=snr;
#endif
        break;
    case LMAC_STATE_DATARECV:
        lmac.recv_len=10;
        /* read data */
        //lmac.recv_len=LMAC_LL_RadioRecv(lmac.recv_buf);
        break;
    }

    if (lmac_task_imme.func!=NULL) {
        enableImmeTask();
    }
}

void LMAC_LL_IntCB(void)
{
    LMAC_IntCB();
}

/**
  * @brief  init and start lmac stack
  * @param  None
  * @retval None
  * @note   must be called before all other stack operations
  */
void LMAC_Init(void)
{
    LMAC_LL_Init();
    lmac.time_err_rate = LMAC_TIME_ERROR_RATE;
    LMAC_LL_CreateTask(LMAC_Task);
}

void LMAC_Reset(void)
{
    /* reset radio */
    LMAC_LL_Init();
    /* clear all task */
    lmac_task_imme.func=NULL;
    lmac_task_time.func=NULL;
    /* stop timer */
    LMAC_LL_StopTimer();
}

/**
  * @brief  get lmac stack status
  * @param  None
  * @retval -2  busy
  * @retval -1  network break
  * @retval  0  network connected
  */
int LMAC_GetNetState(void)
{
    LMAC_LL_Lock();

    if (has_event) {
        LMAC_LL_Unlock();
        return -2;
    }

    if (lmac.state == LMAC_STATE_SYNCED) {
        LMAC_LL_Unlock();
        return 0;
    }

    if (lmac.state == LMAC_STATE_NOSCAN) {
        LMAC_LL_Unlock();
        return -1; /* don't scan and join gateway */
    }

    LMAC_LL_Unlock();
    return -2; /* busy */
}

/**
  * @brief  start a adverting scan process
  * @param  None
  * @retval -1  start failed
  * @retval  0  start successfully
  */
int LMAC_StartScan(void)
{
    if ((lmac.state != LMAC_STATE_NOSCAN 
      && lmac.state != LMAC_STATE_SYNCED)
      || has_event==true) {
        return -1; /* busy */
    }
  
    LMAC_LL_Lock();
    
    event_id=0;
    has_event=true;
    LMAC_LL_Unlock();

    LMAC_LL_TrigEvent();

    return 0;
}

/**
  * @brief  start a adverting sync process
  * @param  None
  * @retval -1  start failed
  * @retval  0  start successfully
  */
int LMAC_StartSync(void)
{
    if ((lmac.state != LMAC_STATE_NOSCAN 
     && lmac.state != LMAC_STATE_SYNCED)
     || has_event==true) {
        return -1; /* busy */
    }

    LMAC_LL_Lock();
    event_id=1;
    has_event=true;
    LMAC_LL_Unlock();
    
    LMAC_LL_TrigEvent();

    return 0;
}

/**
  * @brief  send data in time slot
  * @param  buf   data buffer and  buf[0] is data sequence number
  * @param  len   data length that contain data sequence number
  * @retval -2    busy
  * @retval -1    net break
  * @retval  0    send success
  */
int LMAC_SyncSend(uplink_data_t *data)
{
    if (lmac.state == LMAC_STATE_NOSCAN) {
        return -1; /* net break */
    }

    if (lmac.state != LMAC_STATE_SYNCED || has_event==true) {
        return -2; /* busy */
    }

    LMAC_LL_Lock();
    if (data->confirm)
        lmac.send_acked=0;
    lmac.send_hassyncdata=true;
    lmac.send_confirm=data->confirm?1:0;
    /* build data */
    buildMacFrame(data);
    event_id=2;
    has_event=true;
    LMAC_LL_Unlock();
    
    LMAC_LL_TrigEvent();
    
    return 0;
}

/**
  * @brief  check send status
  * @param  None
  * @retval -2  no ack for confirm packet
  * @retval -1  find current channel time slot busy
  * @retval  0  send success,but no payload
  * @retval >0  payload length
  * @note   this interface must be called after call send data
  */
int LMAC_GetSyncSendResult(void)
{
    static uint32_t csma=0;

    if (csma != lmac_statistic.send_csma) {
        return -1; /* csma refuse */
    }

    return lmac.send_acked==0?-2:(lmac.recv_len<1?0:lmac.recv_len-1);
}

/**
  * @brief  read donwlink data and param
  * @param  data  downlink data and param buffer
  * @retval -1  no received downlink
  * @retval  0  received donwlink, but no payload data
  * @retval >0  payload length
  * @note   this interface must be called after call send data
  */
int LMAC_ReadDlData(dnlink_data_t *data)
{
    data->rssi=-lmac.rssi;
    data->snr =lmac.snr;

    if (lmac.recv_len<1) {
        return -1; /* no downlink */
    }

    data->fcnt=lmac.recv_buf[0];
    lmac.recv_len--;

    if (lmac.recv_len<1) { /* no payload */
        return 0;
    }

    if (lmac.recv_len>data->size) { /* check if payload is too length */
        lmac.recv_len=data->size;
    }

    memcpy(data->buf, lmac.recv_buf+1, lmac.recv_len);
    
    return lmac.recv_len;
}

/**
  * @brief  get some configure information of lmac stack
  * @param  cfg  information buffer
  * @retval None
  */
void LMAC_GetConfig(lmac_cfg_t *cfg)
{
    cfg->bsid=lmac.adv_bsid;
    cfg->devid=lmac.devid;
    cfg->period=lmac.scan_peroid;
    cfg->slot=lmac.send_slot;
    cfg->slots=lmac.channel_solts;
    cfg->time_error_rate=lmac.time_err_rate;
    return;
}

/**
  * @brief  change some configure information of lmac stack
  * @param  cfg  information buffer
  * @retval None
  */
void LMAC_SetConfig(lmac_cfg_t *cfg)
{
    lmac.devid=cfg->devid;
    lmac.send_slot=cfg->slot;
    lmac.scan_peroid=cfg->period;
    return;
}

/**
  * @brief  set callback function which will be called when lmac switched to 
  * @brief  specified state
  * @param  state  specified state
  * @param  func   callback function
  * @retval -1  no specified state
  * @retval  0  set successfully 
  */
int LMAC_SetStateCB(uint8_t state, void(*func)(void))
{
    if (state>=sizeof(state_cb_func_list)/sizeof(state_cb_func_list[0]))
        return -1;
    
    state_cb_func_list[state]=func;
    
    return 0;
}