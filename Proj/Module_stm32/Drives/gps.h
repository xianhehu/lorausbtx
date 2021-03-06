#ifndef __GPS_H
#define __GPS_H	 
#include "DataType.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//ATK-NEO-6M GPS模块驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/3/30
//版本：V2.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved					
//********************************************************************************
//V2.0 修改说明 20140330
//1,添加Ublox_Cfg_Cfg_Save函数
//2,添加Ublox_Cfg_Msg函数
//3,添加Ublox_Cfg_Prt函数.				  
////////////////////////////////////////////////////////////////////////////////// 	   
 									   						    
//GPS NMEA-0183协议重要参数结构体定义 
//卫星信息
#pragma pack (1) /*指定按1字节对齐*/
// __packed typedef struct  
typedef struct  
{										    
 	uint8 num;		//卫星编号
	uint8 eledeg;	//卫星仰角
	uint16 azideg;	//卫星方位角
	uint8 sn;		//信噪比		   
}nmea_slmsg;  
//UTC时间信息
// __packed typedef struct  
typedef struct  
{										    
 	uint16 year;	//年份
	uint8 month;	//月份
	uint8 date;	//日期
	uint8 hour; 	//小时
	uint8 min; 	//分钟
	uint8 sec; 	//秒钟
}nmea_utc_time;   	   
//NMEA 0183 协议解析后数据存放结构体
// __packed typedef struct  
typedef struct  
{										    
 	uint8 svnum;					//可见卫星数
	nmea_slmsg slmsg[12];		//最多12颗卫星
	nmea_utc_time utc;			//UTC时间
	uint32 latitude;				//纬度 分扩大100000倍,实际要除以100000
	uint8 nshemi;					//北纬/南纬,N:北纬;S:南纬				  
	uint32 longitude;			    //经度 分扩大100000倍,实际要除以100000
	uint8 ewhemi;					//东经/西经,E:东经;W:西经
	uint8 gpssta;					//GPS状态:0,未定位;1,非差分定位;2,差分定位;6,正在估算.				  
 	uint8 posslnum;				//用于定位的卫星数,0~12.
 	uint8 possl[12];				//用于定位的卫星编号
	uint8 fixmode;					//定位类型:1,没有定位;2,2D定位;3,3D定位
	uint16 pdop;					//位置精度因子 0~500,对应实际值0~50.0
	uint16 hdop;					//水平精度因子 0~500,对应实际值0~50.0
	uint16 vdop;					//垂直精度因子 0~500,对应实际值0~50.0 

	int altitude;			 	//海拔高度,放大了10倍,实际除以10.单位:0.1m	 
	uint16 speed;					//地面速率,放大了1000倍,实际除以10.单位:0.001公里/小时	 
}nmea_msg; 
//////////////////////////////////////////////////////////////////////////////////////////////////// 	
//UBLOX NEO-6M 配置(清除,保存,加载等)结构体
// __packed typedef struct  
typedef struct  
{										    
 	uint16 header;					//cfg header,固定为0X62B5(小端模式)
	uint16 id;						//CFG CFG ID:0X0906 (小端模式)
	uint16 dlength;				//数据长度 12/13
	uint32 clearmask;				//子区域清除掩码(1有效)
	uint32 savemask;				//子区域保存掩码
	uint32 loadmask;				//子区域加载掩码
	uint8  devicemask; 		  	//目标器件选择掩码	b0:BK RAM;b1:FLASH;b2,EEPROM;b4,SPI FLASH
	uint8  cka;		 			//校验CK_A 							 	 
	uint8  ckb;			 		//校验CK_B							 	 
}_ublox_cfg_cfg; 

//UBLOX NEO-6M 消息设置结构体
// __packed typedef struct  
typedef struct  
{										    
 	uint16 header;					//cfg header,固定为0X62B5(小端模式)
	uint16 id;						//CFG MSG ID:0X0106 (小端模式)
	uint16 dlength;				//数据长度 8
	uint8  msgclass;				//消息类型(F0 代表NMEA消息格式)
	uint8  msgid;					//消息 ID 
								//00,GPGGA;01,GPGLL;02,GPGSA;
								//03,GPGSV;04,GPRMC;05,GPVTG;
								//06,GPGRS;07,GPGST;08,GPZDA;
								//09,GPGBS;0A,GPDTM;0D,GPGNS;
	uint8  iicset;					//IIC消输出设置    0,关闭;1,使能.
	uint8  uart1set;				//UART1输出设置	   0,关闭;1,使能.
	uint8  uart2set;				//UART2输出设置	   0,关闭;1,使能.
	uint8  usbset;					//USB输出设置	   0,关闭;1,使能.
	uint8  spiset;					//SPI输出设置	   0,关闭;1,使能.
	uint8  ncset;					//未知输出设置	   默认为1即可.
 	uint8  cka;			 		//校验CK_A 							 	 
	uint8  ckb;			    	//校验CK_B							 	 
}_ublox_cfg_msg; 

