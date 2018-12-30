/*======================================
// Drv_MonLCD.c文件
// (monochrome)黑白显示屏驱动
//======================================
======================================*/
#include "Drv_MonLCD.h"
/********************************/
// LCD驱动
#include "UC1701X_Drive.h"
/********************************/


#if DRV_MONLCD_USED
/********************************/
//======================================
//显示旋转角度设置
#define DRV_MONLCD_DISPROTATION_MOD		DRV_MONLCD_ROT_0

/********************************/







/********************************/
// 字体尺寸定义
/********************************/
#include "Font_1.h"
#include "Font_2.h"
#include "Font_3.h"
/********************************/
//字体：ASCII字宽
const uint8 FontSizeXA[DRV_MONLCD_FONT_TYPE_MAX] = {
#define FONT_DEFINE(font_name)  font_name##_SIZE_XA,
#include "Font_Def.h"
};
//--------------------------------------
//字体：汉字字宽
const uint8 FontSizeXH[DRV_MONLCD_FONT_TYPE_MAX] = {
#define FONT_DEFINE(font_name)  font_name##_SIZE_XH,
#include "Font_Def.h"
};
//--------------------------------------
//字体：字高
const uint8 FontSizeY[DRV_MONLCD_FONT_TYPE_MAX] = {
#define FONT_DEFINE(font_name)  font_name##_SIZE_Y,
#include "Font_Def.h"
};
//--------------------------------------
//字体：单个ASCII字符占用字节数
const uint8 FontSizeOfLib[DRV_MONLCD_FONT_TYPE_MAX] = {
#define FONT_DEFINE(font_name)  font_name##_SIZEOF_LIB,
#include "Font_Def.h"
};
//======================================
//字体：汉字字库大小
const uint16 FontLibHZ_Size[DRV_MONLCD_FONT_TYPE_MAX] = {
#define FONT_DEFINE(font_name)  font_name##_LIBHZ_SIZE,
#include "Font_Def.h"
};
//======================================
//字体：ASCII码字库
const uint8 *FontLib[DRV_MONLCD_FONT_TYPE_MAX] = {
#define FONT_DEFINE(font_name)  font_name##_Lib,
#include "Font_Def.h"
};
//======================================
//字体：汉字字库
const uint8 *FontLibHZ[DRV_MONLCD_FONT_TYPE_MAX] = {
#define FONT_DEFINE(font_name)  font_name##_LibHZ,
#include "Font_Def.h"
};
//======================================
//字体：ASCII码文字光标
const uint8 *FontCursorLib[DRV_MONLCD_FONT_TYPE_MAX] = {
#define FONT_DEFINE(font_name)  font_name##_Cursor_Lib,
#include "Font_Def.h"
};
//======================================




/********************************/
void Drv_MonLCD_DisASCII(uint8 x, uint8 y, uint8 *pstr, uint8 mod, uint8 font);
void Drv_MonLCD_DisHZ(uint8 x, uint8 y, uint8 *pstr, uint8 mod, uint8 font);







/********************************/
#if 1
//==============================================================================
// 编译时间
uint8 const VER_BuildDate[]= {__DATE__};//12Byte
uint8 const VER_BuildTime[]= {__TIME__};//9Byte
//==============================================================================
// #pragma data_alignment = 8 //如果不是8字节对齐，sprintf的浮点数将输出0.0
#include <stdio.h>
#include <stdarg.h>
//==============================================================================
// 格式转换函数
//==============================================================================
uint8 MyPrintf(uint8 *buf, const char *pFormat, ...)
{
	va_list	ap;
	uint8 len;
	va_start(ap, pFormat);
	len = vsprintf((char *)buf, pFormat, ap);
	va_end(ap);
	return len;
}
#endif




/********************************/
//======================================
// 功能描述: 获取字体字高
//======================================
uint8 Drv_MonLCD_GetFontY( uint8 font )
{
	return FontSizeY[font];
}

