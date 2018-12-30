#include "os_cfg.h"
#include "os.h"


static os_tcb_t *tcblist[10] = {NULL};
uint32_t  tcbnum  = 0;
os_tcb_t* tcbcur  = NULL;
os_tcb_t* tcbhigh = NULL;
uint32_t* OS_CPU_ExceptStkBase = (uint32_t*)OS_CFG_MAIN_STACK;

static uint32_t  osticks = 0;
static uint8_t   osintnestingcnt = 0;
static bool     osrunning = false;
static uint8_t   oslocks[10] = {1};

extern void OSCtxSw(void);
extern void OSIntCtxSw(void);
extern void OSStartHighRdy(void);


bool CheckEvent(os_tcb_t *tcb, uint32_t event, uint8_t type)
{
    uint32_t e = (event & 0x00FFFFFF) | (type << 24);
    
    if (e != tcb->event)
        return false;
    
    return true;
}

bool IsReady(os_tcb_t *tcb)
{
    uint32_t event = tcb->event & 0x00FFFFFF;
    uint8_t  type  = tcb->event >> 24;
    
    // 没有事件
    if (tcb->event == 0)
        return true;
    
    // 延时操作
    if (event == 0) {
        if (osticks >= tcb->target)
            return true;
        else
            return false;
    }
    
    switch(type) {
    case 0:
        if (osticks < tcb->target)
            return false;
        
        return true;
    case 2:
        if (osticks >= tcb->target)
            return true;
        
        return false;
    case 1:
    case 3:
        return false;
        
    default:
        return false;
    }
}

void OSGetHighRdy()
{
    os_tcb_t* high = NULL;

    for (int i = 0; i < tcbnum; i++) {
        if (!IsReady(tcblist[i])) {
            continue;
        }
        
        if (high == NULL) {
            high = tcblist[i];
            
            continue;
        }
        
        if (tcblist[i]->prior < high->prior) {
            high = tcblist[i];
        }
    }

    if (high == NULL)
        while(1);

    tcbhigh = high;
}

void Shed(void)
{
    if (!osrunning)
        return;
    
    OSCtxSw();
}

void Start(void)
{
    OSGetHighRdy();
    tcbcur = tcbhigh;
    OSStartHighRdy();
}

void  os_intenter (void)
{
    if (osintnestingcnt >= 250u) {                          /* Have we nested past 250 levels?                        */
        return;                                             /* Yes                                                    */
    }

    osintnestingcnt++;                                      /* Increment ISR nesting level                            */
}

void  os_intexit (void)
{
    CPU_SR_ALLOC();

    CPU_INT_DIS();
    
    if (osintnestingcnt == 0) {             /* Prevent OSIntNestingCtr from wrapping                  */
        CPU_INT_EN();
        return;
    }
    
    osintnestingcnt--;
    
    if (osintnestingcnt > 0) {              /* ISRs still nested?                                     */
        CPU_INT_EN();                                       /* Yes                                                    */
        return;
    }
    
    OSGetHighRdy();
    
    if (tcbhigh == tcbcur) {                   /* Current task still the highest priority?               */
        CPU_INT_EN();                                       /* Yes                                                    */
        return;
    }

    //OSTaskCtxSwCtr++;                                       /* Keep track of the total number of ctx switches         */

    OSIntCtxSw();                                           /* Perform interrupt level ctx switch                     */

    CPU_INT_EN();
}

uint32_t os_gettime(void)
{
    return osticks;
}

void os_delay(uint32_t ticks)
{
    tcbcur->target = osticks + ticks;
    tcbcur->event  = 1 << 24;
    Shed();
}

int os_lock(uint32_t lock, uint32_t timeout)
{
    if (oslocks[lock] > 1) {
        while(1);
    }
    
    return os_wait(lock, timeout);
}

void os_unlock(uint32_t lock)
{
    // no wait lock
    if (oslocks[lock] > 0) {
        while(1);
    }

    os_post(lock);
}

int os_wait(uint32_t lock, uint32_t timeout)
{
    // lock available
    CPU_SR_ALLOC();
    
    CPU_INT_DIS();
    
    if (oslocks[lock] > 0) {
        oslocks[lock]--;
        CPU_INT_EN();
        
        return 0;
    }

    // wait lock
    if (timeout > 0)
        lock |= 2 << 24;
    else
        lock |= 3 << 24;
    
    tcbcur->event = lock;
    tcbcur->target = osticks + timeout;
    
    CPU_INT_EN();
    
    Shed();
    
    CPU_INT_DIS();
    
    if (tcbcur->event == 0) {
        CPU_INT_EN();
        
        return 0;
    }
    
    tcbcur->event = 0;
    
    CPU_INT_EN();
    
    return -1;
}

void os_initevent(uint32_t lock, uint8_t cnt)
{
    oslocks[lock] = cnt;
}

void os_post(uint32_t lock)
{
    // lock ready
    for (int i = 0; i < tcbnum; i++) {
        if (!CheckEvent(tcblist[i], lock, 2) && !CheckEvent(tcblist[i], lock, 3)) {
            continue;
        }
        
        tcblist[i]->event  = 0;
        tcblist[i]->target = 0;
        
        Shed();
        
        return;
    }
    
    //lock resume
    oslocks[lock]++;
}

void os_tick(void)
{
    osticks++;
    
    Shed();
}

uint32_t os_getticks(void)
{
    return osticks;
}

void os_setticks(uint32_t ticks)
{
    osticks = ticks;
}

static void task_return(void)
{
}

uint32_t *os_stackinit (void (*f) (void *arg),
                         void             *arg,
                         uint32_t          *s,
                         uint32_t          l)
{
    uint32_t *stk;

    stk = &s[l];                                          /* Load stack pointer                                     */
                                                          /* Align the stack to 8-bytes.                            */
    stk = (uint32_t *)((uint32_t)(stk) & 0xFFFFFFF8);
                                                          /* Registers stacked as if auto-saved on exception        */
    *--stk = (uint32_t)0x01000000u;                       /* xPSR                                                   */
    *--stk = (uint32_t)f;                                 /* Entry Point                                            */
    *--stk = (uint32_t)task_return;                       /* R14 (LR)                                               */
    *--stk = (uint32_t)0x12121212u;                       /* R12                                                    */
    *--stk = (uint32_t)0x03030303u;                       /* R3                                                     */
    *--stk = (uint32_t)0x02020202u;                       /* R2                                                     */
    *--stk = (uint32_t)s;                                 /* R1                                                     */
    *--stk = (uint32_t)arg;                               /* R0 : argument                                          */
                                                          /* Remaining registers saved on process stack             */
    *--stk = (uint32_t)0x07070707u;                       /* R7                                                     */
    *--stk = (uint32_t)0x06060606u;                       /* R6                                                     */
    *--stk = (uint32_t)0x05050505u;                       /* R5                                                     */
    *--stk = (uint32_t)0x04040404u;                       /* R4                                                     */

    return (stk);
}

void os_createtsk(os_tcb_t *t, void (*f) (void *arg), void *arg, uint32_t *s, uint32_t l, uint8_t prior)
{
    t->pc  = (void *)f;
    t->psp = os_stackinit(f, arg, s, l);
    t->prior = prior;

    tcblist[tcbnum++] = t;

    Shed();
}

void os_run(void)
{
    osrunning = true;
    Start();
}
