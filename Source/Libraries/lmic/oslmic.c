/*******************************************************************************
 * Copyright (c) 2014-2015 IBM Corporation.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    IBM Zurich Research Lab - initial API, implementation and documentation
 *******************************************************************************/

#include "lmic.h"
#include "lmiclowlevelapi.h"
#include "osal.h"
#include "lmicapi.h"
#include "lmic_task.h"
#include <string.h>

static osjob_t initjob;

#if 0
// application router ID (LSBF)
static u1_t APPEUI[8]  = { 0xBE, 0x7A, 0x00, 0x0B, 0xE7, 0xA0, 0x00, 0x40 };//0x02, 0x00, 0x00, 0x00, 0x00, 0xEE, 0xFF, 0xC0 };

// unique device ID (LSBF)
static u1_t DEVEUI[8]  = { 0xBE, 0x7A, 0x00, 0x0B, 0xE7, 0xA0, 0x00, 0x45 };//0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };

// device-specific AES key (derived from device EUI)
static u1_t DEVKEY[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15  };

#else
// application router ID (LSBF)
static u1_t APPEUI[8]  = { 1, 0, 0, 0, 0, 0, 0, 0 };//0x02, 0x00, 0x00, 0x00, 0x00, 0xEE, 0xFF, 0xC0 };

// unique device ID (LSBF)
static u1_t DEVEUI[8]  = { 1, 0, 0, 0, 0, 0, 0, 0 };//0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };

// device-specific AES key (derived from device EUI)
static u1_t DEVKEY[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15  };

#endif

#if 0
static u1_t nwkKey[16] = { 0xE7, 0x63, 0x2E, 0x1C, 0xF3, 0x61, 0x7E, 0xAD, 0xF5, 0xC0, 0xBC, 0x7E, 0x38, 0xEA, 0x09, 0xA8  };

static u1_t artKey[16] = { 0xE7, 0x63, 0x2E, 0x1C, 0xF3, 0x61, 0x7E, 0xAD, 0xF5, 0xC0, 0xBC, 0x7E, 0x38, 0xEA, 0x09, 0xA8  };
#endif

static uint8_t rxbuf[200];

static uint8_t rxlen = 0;

static uint8_t txlen = 0;

static uint8_t txmode = 0;

static uint8_t txport = 0;

//////////////////////////////////////////////////
// APPLICATION CALLBACKS
//////////////////////////////////////////////////

// provide application router ID (8 bytes, LSBF)
void os_getArtEui (u1_t* buf) 
{
    CPU_DisableInt();
    memcpy(buf, APPEUI, 8);
    CPU_EnableInt();
}

void os_setArtEui (u1_t* buf) 
{
    CPU_DisableInt();
    memcpy(APPEUI, buf, 8);
    CPU_EnableInt();
}

// provide device ID (8 bytes, LSBF)
void os_getDevEui (u1_t* buf) 
{
    CPU_DisableInt();
    memcpy(buf, DEVEUI, 8);
    CPU_EnableInt();
}

void os_setDevEui (u1_t* buf) 
{
    CPU_DisableInt();
    memcpy(DEVEUI, buf, 8);
    CPU_EnableInt();
}

// provide device key (16 bytes)
void os_getDevKey (u1_t* buf) 
{
    CPU_DisableInt();
    memcpy(buf, DEVKEY, 16);
    CPU_EnableInt();
}

void os_setDevKey (u1_t* buf) 
{
    CPU_DisableInt();
    memcpy(DEVKEY, buf, 16);
    CPU_EnableInt();
}

//////////////////////////////////////////////////
// MAIN - INITIALIZATION AND STARTUP
//////////////////////////////////////////////////

// initial job
static void initfunc (osjob_t* j) {
   //LMIC_setSession(0x12345678, 0xB710566C, nwkKey, artKey);

   LMIC_startJoining();	// start joining
   // init done - onEvent() callback will be invoked...
}

//////////////////////////////////////////////////
// LMIC EVENT CALLBACK
//////////////////////////////////////////////////

