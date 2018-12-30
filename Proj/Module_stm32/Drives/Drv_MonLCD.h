/*======================================
// Drv_MonLCD.c头文件
// (monochrome)黑白显示屏驱动
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
// 字体尺寸定义
// #include "Font_1.h"
typedef enum {
	DRV_MONLCD_FONT1,	//8
	DRV_MONLCD_FONT2,	//12 
	DRV_MONLCD_FONT3,	//16 
	DRV_MONLCD_FONT_TYPE_MAX
} DRV_MONLCD_FONT_ENUM;	//字模大小定义
//======================================




/********************************/
//======================================
typedef enum {
	DRV_MONLCD_BLOFF,	//背光灯关
	DRV_MONLCD_BLON,	//背光灯开 
	DRV_MONLCD_BLSET_MAX
} DRV_MONLCD_BLSET_ENUM;	//背光灯操作定义
//======================================
typedef enum {
	DRV_MONLCD_NORMAL,		//字体显示的时候有背景颜色
	DRV_MONLCD_REVERSAL,	//字体显示的时候反色显示
	DRV_MONLCD_CURSOR,		//字体显示增加光标
	DRV_MONLCD_DISPMODE_MAX
} DRV_MONLCD_DISPMOD_ENUM;	//显示模式定义
//======================================
typedef enum {
	DRV_MONLCD_ROT_0,		//屏幕显示旋转0°
	DRV_MONLCD_ROT_180,		//屏幕显示旋转180°
// #if (MONLCD_MAX_X == MONLCD_MAX_Y)
	DRV_MONLCD_ROT_90,		//屏幕显示旋转90°
	DRV_MONLCD_ROT_270,		//屏幕显示旋转270°
// #endif
	DRV_MONLCD_ROTATIONMODE_MAX
} DRV_MONLCD_DISPROTATION_ENUM;	//屏幕显示旋转模式定义
//======================================
typedef enum {
	DRV_MONLCD_CURSOR1,	//_
// 	DRV_MONLCD_CURSOR2,	// 
	DRV_MONLCD_CURSOR_MAX
} DRV_MONLCD_CURSOR_ENUM;	//光标类型定义
//======================================

//------------------------------------------------
// typedef enum {
// #define FONT_DEFINE(font_name)  DRV_MONLCD_##font_name,
// #include "Font_Def.h"
// 	DRV_MONLCD_FONT_TYPE_MAX
// } DRV_MONLCD_FONT_ENUM;	//字模大小定义
//------------------------------------------------


/********************************/
/********************************/
#include "string.h"
extern uint8 MyPrintf(uint8 *buf, const char *pFormat, ...);



/********************************/
extern void Drv_MonLCD_Init(void);	//初始化
extern void Drv_MonLCD_BLSet(uint8 bhl);	//背光灯操作(0-OFF, 1-ON)
extern void Drv_MonLCD_DrawRefresh(void);	//更新显示
extern void Drv_MonLCD_ClearDrawBuf(uint8 mod);	//清除显示缓存
extern void Drv_MonLCD_DisStr(uint8 x, uint8 y, uint8 *pstr, uint8 mod, uint8 font);	//字符串显示
extern void Drv_MonLCD_DisStrExp(uint8 x, uint8 y, uint8 *pstr, uint8 mod, uint8 font, uint8 ex/*, uint8 ey*/);	//字符串扩展显示
extern void Drv_MonLCD_ContrastSet(uint8 dat);	//对比度设置
extern void Drv_MonLCD_DisAddFontCursor(uint8 x, uint8 y, uint8 asciinum, uint8 mod, uint8 font, uint8 ex/*, uint8 ey*/);	//增加光标显示

/********************************/
extern uint8 Drv_MonLCD_GetFontY( uint8 font );	//获取字体字高

/********************************/
#endif	//DRV_MONLCD_DEF


