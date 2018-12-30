/*======================================
// Drv_MonLCD.cͷ�ļ�
// (monochrome)�ڰ���ʾ������
//======================================
#include "Drv_MonLCD.h"
======================================*/
#include "DataType.h"
/********************************/
/********************************/


#ifndef	DRV_MONLCD_DEF
#define	DRV_MONLCD_DEF
#define	DRV_MONLCD_USED	1
/********************************/




/********************************/
// ����ߴ綨��
// #include "Font_1.h"
typedef enum {
	DRV_MONLCD_FONT1,	//8
	DRV_MONLCD_FONT2,	//12 
	DRV_MONLCD_FONT3,	//16 
	DRV_MONLCD_FONT_TYPE_MAX
} DRV_MONLCD_FONT_ENUM;	//��ģ��С����
//======================================




/********************************/
//======================================
typedef enum {
	DRV_MONLCD_BLOFF,	//����ƹ�
	DRV_MONLCD_BLON,	//����ƿ� 
	DRV_MONLCD_BLSET_MAX
} DRV_MONLCD_BLSET_ENUM;	//����Ʋ�������
//======================================
typedef enum {
	DRV_MONLCD_NORMAL,		//������ʾ��ʱ���б�����ɫ
	DRV_MONLCD_REVERSAL,	//������ʾ��ʱ��ɫ��ʾ
	DRV_MONLCD_CURSOR,		//������ʾ���ӹ��
	DRV_MONLCD_DISPMODE_MAX
} DRV_MONLCD_DISPMOD_ENUM;	//��ʾģʽ����
//======================================
typedef enum {
	DRV_MONLCD_ROT_0,		//��Ļ��ʾ��ת0��
	DRV_MONLCD_ROT_180,		//��Ļ��ʾ��ת180��
// #if (MONLCD_MAX_X == MONLCD_MAX_Y)
	DRV_MONLCD_ROT_90,		//��Ļ��ʾ��ת90��
	DRV_MONLCD_ROT_270,		//��Ļ��ʾ��ת270��
// #endif
	DRV_MONLCD_ROTATIONMODE_MAX
} DRV_MONLCD_DISPROTATION_ENUM;	//��Ļ��ʾ��תģʽ����
//======================================
typedef enum {
	DRV_MONLCD_CURSOR1,	//_
// 	DRV_MONLCD_CURSOR2,	// 
	DRV_MONLCD_CURSOR_MAX
} DRV_MONLCD_CURSOR_ENUM;	//������Ͷ���
//======================================

//------------------------------------------------
// typedef enum {
// #define FONT_DEFINE(font_name)  DRV_MONLCD_##font_name,
// #include "Font_Def.h"
// 	DRV_MONLCD_FONT_TYPE_MAX
// } DRV_MONLCD_FONT_ENUM;	//��ģ��С����
//------------------------------------------------


/********************************/
/********************************/
#include "string.h"
extern uint8 MyPrintf(uint8 *buf, const char *pFormat, ...);



/********************************/
extern void Drv_MonLCD_Init(void);	//��ʼ��
extern void Drv_MonLCD_BLSet(uint8 bhl);	//����Ʋ���(0-OFF, 1-ON)
extern void Drv_MonLCD_DrawRefresh(void);	//������ʾ
extern void Drv_MonLCD_ClearDrawBuf(uint8 mod);	//�����ʾ����
extern void Drv_MonLCD_DisStr(uint8 x, uint8 y, uint8 *pstr, uint8 mod, uint8 font);	//�ַ�����ʾ
extern void Drv_MonLCD_DisStrExp(uint8 x, uint8 y, uint8 *pstr, uint8 mod, uint8 font, uint8 ex/*, uint8 ey*/);	//�ַ�����չ��ʾ
extern void Drv_MonLCD_ContrastSet(uint8 dat);	//�Աȶ�����
extern void Drv_MonLCD_DisAddFontCursor(uint8 x, uint8 y, uint8 asciinum, uint8 mod, uint8 font, uint8 ex/*, uint8 ey*/);	//���ӹ����ʾ

/********************************/
extern uint8 Drv_MonLCD_GetFontY( uint8 font );	//��ȡ�����ָ�

/********************************/
#endif	//DRV_MONLCD_DEF


