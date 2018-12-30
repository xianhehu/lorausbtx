/*======================================
// Q_Shell_User.c头文件
//======================================
#include "Q_Shell_User.h"
======================================*/
#include "DataType.h"
/********************************/
/********************************/


#ifndef	Q_SHELL_USER_DEF
#define	Q_SHELL_USER_DEF
/********************************/

//======================================
// typedef enum {
// 	DRV_IOCTL_LOW,
// 	DRV_IOCTL_HIG,
// 	DRV_IOCTL_MAX
// } PORT_IOCTL_OUT_ENUM;
//======================================
typedef union
{
	uint32 flagu32;
	struct
	{
		uint32 len            : 8; // csend send length
		uint32 csenden        : 1; // csend en
		uint32 epten          : 1; // Enter Pass-through mode en
		uint32 notused        : 10; // not used
	}bits;
}QSHELL_FLAG;
//======================================
typedef struct /* 缓存池 */
{
	QSHELL_FLAG flag;
	uint32 cnt;
	uint32 cerr;
}STR_QSHELL_COMMAND;
//======================================
extern STR_QSHELL_COMMAND strQShellConnmand;
//======================================
#define QSF_CSEND		(1<< 0 )	/* 连续发送状态标志 */
extern uint8 QShell_Flag;	//状态标志位

//======================================


//======================================
extern void send(uint8 len);
extern void listen( void );

/********************************/
extern uint8 QShell_csend(void);




/********************************/
// extern void Drv_IOCtl_Init(void);
// extern void Drv_IOCtl_Out(uint8_t pin, uint8_t bhl);
// extern void Drv_IOCtl_Not(uint8_t pin);
// extern uint8_t Drv_IOCtl_In(uint8_t pin);
// extern void Drv_IOCtl_DirSet(uint8_t pin, uint8_t dir);

/********************************/
#endif	//Q_SHELL_USER_DEF


