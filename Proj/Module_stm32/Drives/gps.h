#ifndef __GPS_H
#define __GPS_H	 
#include "DataType.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//ATK-NEO-6M GPSģ����������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/3/30
//�汾��V2.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved					
//********************************************************************************
//V2.0 �޸�˵�� 20140330
//1,���Ublox_Cfg_Cfg_Save����
//2,���Ublox_Cfg_Msg����
//3,���Ublox_Cfg_Prt����.				  
////////////////////////////////////////////////////////////////////////////////// 	   
 									   						    
//GPS NMEA-0183Э����Ҫ�����ṹ�嶨�� 
//������Ϣ
#pragma pack (1) /*ָ����1�ֽڶ���*/
// __packed typedef struct  
typedef struct  
{										    
 	uint8 num;		//���Ǳ��
	uint8 eledeg;	//��������
	uint16 azideg;	//���Ƿ�λ��
	uint8 sn;		//�����		   
}nmea_slmsg;  
//UTCʱ����Ϣ
// __packed typedef struct  
typedef struct  
{										    
 	uint16 year;	//���
	uint8 month;	//�·�
	uint8 date;	//����
	uint8 hour; 	//Сʱ
	uint8 min; 	//����
	uint8 sec; 	//����
}nmea_utc_time;   	   
//NMEA 0183 Э����������ݴ�Žṹ��
// __packed typedef struct  
typedef struct  
{										    
 	uint8 svnum;					//�ɼ�������
	nmea_slmsg slmsg[12];		//���12������
	nmea_utc_time utc;			//UTCʱ��
	uint32 latitude;				//γ�� ������100000��,ʵ��Ҫ����100000
	uint8 nshemi;					//��γ/��γ,N:��γ;S:��γ				  
	uint32 longitude;			    //���� ������100000��,ʵ��Ҫ����100000
	uint8 ewhemi;					//����/����,E:����;W:����
	uint8 gpssta;					//GPS״̬:0,δ��λ;1,�ǲ�ֶ�λ;2,��ֶ�λ;6,���ڹ���.				  
 	uint8 posslnum;				//���ڶ�λ��������,0~12.
 	uint8 possl[12];				//���ڶ�λ�����Ǳ��
	uint8 fixmode;					//��λ����:1,û�ж�λ;2,2D��λ;3,3D��λ
	uint16 pdop;					//λ�þ������� 0~500,��Ӧʵ��ֵ0~50.0
	uint16 hdop;					//ˮƽ�������� 0~500,��Ӧʵ��ֵ0~50.0
	uint16 vdop;					//��ֱ�������� 0~500,��Ӧʵ��ֵ0~50.0 

	int altitude;			 	//���θ߶�,�Ŵ���10��,ʵ�ʳ���10.��λ:0.1m	 
	uint16 speed;					//��������,�Ŵ���1000��,ʵ�ʳ���10.��λ:0.001����/Сʱ	 
}nmea_msg; 
//////////////////////////////////////////////////////////////////////////////////////////////////// 	
//UBLOX NEO-6M ����(���,����,���ص�)�ṹ��
// __packed typedef struct  
typedef struct  
{										    
 	uint16 header;					//cfg header,�̶�Ϊ0X62B5(С��ģʽ)
	uint16 id;						//CFG CFG ID:0X0906 (С��ģʽ)
	uint16 dlength;				//���ݳ��� 12/13
	uint32 clearmask;				//�������������(1��Ч)
	uint32 savemask;				//�����򱣴�����
	uint32 loadmask;				//�������������
	uint8  devicemask; 		  	//Ŀ������ѡ������	b0:BK RAM;b1:FLASH;b2,EEPROM;b4,SPI FLASH
	uint8  cka;		 			//У��CK_A 							 	 
	uint8  ckb;			 		//У��CK_B							 	 
}_ublox_cfg_cfg; 

