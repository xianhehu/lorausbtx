/*======================================
// Drv_IOCtl.cÍ·ÎÄ¼þ
//======================================
#include "Drv_IOCtl.h"
======================================*/
#include "DataType.h"
/********************************/
/********************************/


#ifndef	DRV_IOCTL_DEF
#define	DRV_IOCTL_DEF
/********************************/

//======================================
typedef enum {
	DRV_IOCTL_LOW,
	DRV_IOCTL_HIG,
	DRV_IOCTL_MAX
} PORT_IOCTL_OUT_ENUM;
//======================================
//======================================

//======================================
typedef enum {
	IOCtl_PIN_LED1_D4,
	IOCtl_PIN_LED2_D5,
	IOCtl_PIN_LED3_D6,
	//------------------------
	//------------------------
	IOCtl_PIN_MAX
} DRV_IOCtl_PIN_ENUM;
//======================================




/********************************/




/********************************/
extern void Drv_IOCtl_Init(void);
extern void Drv_IOCtl_Out(uint8_t pin, uint8_t bhl);
extern void Drv_IOCtl_Not(uint8_t pin);
extern uint8_t Drv_IOCtl_In(uint8_t pin);
extern void Drv_IOCtl_DirSet(uint8_t pin, uint8_t dir);
extern void Drv_IOCtl_PowerOn(void);

/********************************/
#endif	//DRV_IOCTL_DEF