void onEvent (ev_t ev)
{
    //debug_event(ev);

    switch(ev)
    {
      // network joined, session established
      case EV_JOINED:
          LMIC_setTxData2(1, LMIC.frame, 1, 0);	// schedule transmission (port 1, datalen 1, no ack requested)
    	  break;

      // scheduled data sent (optionally data received)
      case EV_TXCOMPLETE:
          //LMIC.dataLen = 10; /* for rx test */
          if(LMIC.dataLen) { // data received in rx slot after tx
              //debug_buf(LMIC.frame+LMIC.dataBeg, LMIC.dataLen);
              memcpy(rxbuf, LMIC.frame+LMIC.dataBeg, LMIC.dataLen);
              rxlen = LMIC.dataLen;
          }
          
          if( (LMIC.opmode & (OP_JOINING|OP_REJOIN|OP_TXDATA|OP_POLL)) == 0 ) {
              netstatus = NET_STATUS_IDLE;
              //PWR_EnterSleepMode(PWR_SLEEPEntry_WFI);
          }
          txlen = 0;
          
          break;
      case EV_LINK_DEAD:
          netstatus = NET_STATUS_BREAK;
          break;
    }
}

// RUNTIME STATE
static struct {
    osjob_t* scheduledjobs;
    osjob_t* runnablejobs;
} OS;

void os_init () {
    memset(&OS, 0x00, sizeof(OS));
    LMIC_init();
    os_setCallback(&initjob, initfunc);	// setup initial job
}

static u2_t deltaticks (u4_t time)
{
    u4_t t = osal_getTime();
    s4_t d = time - t;
    if( d<=0 ) return 0;    // in the past
    if( (d>>16)!=0 ) return 0xFFFF; // far ahead
    return (u2_t)d;
}

void os_waitUntil (u4_t time)
{
    while( deltaticks(time) != 0 ); // busy wait until timestamp is reached
}

static u1_t OS_checkTime (u4_t time)
{
    u2_t dt=0;
    dt = dt;
    if((dt = deltaticks(time)) < 5) // event is now (a few ticks ahead)
    {
        return 1;
    }
    
    return 0;
}

ostime_t os_getTime () {
    return osal_getTime();
}

static u1_t unlinkjob (osjob_t** pnext, osjob_t* job) {
    for( ; *pnext; pnext = &((*pnext)->next)) {
        if(*pnext == job) { // unlink
            *pnext = job->next;
            return 1;
        }
    }
    return 0;
}

// clear scheduled job
void os_clearCallback (osjob_t* job) {
    CPU_DisableInt();
    unlinkjob(&OS.scheduledjobs, job) || unlinkjob(&OS.runnablejobs, job);
    CPU_EnableInt();
}

// schedule immediately runnable job
void os_setCallback (osjob_t* job, osjobcb_t cb) {
    osjob_t** pnext;
    CPU_DisableInt();
    // remove if job was already queued
    os_clearCallback(job);
    // fill-in job
    job->func = cb;
    job->next = NULL;
    // add to end of run queue
    for(pnext=&OS.runnablejobs; *pnext; pnext=&((*pnext)->next));
    *pnext = job;
    CPU_EnableInt();
}

// schedule timed job
void os_setTimedCallback (osjob_t* job, ostime_t time, osjobcb_t cb) {
    osjob_t** pnext;
    CPU_DisableInt();
    // remove if job was already queued
    os_clearCallback(job);
    // fill-in job
    job->deadline = time;
    job->func = cb;
    job->next = NULL;
    // insert into schedule
    for(pnext=&OS.scheduledjobs; *pnext; pnext=&((*pnext)->next)) {
        if((*pnext)->deadline - time > 0) { // (cmp diff, not abs!)
            // enqueue before next element and stop
            job->next = *pnext;
            break;
        }
    }
    *pnext = job;
    CPU_EnableInt();
}

void txtest (void);