//UBLOX NEO-6M ��Ϣ���ýṹ��
// __packed typedef struct  
typedef struct  
{										    
 	uint16 header;					//cfg header,�̶�Ϊ0X62B5(С��ģʽ)
	uint16 id;						//CFG MSG ID:0X0106 (С��ģʽ)
	uint16 dlength;				//���ݳ��� 8
	uint8  msgclass;				//��Ϣ����(F0 ����NMEA��Ϣ��ʽ)
	uint8  msgid;					//��Ϣ ID 
								//00,GPGGA;01,GPGLL;02,GPGSA;
								//03,GPGSV;04,GPRMC;05,GPVTG;
								//06,GPGRS;07,GPGST;08,GPZDA;
								//09,GPGBS;0A,GPDTM;0D,GPGNS;
	uint8  iicset;					//IIC���������    0,�ر�;1,ʹ��.
	uint8  uart1set;				//UART1�������	   0,�ر�;1,ʹ��.
	uint8  uart2set;				//UART2�������	   0,�ر�;1,ʹ��.
	uint8  usbset;					//USB�������	   0,�ر�;1,ʹ��.
	uint8  spiset;					//SPI�������	   0,�ر�;1,ʹ��.
	uint8  ncset;					//δ֪�������	   Ĭ��Ϊ1����.
 	uint8  cka;			 		//У��CK_A 							 	 
	uint8  ckb;			    	//У��CK_B							 	 
}_ublox_cfg_msg; 

//UBLOX NEO-6M UART�˿����ýṹ��
// __packed typedef struct  
typedef struct  
{										    
 	uint16 header;					//cfg header,�̶�Ϊ0X62B5(С��ģʽ)
	uint16 id;						//CFG PRT ID:0X0006 (С��ģʽ)
	uint16 dlength;				//���ݳ��� 20
	uint8  portid;					//�˿ں�,0=IIC;1=UART1;2=UART2;3=USB;4=SPI;
	uint8  reserved;				//����,����Ϊ0
	uint16 txready;				//TX Ready��������,Ĭ��Ϊ0
	uint32 mode;					//���ڹ���ģʽ����,��żУ��,ֹͣλ,�ֽڳ��ȵȵ�����.
 	uint32 baudrate;				//����������
 	uint16 inprotomask;		 	//����Э�鼤������λ  Ĭ������Ϊ0X07 0X00����.
 	uint16 outprotomask;		 	//���Э�鼤������λ  Ĭ������Ϊ0X07 0X00����.
 	uint16 reserved4; 				//����,����Ϊ0
 	uint16 reserved5; 				//����,����Ϊ0 
 	uint8  cka;			 		//У��CK_A 							 	 
	uint8  ckb;			    	//У��CK_B							 	 
}_ublox_cfg_prt; 

//UBLOX NEO-6M ʱ���������ýṹ��
// __packed typedef struct  
typedef struct  
{										    
 	uint16 header;					//cfg header,�̶�Ϊ0X62B5(С��ģʽ)
	uint16 id;						//CFG TP ID:0X0706 (С��ģʽ)
	uint16 dlength;				//���ݳ���
	uint32 interval;				//ʱ��������,��λΪus
	uint32 length;				 	//������,��λΪus
	signed char status;			//ʱ����������:1,�ߵ�ƽ��Ч;0,�ر�;-1,�͵�ƽ��Ч.			  
	uint8 timeref;			   		//�ο�ʱ��:0,UTCʱ��;1,GPSʱ��;2,����ʱ��.
	uint8 flags;					//ʱ���������ñ�־
	uint8 reserved;				//����			  
 	signed short antdelay;	 	//������ʱ
 	signed short rfdelay;		//RF��ʱ
	signed int userdelay; 	 	//�û���ʱ	
	uint8 cka;						//У��CK_A 							 	 
	uint8 ckb;						//У��CK_B							 	 
}_ublox_cfg_tp; 

//UBLOX NEO-6M ˢ���������ýṹ��
// __packed typedef struct  
typedef struct  
{										    
 	uint16 header;					//cfg header,�̶�Ϊ0X62B5(С��ģʽ)
	uint16 id;						//CFG RATE ID:0X0806 (С��ģʽ)
	uint16 dlength;				//���ݳ���
	uint16 measrate;				//����ʱ��������λΪms�����ٲ���С��200ms��5Hz��
	uint16 navrate;				//�������ʣ����ڣ����̶�Ϊ1
	uint16 timeref;				//�ο�ʱ�䣺0=UTC Time��1=GPS Time��
 	uint8  cka;					//У��CK_A 							 	 
	uint8  ckb;					//У��CK_B							 	 
}_ublox_cfg_rate; 
#pragma pack () /*ȡ��ָ�����룬�ָ�ȱʡ����*/
//======================================
extern nmea_msg gpsx;	//GPS��Ϣ

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
extern void GPS_RxdByte(uint8 bt);	//����һ���ֽ�

//======================================
#endif  





