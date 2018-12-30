/*======================================
// �������ͺ궨��
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
typedef unsigned char				uint8;		/* �޷���  8 bit */
typedef signed   char				sint8;		/* �з���  8 bit */
typedef unsigned short int			uint16;		/* �޷���  16 bit */
typedef signed   short int			sint16;		/* �з���  16 bit */
typedef unsigned long int			uint32;		/* �޷���  32 bit */
typedef signed   long int			sint32;		/* �з���  32 bit */
typedef unsigned long long int		uint64;		/* �޷���  64 bit */
typedef signed   long long int		sint64;		/* �з���  64 bit */
typedef float						float32;	/* �����ȸ����� */
typedef double						float64;	/* ˫���ȸ����� */

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
// ��Ƭ�����Ͷ��壬���������ð���·����ҳ���ڵ��Ǹ��궨��
/********************************/
/********************************/
#include "stm32f0xx_conf.h"
#include "stm32f0xx.h"
/********************************/


/********************************/
/********************************/