/********************************/
//======================================
// 功能描述: 初始化
//======================================
void Drv_MonLCD_Init(void)
{
	uint8 i,j;
	//======================================
#ifdef UC1701X_DRIVE
	UC1701X_LCD_Init();
	UC1701X_LCD_DISACOL(0xFF);
#elif 
#endif	//UC1701X_DRIVE
	//======================================
	//===============================================
	for(i=0;i<(MONLCD_MAX_X);i++)
		for(j=0;j<(MONLCD_MAX_Y>>3);j++)
			MONLCD_DispBuf[i][j] = 0;
	//===============================================
}

//======================================
// 功能描述: 背光灯操作(0-OFF, 1-ON)
//======================================
void Drv_MonLCD_BLSet(uint8 bhl)
{
	//======================================
#ifdef UC1701X_DRIVE
	if(bhl == DRV_MONLCD_BLON)
		UC1701X_BL_H();
	else
		UC1701X_BL_L();
#elif 
#endif	//UC1701X_DRIVE
	//======================================
}

//======================================
// 功能描述: 更新显示
//======================================
void Drv_MonLCD_DrawRefresh(void)
{
	//======================================
#ifdef UC1701X_DRIVE
	UC1701X_LCD_DispUpdate();
#elif 
#endif	//UC1701X_DRIVE
	//======================================
}

//======================================
// 功能描述: 清除显示缓存
//======================================
void Drv_MonLCD_ClearDrawBuf(uint8 mod)
{
	uint8 i,j;
	//======================================
	for(i=0;i<(MONLCD_MAX_X);i++)
		for(j=0;j<(MONLCD_MAX_Y>>3);j++)
			MONLCD_DispBuf[i][j] = mod;
	//======================================
#ifdef UC1701X_DRIVE
#elif 
#endif	//UC1701X_DRIVE
	//======================================
}

//======================================
// 功能描述: 对比度设置
//======================================
void Drv_MonLCD_ContrastSet(uint8 dat)
{
	//======================================
#ifdef UC1701X_DRIVE
	UC1701X_LCD_ContrastSet(dat);
#elif 
#endif	//UC1701X_DRIVE
	//======================================
	//======================================
}

//======================================
// 功能描述: 字符串显示
//======================================
void Drv_MonLCD_DisStr(uint8 x, uint8 y, uint8 *pstr, uint8 mod, uint8 font)
{
	//======================================
	//======================================
	while(1)
	{
		if(*pstr == 0)
			break;
		if(*pstr > 0x80)
		{//汉字
			Drv_MonLCD_DisHZ(x, y, pstr, mod, font);
			x += FontSizeXH[font];
			pstr += 2;
		}
		else
		{//ASCII
			Drv_MonLCD_DisASCII(x, y, pstr, mod, font);
			x += FontSizeXA[font];
			pstr ++;
		}
	}
	//======================================
	//======================================
}

//======================================
// 功能描述: 字符串扩展显示
//======================================
void Drv_MonLCD_DisStrExp(uint8 x, uint8 y, uint8 *pstr, uint8 mod, uint8 font, uint8 ex/*, uint8 ey*/)
{
	//======================================
	//======================================
	while(1)
	{
		if(*pstr == 0)
			break;
		if(*pstr > 0x80)
		{//汉字
			Drv_MonLCD_DisHZ(x, y, pstr, mod, font);
			x += FontSizeXH[font]+ex;
			pstr += 2;
		}
		else
		{//ASCII
			Drv_MonLCD_DisASCII(x, y, pstr, mod, font);
			x += FontSizeXA[font]+ex;
			pstr ++;
		}
	}
	//======================================
	//======================================
}

//======================================
// 功能描述: 设定指定位显示状态
//======================================
void Drv_MonLCD_DispBit(uint8 x, uint8 y, uint8 dsp)
{
	if((x >= MONLCD_MAX_X) || (y >= MONLCD_MAX_Y))
		return ;
	if(dsp)
		MONLCD_DispBuf[x][y>>3] |= (1<<(y%8));
	else
		MONLCD_DispBuf[x][y>>3] &= ~(1<<(y%8));
}