// execute jobs from timer and from run queue
void lmic_task (void)
{
    osjob_t* j = NULL;

    CPU_DisableInt();
    // check for runnable jobs
    if(OS.runnablejobs) {
        j = OS.runnablejobs;
        OS.runnablejobs = j->next;
    } else if(OS.scheduledjobs && OS_checkTime(OS.scheduledjobs->deadline)) { // check for expired timed jobs
        j = OS.scheduledjobs;
        OS.scheduledjobs = j->next;
    } else {// nothing pending
        //hal_sleep(); // wake by irq (timer already restarted)
        
    }
    
    if (LMIC.pendTxLen == 0 && txlen > 0) {
        CPU_DisableInt();
        if (txport == 0) {
            txport = 1;
        }
        
        LMIC_setTxData2(txport, LMIC.pendTxData, txlen, txmode);
        CPU_EnableInt();
    }
    CPU_EnableInt();
    if(j) { // run job callback
        j->func(j);
    }
}

void lmic_init(void)
{
    LMIC_reset();	// reset MAC state
    if (flash_param.valid==0) { /* save default param to flash */
        memcpy(flash_param.appeui, APPEUI, 8);
        memcpy(flash_param.appkey, DEVKEY, 16);
        lmicapi_saveparam();
    }
    lmicapi_readid();
    TIMER_TxRxTimeoutCreate(5000);
}

bool lmic_idle(void)
{
    CPU_DisableInt();
    if (netstatus == NET_STATUS_IDLE && txlen == 0) {
        CPU_EnableInt();
        return true;
    }
    CPU_EnableInt();
    return false;
}

bool lmic_isconnect(void)
{
    return netstatus != NET_STATUS_BREAK;
}

int lmic_send(u1_t port, u1_t *data, u1_t len, u1_t mode)
{
    CPU_DisableInt();
    
    if (len > MAX_LEN_PAYLOAD) {
        return -2;
    }
    
    if (txlen > 0) {     /* check if network is busy */
        CPU_EnableInt();
        return -3;
    }
    
    txlen = len;
    txport = port;
    txmode = mode;
    memcpy(LMIC.pendTxData, data, len);
    netstatus = NET_STATUS_BUSY;
    CPU_EnableInt();
    return 0;
}

int lmic_receive(uint8_t *data)
{
    int ret = 0;
    CPU_DisableInt();
    if (rxlen > 0) {
        memcpy(data, rxbuf, rxlen);
        ret = rxlen;
        rxlen = 0;
    }
    CPU_EnableInt();
    return ret;
}

int lmic_setbasefreq(u4_t txfreq, u4_t rxfreq)
{
    if (lmic_isconnect() == true) {
        return -1;
    }
    LMIC.basefreq_tx = txfreq;
    LMIC.basefreq_rx = rxfreq;
    flash_param.txfreq=txfreq;
    flash_param.rxfreq=rxfreq;
    lmicapi_saveparam();
    
    return 0;
}

int lmic_setband(u1_t band)
{
    CPU_DisableInt();
    LMIC.startband = band;
    CPU_EnableInt();
    flash_param.band=band;
    lmicapi_saveparam();
    
    return 0;
}

int lmic_setsf(u1_t sf)
{
    CPU_DisableInt();
    LMIC.datarate = 5-(sf-7);
    CPU_EnableInt();
    flash_param.datarate=LMIC.datarate;
    lmicapi_saveparam();
    
    return 0;
}

int lmic_setrx2param(u4_t freq, u1_t dr)
{
    LMIC.dn2Freq = freq;
    LMIC.dn2SDr  = dr;
    LMIC.dn2Dr   = dr;
    flash_param.rx2freq=freq;
    flash_param.rx2dr=dr;
    lmicapi_saveparam();
    
    return 0;
}

void lmic_getrx2param(u4_t *freq, u1_t *dr)
{
    *freq = LMIC.dn2Freq;
    *dr = LMIC.dn2Dr;
}

/* 
* get data transfer status
* return 0:successful, -1:failed
*/
int lmic_gettrasstatus(void)
{
    if ((LMIC.txrxFlags & TXRX_NACK) != 0) {
        return -3;
    }
    return 0;
}