/*======================================
// Q_Shell_User.c文件
// 用户Q_Shell命令操作处理
//======================================
//======================================
======================================*/
#include "Q_Shell_User.h"
/********************************/
#include "Q_Shell.h"
/********************************/
#include "Drv_SX127x.h"
#include <stdlib.h>


/********************************/



/********************************/
STR_QSHELL_COMMAND strQShellConnmand;


/********************************/
//======================================
// Q_Shell command : h
//======================================
void h(void)
{
	//======================================
#if 0
	printf("\r\n-_-_-_-_-_-_-_- Q_Shell help info -_-_-_-_-_-_-_-_\r\n");
	printf("lf()                       -- list registered fun\r\n");
	printf("-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_\r\n");
#endif
	printf("\r\n-------------------- help info -------------------\r\n");
	printf("csend(length)              ------ Continue send out radon value frame(data length == length in byte)\r\n");
	printf("ed(tim)                    ------ Energy detection\r\n");
	printf("h()                        ------ All the commands of this demo\r\n");
	printf("info()                     ------ Print out radio channel's info\r\n");
	printf("listen()                   ------ Enable readio channel to listen data frame\r\n");
	printf("reset()                    ------ Reset radio channel\r\n");
	printf("send(length)               ------ Send out one radon value frame(data length == length in byte)\r\n");
	printf("setbandwidth(Hz)           ------ Setup new bandwidth\r\n");
	printf("setbitrate(bitrate)        ------ Set bitrate (LoRa: 7, SF7 ~ 12, SF12; FSK: bps)\r\n");
// 	printf("setcrc(isCRC)              ------ Setup CRC check of this radio channel(isCRC: 0, disable; 1, enable)\r\n");
	printf("setfreq(freq)              ------ Setup new freq of this radio channel\r\n");
	printf("setmode(modulation)        ------ Setup modulation (LoRa: 0, FSK: 1)\r\n");
	printf("setpreamble(length)        ------ Set preamble length\r\n");
	printf("setpwr(power)              ------ Set Tx power (0~20)\r\n");
	printf("setuser(user)              ------ Set the user of radio PHY: 0, CLI; 1, MAC\r\n");
	printf("stop()                     ------ Stop csend command and set radio channel in stop state\r\n");
	printf("ept()                      ------ Enter Pass-through mode\r\n");

	printf("--------------------------------------------------\r\n");
	//======================================
// 	printf("()				------ \r\n");
	//======================================
}