//======================================
// 功能描述: 设定指定位显示状态取反
//======================================
void Drv_MonLCD_DispNotBit(uint8 x, uint8 y)
{
	if((x >= MONLCD_MAX_X) || (y >= MONLCD_MAX_Y))
		return ;
	MONLCD_DispBuf[x][y>>3] ^= (1<<(y%8));
// 	if(MONLCD_DispBuf[x][y>>3] & (1<<(y%8)))
// 		MONLCD_DispBuf[x][y>>3] &= ~(1<<(y%8));
// 	else
// 		MONLCD_DispBuf[x][y>>3] |= (1<<(y%8));
}

//======================================
// 功能描述: 增加光标显示
//======================================
void Drv_MonLCD_DisAddFontCursor(uint8 x, uint8 y, uint8 asciinum, uint8 mod, uint8 font, uint8 ex/*, uint8 ey*/)
{
	uint16 offset;
	uint8 xx,yy;
	uint8 str;
	uint8 px,py;
	uint8 i,j;
	//======================================
	for(j=0;j<asciinum;j++)
	{
		offset = FontSizeOfLib[font]*mod;
		i = FontSizeXA[font]>>3;
		if(FontSizeXA[font] & 0x07)
			i ++;
		for(yy=0;yy<FontSizeY[font];yy++)
		{//单行
			for(xx=0;xx<FontSizeXA[font];xx++)
			{//单列
				if((xx&0x07) == 0)
					str = *(FontCursorLib[font] + offset + yy*i + (xx>>3));
				if(DRV_MONLCD_DISPROTATION_MOD == DRV_MONLCD_ROT_0)
				{//屏幕显示旋转0°
					px = x+xx;
					py = y+yy;
				}
				else if(DRV_MONLCD_DISPROTATION_MOD == DRV_MONLCD_ROT_180)
				{//屏幕显示旋转180°
					px = MONLCD_MAX_X-(x+xx)-1;
					py = MONLCD_MAX_Y-(y+yy)-1;
				}
				else if(DRV_MONLCD_DISPROTATION_MOD == DRV_MONLCD_ROT_90)
				{//屏幕显示旋转90°
					px = y+yy;
					py = MONLCD_MAX_Y-(x+xx)-1;
				}
				else if(DRV_MONLCD_DISPROTATION_MOD == DRV_MONLCD_ROT_270)
				{//屏幕显示旋转270°
					px = MONLCD_MAX_X-(y+yy)-1;
					py = x+xx;
				}
				else
					continue ;
				if(str&(0x80>>(xx&0x07)))
					Drv_MonLCD_DispNotBit(px,py);
// 					Drv_MonLCD_DispBit(px,py,1);
			}
		}
		x += FontSizeXA[font]+ex;
	}
	//======================================
	//======================================
	//======================================
}

//======================================
// 功能描述: ASCII显示
//======================================
void Drv_MonLCD_DisASCII(uint8 x, uint8 y, uint8 *pstr, uint8 mod, uint8 font)
{
	uint16 offset;
	uint8 xx,yy;
	uint8 str;
	uint8 px,py;
	uint8 i;
	//======================================
	offset=(uint16)(*pstr-32)*FontSizeOfLib[font];
	i = FontSizeXA[font]>>3;
	if(FontSizeXA[font] & 0x07)
		i ++;
	for(yy=0;yy<FontSizeY[font];yy++)
	{//单行
		for(xx=0;xx<FontSizeXA[font];xx++)
		{//单列
			if((xx&0x07) == 0)
				str = *(FontLib[font] + offset + yy*i + (xx>>3));
			if(DRV_MONLCD_DISPROTATION_MOD == DRV_MONLCD_ROT_0)
			{//屏幕显示旋转0°
				px = x+xx;
				py = y+yy;
			}
			else if(DRV_MONLCD_DISPROTATION_MOD == DRV_MONLCD_ROT_180)
			{//屏幕显示旋转180°
				px = MONLCD_MAX_X-(x+xx)-1;
				py = MONLCD_MAX_Y-(y+yy)-1;
			}
			else if(DRV_MONLCD_DISPROTATION_MOD == DRV_MONLCD_ROT_90)
			{//屏幕显示旋转90°
				px = y+yy;
				py = MONLCD_MAX_Y-(x+xx)-1;
			}
			else if(DRV_MONLCD_DISPROTATION_MOD == DRV_MONLCD_ROT_270)
			{//屏幕显示旋转270°
				px = MONLCD_MAX_X-(y+yy)-1;
				py = x+xx;
			}
			else
				continue ;
			if(mod == DRV_MONLCD_REVERSAL)
			{
				if(str&(0x80>>(xx&0x07)))
					Drv_MonLCD_DispBit(px,py,0);
				else
					Drv_MonLCD_DispBit(px,py,1);
			}
			else
			{
				if(str&(0x80>>(xx&0x07)))
					Drv_MonLCD_DispBit(px,py,1);
				else
					Drv_MonLCD_DispBit(px,py,0);
			}
		}
	}
	//======================================
	//======================================
}

