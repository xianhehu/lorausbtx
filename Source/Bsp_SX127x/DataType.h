/*======================================
// 数据类型宏定义
//======================================
#include "DataType.h"
======================================*/
/********************************/
/********************************/


#ifndef	DATA_TYPE_DEF
#define	DATA_TYPE_DEF

#include <stdint.h>
#include <stdbool.h>

/********************************/
typedef unsigned char				uint8;		/* 无符号  8 bit */
typedef signed   char				sint8;		/* 有符号  8 bit */
typedef unsigned short int			uint16;		/* 无符号  16 bit */
typedef signed   short int			sint16;		/* 有符号  16 bit */
typedef unsigned long int			uint32;		/* 无符号  32 bit */
typedef signed   long int			sint32;		/* 有符号  32 bit */
typedef unsigned long long int		uint64;		/* 无符号  64 bit */
typedef signed   long long int		sint64;		/* 有符号  64 bit */
typedef float						float32;	/* 单精度浮点数 */
typedef double						float64;	/* 双精度浮点数 */

#ifndef bool
typedef	unsigned char	bool;
#endif

#ifndef FALSE
#define	FALSE  (1==0)
#endif
#ifndef TRUE
#define	TRUE   (1==1)
#endif

#ifndef true
#define true TRUE
#endif

#ifndef false
#define false FALSE
#endif

#ifndef NULL
#define NULL    ((void *)0)
#endif

/********************************/

#define BIT0				0x00000001
#define BIT1				0x00000002
#define BIT2				0x00000004
#define BIT3				0x00000008
#define BIT4				0x00000010
#define BIT5				0x00000020
#define BIT6				0x00000040
#define BIT7				0x00000080
#define BIT8				0x00000100
#define BIT9				0x00000200
#define BIT10				0x00000400
#define BIT11				0x00000800
#define BIT12				0x00001000
#define BIT13				0x00002000
#define BIT14				0x00004000
#define BIT15				0x00008000
#define BIT16				0x00010000
#define BIT17				0x00020000
#define BIT18				0x00040000
#define BIT19				0x00080000
#define BIT20				0x00100000
#define BIT21				0x00200000
#define BIT22				0x00400000
#define BIT23				0x00800000
#define BIT24				0x01000000
#define BIT25				0x02000000
#define BIT26				0x04000000
#define BIT27				0x08000000
#define BIT28				0x10000000
#define BIT29				0x20000000
#define BIT30				0x40000000
#define BIT31				0x80000000



/********************************/
#endif	//DATA_TYPE_DEF


/********************************/
// 单片机类型定义，定义在设置包含路径的页面内的那个宏定义
/********************************/
/********************************/
#include "stm32f0xx_conf.h"
#include "stm32f0xx.h"
/********************************/


/********************************/
/********************************/