//UBLOX NEO-6M UART端口设置结构体
// __packed typedef struct  
typedef struct  
{										    
 	uint16 header;					//cfg header,固定为0X62B5(小端模式)
	uint16 id;						//CFG PRT ID:0X0006 (小端模式)
	uint16 dlength;				//数据长度 20
	uint8  portid;					//端口号,0=IIC;1=UART1;2=UART2;3=USB;4=SPI;
	uint8  reserved;				//保留,设置为0
	uint16 txready;				//TX Ready引脚设置,默认为0
	uint32 mode;					//串口工作模式设置,奇偶校验,停止位,字节长度等的设置.
 	uint32 baudrate;				//波特率设置
 	uint16 inprotomask;		 	//输入协议激活屏蔽位  默认设置为0X07 0X00即可.
 	uint16 outprotomask;		 	//输出协议激活屏蔽位  默认设置为0X07 0X00即可.
 	uint16 reserved4; 				//保留,设置为0
 	uint16 reserved5; 				//保留,设置为0 
 	uint8  cka;			 		//校验CK_A 							 	 
	uint8  ckb;			    	//校验CK_B							 	 
}_ublox_cfg_prt; 

//UBLOX NEO-6M 时钟脉冲配置结构体
// __packed typedef struct  
typedef struct  
{										    
 	uint16 header;					//cfg header,固定为0X62B5(小端模式)
	uint16 id;						//CFG TP ID:0X0706 (小端模式)
	uint16 dlength;				//数据长度
	uint32 interval;				//时钟脉冲间隔,单位为us
	uint32 length;				 	//脉冲宽度,单位为us
	signed char status;			//时钟脉冲配置:1,高电平有效;0,关闭;-1,低电平有效.			  
	uint8 timeref;			   		//参考时间:0,UTC时间;1,GPS时间;2,当地时间.
	uint8 flags;					//时间脉冲设置标志
	uint8 reserved;				//保留			  
 	signed short antdelay;	 	//天线延时
 	signed short rfdelay;		//RF延时
	signed int userdelay; 	 	//用户延时	
	uint8 cka;						//校验CK_A 							 	 
	uint8 ckb;						//校验CK_B							 	 
}_ublox_cfg_tp; 

//UBLOX NEO-6M 刷新速率配置结构体
// __packed typedef struct  
typedef struct  
{										    
 	uint16 header;					//cfg header,固定为0X62B5(小端模式)
	uint16 id;						//CFG RATE ID:0X0806 (小端模式)
	uint16 dlength;				//数据长度
	uint16 measrate;				//测量时间间隔，单位为ms，最少不能小于200ms（5Hz）
	uint16 navrate;				//导航速率（周期），固定为1
	uint16 timeref;				//参考时间：0=UTC Time；1=GPS Time；
 	uint8  cka;					//校验CK_A 							 	 
	uint8  ckb;					//校验CK_B							 	 
}_ublox_cfg_rate; 
#pragma pack () /*取消指定对齐，恢复缺省对齐*/
//======================================
extern nmea_msg gpsx;	//GPS信息

//======================================
int NMEA_Str2num(uint8 *buf,uint8*dx);
void GPS_Analysis(nmea_msg *gpsx,uint8 *buf);
void NMEA_GPGSV_Analysis(nmea_msg *gpsx,uint8 *buf);
void NMEA_GPGGA_Analysis(nmea_msg *gpsx,uint8 *buf);
void NMEA_GPGSA_Analysis(nmea_msg *gpsx,uint8 *buf);
void NMEA_GPGSA_Analysis(nmea_msg *gpsx,uint8 *buf);
void NMEA_GPRMC_Analysis(nmea_msg *gpsx,uint8 *buf);
void NMEA_GPVTG_Analysis(nmea_msg *gpsx,uint8 *buf);
// uint8 Ublox_Cfg_Cfg_Save(void);
// uint8 Ublox_Cfg_Msg(uint8 msgid,uint8 uart1set);
// uint8 Ublox_Cfg_Prt(uint32 baudrate);
// uint8 Ublox_Cfg_Tp(uint32 interval,uint32 length,signed char status);
// uint8 Ublox_Cfg_Rate(uint16 measrate,uint8 reftime);


//======================================
extern void GPS_RxdByte(uint8 bt);	//接收一个字节

//======================================
#endif  