//======================================
// 功能描述: 汉字字库表搜索
//======================================
uint16 Drv_MonLCD_FontHZ_TabSeartch(uint8 *pstr, uint8 font)
{
	uint16 i;
	//======================================
	for(i=0;i<FontLibHZ_Size[font];i+=(FontSizeOfLib[font]<<1)+2)
	{
		if(i >= FontLibHZ_Size[font])
			return -1;//无对应汉字
		if((FontLibHZ[font][i] == pstr[0]) && (FontLibHZ[font][i+1] == pstr[1]))
			return i;
	}
	//======================================
	return -1;
}

//======================================
// 功能描述: 汉字显示
//======================================
void Drv_MonLCD_DisHZ(uint8 x, uint8 y, uint8 *pstr, uint8 mod, uint8 font)
{
	uint16 offset;
	uint8 xx,yy;
	uint8 px,py;
	uint8 str;
	uint8 i;
	//======================================
	offset = Drv_MonLCD_FontHZ_TabSeartch(pstr, font) + 2;
	if(offset == (uint16)-1)	return ;
	i = FontSizeXH[font]>>3;
	if(FontSizeXH[font] & 0x0F)
		i ++;
	for(yy=0;yy<FontSizeY[font];yy++)
	{//单行
		for(xx=0;xx<FontSizeXH[font];xx++)
		{//单列
			if((xx&0x07) == 0)
				str = *(FontLibHZ[font] + offset + yy*i + (xx>>3));
			if(DRV_MONLCD_DISPROTATION_MOD == DRV_MONLCD_ROT_0)
			{//屏幕显示旋转0°
				px = x+xx;
				py = y+yy;
			}
			else if(DRV_MONLCD_DISPROTATION_MOD == DRV_MONLCD_ROT_180)
			{//屏幕显示旋转180°
				px = MONLCD_MAX_X-(x+xx)-1;
				py = MONLCD_MAX_Y-(y+yy)-1;
			}
			else if(DRV_MONLCD_DISPROTATION_MOD == DRV_MONLCD_ROT_90)
			{//屏幕显示旋转90°
				px = y+yy;
				py = MONLCD_MAX_Y-(x+xx)-1;
			}
			else if(DRV_MONLCD_DISPROTATION_MOD == DRV_MONLCD_ROT_270)
			{//屏幕显示旋转270°
				px = MONLCD_MAX_X-(y+yy)-1;
				py = x+xx;
			}
			else
				continue ;
			if(mod == DRV_MONLCD_REVERSAL)
			{
				if(str&(0x80>>(xx&0x07)))
					Drv_MonLCD_DispBit(px,py,0);
				else
					Drv_MonLCD_DispBit(px,py,1);
			}
			else
			{
				if(str&(0x80>>(xx&0x07)))
					Drv_MonLCD_DispBit(px,py,1);
				else
					Drv_MonLCD_DispBit(px,py,0);
			}
		}
	}
	//======================================
	//======================================
}


/********************************/
#endif	//DRV_MONLCD_USED