//======================================
// Q_Shell command : 
//======================================
void info( void )
{
	// --- Print out the info of rado channel ---
	printf("\r\n");
	printf("--------------- radio channel info ---------------\r\n"); 
	printf("modulation: %d (0, LoRa; 1, 2FSK; 2, 4FSK)\r\n", strSX127x[SX127x_CHIP1].radio.modulation);
	printf("freq: %dHz\r\n", strSX127x[SX127x_CHIP1].radio.freq);  
	printf("freqBand: %d (2, 470; 14, 433)\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.freqBand);  
	printf("state: %d (0, off; 1, idle; 2, setting; 3, Tx; 4, Rx; 5, listening; 6, error)\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.state);
	printf("user: %d (0, CLI; 1, MAC)\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.user);
	printf("preambleLen: %d\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.preambleLen);
	printf("isCRC: %d(0, CRC off; 1, on)\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.isCRC);
	printf("duplex: %d (0, half duplex; 1, full duplex)\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.duplex);
	printf("isPend: %d (0, NonPendable; 1, Pendable)\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.isPend);
	printf("led: %d (0, unuse; 1, use)\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.led);
	printf("RSSI: %d \r\n", strSX127x[SX127x_CHIP1].radio.RSSI);
	printf("SNR: %d \r\n", strSX127x[SX127x_CHIP1].radio.SNR);
	printf("chip_descrip: %s\r\n", strSX127x[SX127x_CHIP1].radio.chip_descrip);
	if (strSX127x[SX127x_CHIP1].radio.modulation == RADIO_MTYPE_LORA)
	{ // LoRa
		printf("bitrate(SF): %d\r\n", strSX127x[SX127x_CHIP1].flag.bits.sf);
		printf("coderate: %d (1, 4/5; 2, 4/6; 3, 4/7; 4, 4/8)\r\n", strSX127x[SX127x_CHIP1].flag.bits.coderate);
	}
	else
	{
		printf("bitrate: %dbps\r\n", strSX127x[SX127x_CHIP1].radio.bitrate);
	}
	printf("bandwidth: %dHz\r\n", strSX127x[SX127x_CHIP1].radio.bandwidth);
	printf("send_power: %ddBm\r\n", strSX127x[SX127x_CHIP1].radio.send_power);
	// -----------------------------------------  
}

//======================================
// Q_Shell command : 
//======================================
void listen( void )
{
	uint8 dataBuff[SX127X_BUFF_LEN];
	uint16 len;
	uint16 i;
	// --- radio listen for a while ---
	if(Drv_SX127x_Radio_Listen(SX127x_CHIP1, 10000, true) == 1) // Start to listen
	{
		len = Drv_SX127x_Radio_Read(SX127x_CHIP1, dataBuff, SX127X_BUFF_LEN);
		if ((len <= SX127X_BUFF_LEN) && (len < SX127X_BUFF_LEN))
		{
			printf("Worker receive one frame (len = %d, RSSI: %d, SNR: %d): ", len, strSX127x[SX127x_CHIP1].radio.RSSI, strSX127x[SX127x_CHIP1].radio.SNR);
			for (i = 0; i < len; i++)
			{
				printf("%d  ", dataBuff[i]);
			}
			printf("\r\n");
		}
		else if (len > SX127X_BUFF_LEN)
		{
			printf("Error: len(%d) not right!\r\n", len);
			Drv_SX127x_Radio_Control(SX127x_CHIP1, RADIO_CONTROL_OPT_RESET);
			Drv_SX127x_Radio_Control(SX127x_CHIP1, RADIO_CONTROL_OPT_SET_PARAM);
		} 
		else // len == 0
		{
			printf("Cannot receive any data frame!\r\n");
		}
	}
	Drv_SX127x_Radio_Control(SX127x_CHIP1, RADIO_CONTROL_OPT_OFF);
}

//======================================
// Q_Shell command : 
//======================================
void reset( void )
{
	// ------------------------- 
	// Reset radio channel 
	Drv_SX127x_Radio_Control(SX127x_CHIP1, RADIO_CONTROL_OPT_RESET);
	Drv_SX127x_Radio_Control(SX127x_CHIP1, RADIO_CONTROL_OPT_SET_PARAM);
	printf("Reset radio channel finished!\r\n");
}

//======================================
// Q_Shell command : 能量侦听
//======================================
void ed( uint32 tim )// Period
{
	printf("\r\n");
	printf("------ radio channel setup energy detection ------\r\n");
	if ((tim < RADIO_ED_PERIOD_MIN) || (tim > RADIO_ED_PERIOD_MAX))
	{
		printf("CliRadio_ed: not supported ed period (%d, must be %dus ~ %dus)!\r\n", tim, RADIO_ED_PERIOD_MIN, RADIO_ED_PERIOD_MAX);
		return  ;     
	}
	if (strSX127x[SX127x_CHIP1].radio.flag.bits.user != RADIO_USER_CLI)
	{
		printf("CliRadio_ed: This radio channel cannot be used by CLI (%d)!\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.user);
		return  ;    
	}
	Drv_SX127x_Radio_Listen(SX127x_CHIP1, tim, false); // Enable radio listening for energy detection
	printf("Enable the energy detection of radio channel in %dms!\r\n", tim);  
}

//======================================
// Q_Shell command : 
//======================================
void ept( void )
{
	printf("\r\n");
	printf("------ Enter Pass-through mode ------\r\n");
	if (strSX127x[SX127x_CHIP1].radio.flag.bits.user != RADIO_USER_CLI)
	{
		printf("CliRadio_send: This radio channel cannot be used by CLI (%d)!\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.user);
		return  ;    
	}
	strQShellConnmand.flag.bits.epten = 1;
	printf("Entered Pass-through mode!\r\n");
}

//======================================
// Q_Shell command : 
//======================================
void send(uint8 len)
{
	printf("\r\n");
	printf("------ radio channel sendout one frame (random value) ------\r\n");
	// --- Check input param ---
	if ((len < 1) || (len > SX127X_BUFF_LEN))
	{
		printf("CliRadio_send: Not supported data length (%d, must be 1 ~ %d)!\r\n", len, SX127X_BUFF_LEN);
		return  ;    
	}  
	if (strSX127x[SX127x_CHIP1].radio.flag.bits.user != RADIO_USER_CLI)
	{
		printf("CliRadio_send: This radio channel cannot be used by CLI (%d)!\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.user);
		return  ;    
	}
	if ((strSX127x[SX127x_CHIP1].radio.flag.bits.state != RADIO_STATE_IDLE) && (strSX127x[SX127x_CHIP1].radio.flag.bits.state != RADIO_STATE_OFF))
	{
		printf("CliRadio_send: Cannot send out data frame while this channel is in busy state (%d)!\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.state);
		return  ;    
	}   
	// -------------------------
	uint16 i;
	for (i = 0; i < len; i++)
	{
		strSX127x[SX127x_CHIP1].buff[i] = (rand() & 0xFF); // Create random data frame
	}
	strSX127x[SX127x_CHIP1].flag.bits.availBuffLen = len;
	printf("Channel %d prepare to send one frame (%d): ", strSX127x[SX127x_CHIP1].radio.channel_id, len);
	for (i = 0; i < len; i++)
	{
		printf("%.2X ", strSX127x[SX127x_CHIP1].buff[i]);
	}
	printf("\r\n");
// 	printf("Start OSTick: (%d ms)\r\n", OSTimeGet());
	// -------------
	Drv_SX127x_Radio_Write( SX127x_CHIP1, strSX127x[SX127x_CHIP1].buff, len);	// Write data buffer
	Drv_SX127x_Radio_Send( SX127x_CHIP1, 6000);	// Send out data
// 	printf("Finish OSTick: (%d ms)\r\n", OSTimeGet());
}

//======================================
// Q_Shell command : 连续发送测试
//======================================
void csend( uint8 len )
{
	printf("\r\n");
	printf("------ radio channel continue send/receive frame (random value) test ------\r\n");
	// --- Check input param ---
	if ((len < 1) || (len > SX127X_BUFF_LEN))
	{
		printf("CliRadio_send: Not supported data length (%d, must be 1 ~ %d)!\r\n", len, SX127X_BUFF_LEN);
		return  ;    
	}  
	if (strSX127x[SX127x_CHIP1].radio.flag.bits.user != RADIO_USER_CLI)
	{
		printf("CliRadio_send: This radio channel cannot be used by CLI (%d)!\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.user);
		return  ;    
	}
	if ((strSX127x[SX127x_CHIP1].radio.flag.bits.state != RADIO_STATE_IDLE) && (strSX127x[SX127x_CHIP1].radio.flag.bits.state != RADIO_STATE_OFF))
	{
		printf("CliRadio_send: Cannot send out data frame while this channel is in busy state (%d)!\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.state);
		return  ;    
	}   
	// -------------------------
	strQShellConnmand.flag.bits.len = len;
	strQShellConnmand.cnt = 0;
	strQShellConnmand.cerr = 0;
	strQShellConnmand.flag.bits.csenden = 1;
	// -------------------------
}

//======================================
// Q_Shell command : 连续发送测试
//======================================
uint8 QShell_csend( void )
{
	// -------------------------
	uint8 buf[SX127X_BUFF_LEN];
	uint8 len;
	uint8 i;
	uint8 err = 1;
	// -------------------------
	if(strQShellConnmand.flag.bits.csenden && strQShellConnmand.flag.bits.len)
	{
		len = strQShellConnmand.flag.bits.len;
		for (i = 0; i < len; i++)
		{
			strSX127x[SX127x_CHIP1].buff[i] = (rand() & 0xFF); // Create random data frame
		}
		strSX127x[SX127x_CHIP1].flag.bits.availBuffLen = len;
		printf("Channel %d prepare to send one frame (%d): ", strSX127x[SX127x_CHIP1].radio.channel_id, len);
		for (i = 0; i < len; i++)
		{
			printf("%.2X ", strSX127x[SX127x_CHIP1].buff[i]);
		}
		printf("\r\n");
		// -------------
		Drv_SX127x_Radio_Write( SX127x_CHIP1, strSX127x[SX127x_CHIP1].buff, len);	// Write data buffer
		Drv_SX127x_Radio_Send( SX127x_CHIP1, 2000);	// Send out data
		// -------------
// 		printf("frame send finished (%d)!\r\n", len);
		// -------------
		if(Drv_SX127x_Radio_Listen(SX127x_CHIP1, 5000, true) == 1)
		{
			i = Drv_SX127x_Radio_Read(SX127x_CHIP1, buf, SX127X_BUFF_LEN);
			if(i <= SX127X_BUFF_LEN)
			{
				if(i != len)
				{
					printf("received frame length error (S:%d/R:%d)!", len, i);
					len = i;
				}
				// -------------
				//数据对比
				for (i = 0; i < len; i++)
				{
					if(strSX127x[SX127x_CHIP1].buff[i] != buf[i])
					{
						strQShellConnmand.cerr ++;
						printf("\treceived frame (%d/%d): ", len, i);
						for (i = 0; i < len; i++)
						{
							printf("%.2X ", buf[i]);
						}
						printf("\r\n");
						Drv_SX127x_WAIT_MS(1000);	//遇到错误延时1S
						break;
					}
				}
				if(i >= len)
					err = 0;
			}
			else
			{
				strQShellConnmand.cerr ++;
				printf("Error: no frame received[%d]!\r\n", i);
			}
		}
		else
		{
			strQShellConnmand.cerr ++;
			printf("Error: no frame received!\r\n");
		}
		// -------------
		strQShellConnmand.cnt ++;
		printf("frame send/receive finished %d times,error %d times:\r\n", strQShellConnmand.cnt, strQShellConnmand.cerr);
		// -------------
	}
	return err;
}

//======================================
// Q_Shell command : 
//======================================
void setbandwidth( uint32 freq )
{
	printf("\r\n");
	printf("------ radio channel setup bandwidth ------\r\n");
	// ------------------------- 
	// bandwidth
	if ((freq < RADIO_BANDWIDTH_MIN) || (freq > RADIO_BANDWIDTH_MAX))
	{
		printf("CliRadio_setbandwidth: Not supported freq bandwidth (%dHz)!\r\n", freq);
		return  ;    
	}
	if (strSX127x[SX127x_CHIP1].radio.modulation == RADIO_MTYPE_LORA)
	{ // LoRa
		if ((freq != 125000) && (freq != 62500))
		{
			printf("CliRadio_setbandwidth: Only support 125kHz or 62.5kHz for LoRa mode (%dHz)!\r\n", freq);
			return  ;      
		}
	}  
	if (strSX127x[SX127x_CHIP1].radio.flag.bits.user != RADIO_USER_CLI)
	{
		printf("CliRadio_setbandwidth: This radio channel cannot be used by CLI (%d)!\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.user);
		return  ;    
	}
	if ((strSX127x[SX127x_CHIP1].radio.flag.bits.state != RADIO_STATE_IDLE) && (strSX127x[SX127x_CHIP1].radio.flag.bits.state != RADIO_STATE_OFF))
	{
		printf("CliRadio_setbandwidth: Cannot send out data frame while this channel is in busy state(%d)!\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.state);
		return  ;    
	}
	if (freq == strSX127x[SX127x_CHIP1].radio.bandwidth)
	{
		printf("CliRadio_setbandwidth: This radio channel has the same bandwidth (%dHz)!\r\n", strSX127x[SX127x_CHIP1].radio.bandwidth);
		return  ;    
	}
	// Setup new bandwidth
	strSX127x[SX127x_CHIP1].radio.bandwidth = freq;
	Drv_SX127x_Radio_Control(SX127x_CHIP1, RADIO_CONTROL_OPT_SET_BANDW);
	printf("CliRadio_setbandwidth: Start to setup the bandwidth (%dHz) of radio channel!\r\n", freq);  
}

//======================================
// Q_Shell command : 
//======================================
void setbitrate( uint32 bitrate )
{
	printf("\r\n");
	printf("------ radio channel setup bitrate ------\r\n");
	if (strSX127x[SX127x_CHIP1].radio.modulation == RADIO_MTYPE_LORA)
	{ // LoRa
		if ((bitrate < LORA_PARAM_SF_7) || (bitrate > LORA_PARAM_SF_12))
		{
			printf("CliRadio_setbitrate: Not supported bitrat (%d) in LoRa mode!\r\n", bitrate);
			return  ;      
		}
		if (bitrate == strSX127x[SX127x_CHIP1].flag.bits.sf)
		{
			printf("CliRadio_setbitrate: This radio channel has the same bitrate (%d) in LoRa mode!\r\n", bitrate);
			return  ;         
		}
	}
	else
	{ // FSK
		if ((bitrate < RADIO_BITRATE_MIN) || (bitrate > RADIO_BITRATE_MAX))
		{
			printf("CliRadio_setbitrate: Not supported bitrate (%d) in FSK mode!\r\n", bitrate);
			return  ;        
		}
		if (bitrate == strSX127x[SX127x_CHIP1].radio.bitrate)
		{
			printf("CliRadio_setbitrate: This radio channel has the same bitrate (%d) in FSK mode!\r\n", bitrate);
			return  ;       
		}
	}
	if (strSX127x[SX127x_CHIP1].radio.flag.bits.user != RADIO_USER_CLI)
	{
		printf("CliRadio_setbitrate: This radio channel cannot be used by CLI (%d)!\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.user);
		return  ;    
	}
	if ((strSX127x[SX127x_CHIP1].radio.flag.bits.state != RADIO_STATE_IDLE) && (strSX127x[SX127x_CHIP1].radio.flag.bits.state != RADIO_STATE_OFF))
	{
		printf("CliRadio_setbitrate: Cannot send out data frame while this channel is in busy state(%d)!\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.state);
		return  ;    
	}
	// --- Setup new bitrate ---
	if (strSX127x[SX127x_CHIP1].radio.modulation == RADIO_MTYPE_LORA)
	{ // LoRa mode
		strSX127x[SX127x_CHIP1].flag.bits.sf = bitrate;
	}
	else
	{ // FSK mode
		strSX127x[SX127x_CHIP1].radio.bitrate = bitrate;
	}
	Drv_SX127x_Radio_Control(SX127x_CHIP1, RADIO_CONTROL_OPT_SET_BITRATE);
	printf("Setup bitrate (%d) for radio channel!\r\n", bitrate);
}

//======================================
// Q_Shell command : 
//======================================
// void setcrc(uint8 isCRC)
// {
// }

//======================================
// Q_Shell command : 
//======================================
void setfreq(uint32 freq)
{
	printf("\r\n");
	printf("------ radio channel setup freq ------\r\n"); 
	if ((freq < RADIO_FREQ_MIN) || (freq > RADIO_FREQ_MAX))
	{
		printf("CliRadio_setfreq: Not supported freq (%d)!\r\n", freq);
		return  ;    
	}      
	if (Drv_SX127x_Radio_CheckFreqBand(strSX127x[SX127x_CHIP1].radio.flag.bits.freqBand, freq) < 0)
	{
		printf("CliRadio_setfreq: Freq(%dHz) not in scale (%d)!\r\n", freq, strSX127x[SX127x_CHIP1].radio.flag.bits.freqBand);
		return  ;    
	}
	if (strSX127x[SX127x_CHIP1].radio.flag.bits.user != RADIO_USER_CLI)
	{
		printf("CliRadio_setfreq: This radio channel cannot be used by CLI (%d)!\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.user);
		return  ;    
	}
	if ((strSX127x[SX127x_CHIP1].radio.flag.bits.state != RADIO_STATE_IDLE) && (strSX127x[SX127x_CHIP1].radio.flag.bits.state != RADIO_STATE_OFF))
	{
		printf("CliRadio_setfreq: Cannot send out data frame while this channel is in busy state (%d)!\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.state);
		return  ;    
	}
	if (freq == strSX127x[SX127x_CHIP1].radio.freq)
	{
		printf("CliRadio_setfreq: This radio channel has the same freq (%dHz)!\r\n", strSX127x[SX127x_CHIP1].radio.freq);
		return  ;    
	}
	// Set new freq
	strSX127x[SX127x_CHIP1].radio.freq = freq; 
	Drv_SX127x_Radio_Control(SX127x_CHIP1, RADIO_CONTROL_OPT_SET_FREQ);
	printf("Setup the freq of radio channel to %dHz!\r\n", freq);  
}

//======================================
// Q_Shell command : 
//======================================
void setmode( uint8 modulation )
{
	printf("\r\n");
	printf("------ radio channel setup modulation ------\r\n");
	if (modulation > RADIO_MTYPE_LAST)
	{
		printf("CliRadio_setmode: Not supported modulation type (%d)!\r\n", modulation);
		return  ;
	}    
	if (strSX127x[SX127x_CHIP1].radio.modulation == modulation)
	{
		printf("CliRadio_setmode: This channel is in the same modulation type (%d)!\r\n", modulation);
		return  ;
	}
	if (strSX127x[SX127x_CHIP1].radio.flag.bits.user != RADIO_USER_CLI)
	{
		printf("CliRadio_setmode: This radio channel cannot be used by CLI (%d)!\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.user);
		return  ;
	}
	if ((strSX127x[SX127x_CHIP1].radio.flag.bits.state != RADIO_STATE_IDLE) && (strSX127x[SX127x_CHIP1].radio.flag.bits.state != RADIO_STATE_OFF))
	{
		printf("CliRadio_setmode: Cannot send out data frame while this channel is in busy state(%d)!\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.state);
		return  ;
	}
	strSX127x[SX127x_CHIP1].radio.modulation = modulation;
	Drv_SX127x_Radio_Control(SX127x_CHIP1, RADIO_CONTROL_OPT_SET_PARAM);
	printf("Setup modulation type (%d) of radio channel!\r\n", modulation);  
}

//======================================
// Q_Shell command : 
//======================================
void stop( void )
{
	// -------------------------
	strQShellConnmand.flag.bits.csenden = 0;
	// -------------------------
	if (strSX127x[SX127x_CHIP1].radio.flag.bits.user != RADIO_USER_CLI)
	{
		printf("CliRadio_stop: This radio channel cannot be used by CLI (%d)!\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.user);
		return  ;    
	}
	Drv_SX127x_Radio_Control(SX127x_CHIP1, RADIO_CONTROL_OPT_OFF); // Turn off radio
	printf("Stop radio channel!\r\n");  
}

//======================================
// Q_Shell command : 
//======================================
void setpreamble(uint8 len)
{
	printf("\r\n");
	printf("------ radio channel setup preamble ------\r\n");
	if ((len > RADIO_PREAMBLE_MAX) || (len < RADIO_PREAMBLE_MIN))
	{
		printf("CliRadio_setpreamble: Not supported preamble length (%d)!\r\n", len);
		return  ;     
	}    
	if (strSX127x[SX127x_CHIP1].radio.flag.bits.preambleLen == len)
	{
		printf("CliRadio_setpreamble: This channel has the same preamble length (%d)!\r\n", len);
		return  ;    
	}
	if (strSX127x[SX127x_CHIP1].radio.flag.bits.user != RADIO_USER_CLI)
	{
		printf("CliRadio_setpreamble: This radio channel cannot be used by CLI (%d)!\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.user);
		return  ;    
	}
	if ((strSX127x[SX127x_CHIP1].radio.flag.bits.state != RADIO_STATE_IDLE) && (strSX127x[SX127x_CHIP1].radio.flag.bits.state != RADIO_STATE_OFF))
	{
		printf("CliRadio_setpreamble: Cannot send out data frame while this channel is in busy (%d) state!\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.state);
		return  ;    
	}
	strSX127x[SX127x_CHIP1].radio.flag.bits.preambleLen = len; // Set new preamble length
	Drv_SX127x_Radio_Control(SX127x_CHIP1, RADIO_CONTROL_OPT_SET_PARAM); // Setup whole param
	printf("Setup preamble length (%d) of radio channel!\r\n", len);  
}

//======================================
// Q_Shell command : 
//======================================
void setpwr(uint8 power)
{
	printf("\r\n");
	printf("------ radio channel setup Tx power ------\r\n");
	if (power > RADIO_POWER_MAX)
	{
		printf("CliRadio_setpwr: not supported Tx power (%d)!\r\n", power);
		return  ;     
	} 
	if (strSX127x[SX127x_CHIP1].radio.flag.bits.user != RADIO_USER_CLI)
	{
		printf("CliRadio_setpwr: This radio channel cannot be used by CLI (%d)!\r\n", strSX127x[SX127x_CHIP1].radio.flag.bits.user);
		return  ;    
	}    
	// Setup new Tx power
	strSX127x[SX127x_CHIP1].radio.send_power = power;
	Drv_SX127x_Radio_Control(SX127x_CHIP1, RADIO_CONTROL_OPT_SET_POWER);
	printf("Setup new Tx power (%ddBm) for radio channel!\r\n", power);
}

//======================================
// Q_Shell command : 
//======================================
void setuser(uint8 user)
{
	printf("\r\n");
	printf("------ radio channel set user ------\r\n");
	if (user > 1)
	{
		printf("CliRadio_setuser: Not supported user id (%d)!\r\n", user);
		return  ;     
	}    
	if (strSX127x[SX127x_CHIP1].radio.flag.bits.user == user)
	{
		printf("CliRadio_setuser: This channel is in the same user id (%d)!\r\n", user);
		return  ;    
	}
	strSX127x[SX127x_CHIP1].radio.flag.bits.user = user;
	printf("Set new user id (%d) for radio channel!\r\n", user);
}



//======================================
// Q_Shell command : 
//======================================
// void ( void )
// {
// }



/********************************/
// Q_Shell 函数注册
/********************************/
QSH_FUN_REG( ed , "void ed(uint32 tim)" );
QSH_FUN_REG( ept , "void ept(void)" );
QSH_FUN_REG( h , "void h(void)" );
QSH_FUN_REG( info , "void info(void)" );
QSH_FUN_REG( listen , "void listen(void)" );
QSH_FUN_REG( reset , "void reset(void)" );
QSH_FUN_REG( send , "void send(uint8 len)" );
QSH_FUN_REG( csend , "void csend(uint8 len)" );
QSH_FUN_REG( setbandwidth , "void setbandwidth(uint32 freq)" );
QSH_FUN_REG( setbitrate , "void setbitrate(uint32 bitrate)" );
// QSH_FUN_REG( setcrc , "void setcrc(uint8 isCRC)" );
QSH_FUN_REG( setfreq , "void setfreq(uint32 freq)" );
QSH_FUN_REG( setmode , "void setmode(uint8 modulation)" );
QSH_FUN_REG( setpreamble , "void setpreamble(uint8 len)" );
QSH_FUN_REG( setpwr , "void setpwr(uint8 power)" );
QSH_FUN_REG( setuser , "void setuser(uint8 user)" );
QSH_FUN_REG( stop , "void stop(void)" );



/********************************/


