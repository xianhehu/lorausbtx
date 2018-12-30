#ifndef __OS_H__
#define __OS_H__
#include "types.h"

extern void CPU_SR_Restore(uint32_t s);
extern uint32_t CPU_SR_Save(void);

#define  CPU_SR_ALLOC()        uint32_t  cpu_sr = 0
#define  CPU_INT_DIS()         do { cpu_sr = CPU_SR_Save(); } while (0) /* Save    CPU status word & disable interrupts.*/
#define  CPU_INT_EN()          do { CPU_SR_Restore(cpu_sr); } while (0) /* Restore CPU status word.                     */

typedef void (*os_task_t) (void *arg);

typedef struct {
    uint32_t  *psp;
    void      *pc;
    uint32_t  event;
    uint32_t  target;
    uint8_t   prior;
    void      *arg;
} os_tcb_t;

void  os_run(void);
void  os_tick(void);
void  os_delay(uint32_t ticks);
void  os_initevent(uint32_t lock, uint8_t cnt);
int   os_wait(uint32_t sem, uint32_t timeout);
void  os_post(uint32_t sem);
int   os_lock(uint32_t lock, uint32_t timeout);
void  os_unlock(uint32_t lock);
void  os_setticks(uint32_t ticks);
void  os_createtsk(os_tcb_t *t, void (*f) (void *arg), void *arg, 
                  uint32_t *s, uint32_t l, uint8_t prior);
void  os_intenter (void);
void  os_intexit (void);

uint32_t os_getticks(void);

#endif