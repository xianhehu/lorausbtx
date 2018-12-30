/*======================================
// Drv_SX127x.c文件
//======================================
======================================*/
#include "Drv_SX127x.h"
#include "Drv_SX127x_IOCTL.h"
/********************************/
#include "sx127x_reg.h"

/********************************/
#define SX127X_CHIP_VERSION         (0x12) // RegVersion value
// --- Chip type define ---
#define SX127X_TYPE_76              (0) // SX1276
#define SX127X_TYPE_77              (1) // SX1277
#define SX127X_TYPE_78              (2) // SX1278
// --- Chip descrip ---
#define SX1276_DESCRIP              ("Semtech SX1276: dual band transceiver(137 - 1020MHz)")
#define SX1277_DESCRIP              ("Semtech SX1277: dual band transceiver(137 - 1020MHz)")
#define SX1278_DESCRIP              ("Semtech SX1278: low band transceiver(137 - 525MHz)")
//======================================
/**** Local variable ****/
// --- LoRa local variable ---
// Precomputed signal bandwidth log values used to compute the packet RSSI value.
const double SignalBwLog[] =
{
    5.0969100130080564143587833158265, // 125kHz
    5.397940008672037609572522210551,  // 250kHz
    5.6989700043360188047862611052755  // 500kHz
};
const double RssiOffsetLF[] =
{   // These values need to be specify in the Lab
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
};
const double RssiOffsetHF[] =
{   // These values need to be specify in the Lab
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
};

// ---------------------------
// DIO_0 mapping type. NOTE: This is LoRa mode DIO0 mapping
typedef enum
{
    DIO0_MAP_RxDone = 0, // Rx Done
    DIO0_MAP_TxDone,     // Tx Done
    DIO0_MAP_CadDone     // Cad Done
}SX127X_DIO0_MAP;
// DIO_0 mapping type. NOTE: This is the DIO0 mapping of FSK mode (packet mode)
typedef enum
{
    DIO0_MAP_FSK_RXTX = 0,   // 00, PayloadReady and PacketSent
    DIO0_MAP_FSK_CRCOK,      // 01, CrcOk
    DIO0_MAP_FSK_TEMPBAT = 3 // 11, TempChange/LowBat
}SX127X_FSK_DIO0_MAP;
// ---------------------------
// --- Precomputed FSK bandwidth param ---
struct fsk_bandwidth
{
    uint32 bandwidth;
    uint8  reg_val;
};

#define LEN_FSK_BANDWIDTH_TBL             (21)
#ifdef RF_USE_TCXO
const struct fsk_bandwidth fskBandwidth[LEN_FSK_BANDWIDTH_TBL] = // 26 MHz TCXO
{
    { 2115  , 0x17 },   
    { 2539  , 0x0F },
    { 3173  , 0x07 },
    { 4231  , 0x16 },
    { 5078  , 0x0E },
    { 6347  , 0x06 },
    { 8463  , 0x15 },
    { 10156 , 0x0D },
    { 12695 , 0x05 },
    { 16972 , 0x14 },
    { 20312 , 0x0C },
    { 25390 , 0x04 },
    { 33854 , 0x13 },
    { 40625 , 0x0B },
    { 50781 , 0x03 },
    { 67708 , 0x12 },
    { 82600 , 0x0A },
    { 101562, 0x02 },
    { 135416, 0x11 },
    { 162500, 0x09 },
    { 203125, 0x01 }  
};
#else
const struct fsk_bandwidth fskBandwidth[LEN_FSK_BANDWIDTH_TBL] = // 32 MHz Osc
{
    { 2600  , 0x17 },   
    { 3100  , 0x0F },
    { 3900  , 0x07 },
    { 5200  , 0x16 },
    { 6300  , 0x0E },
    { 7800  , 0x06 },
    { 10400 , 0x15 },
    { 12500 , 0x0D },
    { 15600 , 0x05 },
    { 20800 , 0x14 },
    { 25000 , 0x0C },
    { 31300 , 0x04 },
    { 41700 , 0x13 },
    { 50000 , 0x0B },
    { 62500 , 0x03 },
    { 83333 , 0x12 },
    { 100000, 0x0A },
    { 125000, 0x02 },
    { 166700, 0x11 },
    { 200000, 0x09 },
    { 250000, 0x01 } 
};
#endif // RF_USE_TCXO
// ---------------------------------------



/********************************/
/****    Local defines    ****/
#ifdef RF_USE_TCXO
#define XTAL_FREQ                         (26000000)     // 26MHz TCXO
#define FREQ_STEP                         (49.591064453) // Fstep = Fxosc/2^19
#else
#define XTAL_FREQ                         (32000000)    // 32MHz OSC
#define FREQ_STEP                         (61.03515625) // Fstep = Fxosc/2^19
#endif // RF_USE_TCXO

// --- LoRa local define ---
#define SX127X_LORA_RSSI_OFFSET           (50)
// -------------------------
#define SX127X_NOISE_ABSOLUTE_ZERO        (-174.0)
#define SX127X_NOISE_FIGURE_LF            (4.0)
#define SX127X_NOISE_FIGURE_HF            (6.0)
#define SX127X_RSSI_OFFSET_LF             (-164.0)
#define SX127X_RSSI_OFFSET_HF             (-157.0)
// --- Fdev and bitrate param define ---
#define MAX_SINGLE_BANDWIDTH              (250000) // Fdev + BR/2 <= 250KHz
// 0.5 <= (b = 2*Fdev/BR) <= 10
#define MIN_BETA                          (0.5) 
#define MAX_BETA                          (10.0) 
// 600 < Fdev < 200000 Hz
#define MIN_FDEV                          (600)    // 600Hz
#define MAX_FDEV                          (200000) // 200KHz
// -------------------------------------

/********************************/
struct sx127x strSX127x[SX127x_CHIP_MAX];


/********************************/
/********************************/






//======================================
// ms延时
//======================================
void Drv_SX127x_WAIT_MS(uint16 ms)
{
    uint16 i;
    for(;ms;ms--)
    {
//         for(i=8400;i;i--);// @72MHz
        for(i=7987;i;i--);// @48MHz
    }
}

//======================================
// 限时等待DIO0中断信号
// 0-正常，1-超时
//======================================
uint8 Drv_SX127x_WAIT_DIO0ms(SX127x_CHIP_ENUM ChipID, uint32 ms)
{
    uint16 i;
    for(;ms;ms--)
    {
//         for(i=8400;i;i--);// @72MHz
         for(i=786;i;i--)// @48MHz
        {
            if ( Drv_SX127x_IOCtl_In(strSX127xIOCtl_Pin[ChipID].Pin_DIO0) )
            {
                return 0;
            }
        }
    }
    return 1;
}



//======================================
//接口初始化
//======================================
void Drv_SX127x_Init(SX127x_CHIP_ENUM ChipID)
{
    uint8 reg_value = 0;
    //======================================
    Drv_SX127x_IOCtl_Init(ChipID);
    //--------------------------
    Drv_SX127x_Control(ChipID, SX127X_CONTROL_OPT_RESET);
    //--------------------------
    // --- Init radio param ---
    strSX127x[ChipID].radio.flag.flag                 = 0;                         // Clear flag param
    strSX127x[ChipID].radio.flag.bits.state           = RADIO_STATE_IDLE;          // state -> in idle
    strSX127x[ChipID].radio.flag.bits.freqBand        = SX127X_DEFAULT_FREQ_BAND;  // Frequency band
    strSX127x[ChipID].radio.modulation                = SX127X_DEFAULT_MODULATION; // Set default modulation
    strSX127x[ChipID].radio.flag.bits.isCRC           = SX127X_DEFAULT_CRC;        // Set default CRC 
    strSX127x[ChipID].radio.flag.bits.duplex          = 0;                         // SX1278 is half duplex
    strSX127x[ChipID].radio.flag.bits.isMSBFirst      = 1;                         // By default, always use MSB first
    strSX127x[ChipID].radio.flag.bits.read_write      = RADIO_READ_WRITE;          // read/write both
    strSX127x[ChipID].radio.flag.bits.isPend          = SX127X_DEFAULT_PEND;       // Is pendable mode or unpendable mode
    strSX127x[ChipID].radio.avail_buff_size           = SX127X_BUFF_LEN;           // Set available buffer length   
    strSX127x[ChipID].radio.flag.bits.led             = SX127X_DEFAULT_LED;        // Set LED indication flag
    strSX127x[ChipID].radio.flag.bits.user            = SX127X_DEFAULT_USER;       // Set default user ID
    strSX127x[ChipID].radio.chip_descrip = NULL; // Chip description will be setup by Radio_FirstSetup()
    strSX127x[ChipID].radio.freq = SX127X_DEFAULT_FREQ; // Set default freq
    strSX127x[ChipID].radio.ChipID   = ChipID;                  // SPI device ptr
    strSX127x[ChipID].radio.channel_id  = 0;                        // Only one channel
    strSX127x[ChipID].radio.send_power  = SX127X_DEFAULT_POWER;     // Set default send power
    strSX127x[ChipID].radio.SNR         = 0;
    strSX127x[ChipID].radio.RSSI        = 0;
    strSX127x[ChipID].radio.temperature = 0;
    // ------------------------  
    strSX127x[ChipID].radio.bandwidth             = SX127X_LORA_DEFAULT_BANDWIDTH; // Set default bandwidth
    strSX127x[ChipID].radio.flag.bits.preambleLen = SX127X_LORA_DEFAULT_PREAMBLE;  // Set default preamble length
    // ------------------------  
    strSX127x[ChipID].flag.flag           = 0;                // Clear flag param
    strSX127x[ChipID].flag.bits.sf                = SX127X_LORA_DEFAULT_SF;        // Set LoRa default SF
    strSX127x[ChipID].flag.bits.coderate          = SX127X_LORA_DEFAULT_CODERATE;  // Set LoRa default code rate
    // ------------------------  
//     Mem_Clr(strSX127x[ChipID].buff, SX127X_BUFF_LEN); // Clear buffer
    // --- Init SX127X chip specific param ---
    strSX127x[ChipID].flag.bits.chip_type = SX127X_CHIP_TYPE; // Set up chip type
    //--------------------------
    Drv_SX127X_RxChainCalibration(ChipID);       // Perform Rx chain calibration
#ifdef RF_USE_TCXO  
    Drv_SX127x_SingleWrite(ChipID, SX127X_PUB_RegTcxo, 0x09 | BIT4); // Use TCXO
#endif // RF_USE_TCXO  
    //--------------------------
    Drv_SX127x_CheckVersion(ChipID);    // Checkout its version
    Drv_SX127x_SetModulation(ChipID);    // Set modulation type based on its modulation type (which setup in BSP_Radio_Init()) 
    Drv_SX127x_SetFreq(ChipID); // Set default freq
    if (strSX127x[ChipID].radio.modulation == RADIO_MTYPE_LORA)
    { // LoRa modulation
        // ?? Set LoRa SyncWord
        Drv_SX127x_LoRa_SetMode(ChipID, LORA_MODE_STDBY); // Set in standby mode
        // --- Channel param --- 
        Drv_SX127x_LoRa_SetSF(ChipID);                  // Set default SF
        Drv_SX127x_LoRa_SetCodeRate(ChipID);            // Set coderate
        Drv_SX127x_LoRa_SetCRC(ChipID);                 // Set CRC on or off
        Drv_SX127x_LoRa_SetBandwidth(ChipID);           // Set bandwidth
        Drv_SX127x_LoRa_SetSymbTimeout(ChipID, 0x3FF);  // Set RX Symble Time-out
        Drv_SX127x_LoRa_SetPreambleLength(ChipID);      // Set default preamble length
        // --------------------- 
    }
    else 
    { // FSK modulation
        Drv_SX127x_FSK_SetMode(ChipID, FSK_MODE_STDBY); // Set in standby mode

        // --- Channel param ---
        Drv_SX127x_FSK_SetDefaultParam(ChipID);   // First set default param
        Drv_SX127x_FSK_SetBitrate(ChipID);        // Set bitrate
        Drv_SX127x_FSK_SetFdev(ChipID);           // Set Fdev
        Drv_SX127x_FSK_SetCRC(ChipID);            // Set CRC
        Drv_SX127x_FSK_SetRxBandwidth(ChipID);    // Set Rx bandwidth
        Drv_SX127x_FSK_SetPreambleLength(ChipID); // Set preamble length
    }

    // Output power
    Drv_SX127x_SetPABOOST(ChipID);  // Select PA_BOOST pin
    Drv_SX127x_SetOutPower(ChipID); // Set output power

    // --- 2015-12-27-Season: Close OCP and enable LNA ---
    // Disable OCP(over current protection)
    Drv_SX127x_SingleRead(ChipID, SX127X_PUB_RegOcp, &reg_value); // Read out current value
    reg_value &= (~BIT5);
    Drv_SX127x_SingleWrite(ChipID, SX127X_PUB_RegOcp, reg_value); // Write new value

    if (strSX127x[ChipID].radio.modulation == RADIO_MTYPE_LORA)
    {
        Drv_SX127x_SingleWrite(ChipID, SX127X_PUB_RegLna, 0x23); // Enable LNA 
    }
    else
    {
        Drv_SX127x_SingleWrite(ChipID, SX127X_PUB_RegLna, 0x20); // Enable LNA 
    }
    
    // ---------------------------------------------------
#if 1
    if (strSX127x[ChipID].radio.modulation == RADIO_MTYPE_LORA) // After finish init, set chip in sleep mode
    { // LoRa modulation
        Drv_SX127x_LoRa_SetMode(ChipID, LORA_MODE_SLEEP); // Set in sleep mode
    }
    else
    { // FSK modulation
        Drv_SX127x_FSK_SetMode(ChipID, FSK_MODE_SLEEP); // Set in sleep mode
      
    }
#endif
    
    strSX127x[ChipID].radio.flag.bits.init = 1; // Finally, finish init 
    //--------------------------
    //--------------------------
    //--------------------------
#if DRV_SX127x_TEST_EN
    Drv_SX127x_Test_RadioFIFO(ChipID);
#endif    //DRV_SX127x_TEST_EN
    //--------------------------
    //Drv_SX127x_FSK_SetMode(ChipID, FSK_MODE_SLEEP);
}

//======================================
//操作控制
//======================================
sint8 Drv_SX127x_Control(SX127x_CHIP_ENUM ChipID, uint32 opt)
{
    if(ChipID >= SX127x_CHIP_MAX)
        return -1;
    if (opt == SX127X_CONTROL_OPT_RESET)
    { // Reset
        Drv_SX127x_IOCtl_Out(strSX127xIOCtl_Pin[ChipID].Pin_NSS, 
                             DRV_SX127x_IOCTL_HIG);
        Drv_SX127x_IOCtl_Out(strSX127xIOCtl_Pin[ChipID].Pin_NRESET, 
                             DRV_SX127x_IOCTL_LOW);
        Drv_SX127x_WAIT_MS(1);
        Drv_SX127x_IOCtl_Out(strSX127xIOCtl_Pin[ChipID].Pin_NRESET, 
                             DRV_SX127x_IOCTL_HIG);
        Drv_SX127x_WAIT_MS(6);
    }
    else if (opt == SX127X_CONTROL_OPT_TX)
    { // Switch to TX
        Drv_SX127x_IOCtl_Out(strSX127xIOCtl_Pin[ChipID].Pin_CPS, 
                             DRV_SX127x_IOCTL_HIG);
#ifdef RF_USE_TCXO     
        Drv_SX127x_IOCtl_Out(strSX127xIOCtl_Pin[ChipID].Pin_CTX, 
                             DRV_SX127x_IOCTL_LOW);
#else
        Drv_SX127x_IOCtl_Out(strSX127xIOCtl_Pin[ChipID].Pin_CTX, 
                             DRV_SX127x_IOCTL_HIG);
#endif    
    }
    else if (opt == SX127X_CONTROL_OPT_RX)
    { // Switch to RX
        Drv_SX127x_IOCtl_Out(strSX127xIOCtl_Pin[ChipID].Pin_CPS, 
                             DRV_SX127x_IOCTL_LOW);
        Drv_SX127x_IOCtl_Out(strSX127xIOCtl_Pin[ChipID].Pin_CTX, 
                             DRV_SX127x_IOCTL_HIG);
// #ifndef RF_USE_TCXO    
// #endif    
    }
    else if (opt == SX127X_CONTROL_OPT_EN_DIO0_INT)
    { // Enable DIO_0 interrupt
//         Drv_SX127x_IOCtl_IRQ_EN(strSX127xIOCtl_Pin[ChipID].Pin_DIO0, ENABLE);
    }
    else if (opt == SX127X_CONTROL_OPT_DIS_DIO0_INT)
    { // Disable DIO_0 interrupt
//         Drv_SX127x_IOCtl_IRQ_EN(strSX127xIOCtl_Pin[ChipID].Pin_DIO0, DISABLE);
    }
    else
    { // Not supported opt
#ifdef USE_DEBUG_ERR    
        Debug_Error("BSP_SX1278_Control: cannot setup state mode 
                    which radio not in FSK modulation!\r\n");
#endif
#ifdef DEBUG 
        printk("BSP_SX1278_Control: cannot setup state mode 
               which radio not in FSK modulation!\r\n");
#endif 
        return -2;
    }
    return 1;
}

//======================================
// Decrip: Read one data from SX127x.
//======================================
sint8 Drv_SX127x_SingleRead(SX127x_CHIP_ENUM ChipID, const uint8 addr, 
                            uint8 *read_data)
{
    uint8 out_buff[2];
    uint8 in_buff[2];
    if(ChipID >= SX127x_CHIP_MAX)
        return -1;
    // Prepare frame to be sent
    out_buff[0] = addr;
    out_buff[1] = 0x00;
    in_buff[0] = 0x00;
    in_buff[1] = 0x00;
    Drv_SX127x_IOCtl_Out(strSX127xIOCtl_Pin[ChipID].Pin_NSS, 
                         DRV_SX127x_IOCTL_LOW);
    if (Drv_Spix_Transfer(strSX127xIOCtl_Pin[ChipID].PortID, out_buff, 2, 
                          in_buff, 2, true) > 0)
    { // Transfer success
        read_data[0] = in_buff[1]; // Get received data
        Drv_SX127x_IOCtl_Out(strSX127xIOCtl_Pin[ChipID].Pin_NSS, 
                             DRV_SX127x_IOCTL_HIG);
        return 1;
    }
    Drv_SX127x_IOCtl_Out(strSX127xIOCtl_Pin[ChipID].Pin_NSS, 
                         DRV_SX127x_IOCTL_HIG);
    return 0;   
}

//======================================
// Decrip: Write one data to SX127x
//======================================
sint8 Drv_SX127x_SingleWrite(SX127x_CHIP_ENUM ChipID, const uint8 addr, 
                             const uint8 write_data)
{
    uint8 out_buff[2];
    sint8 result;  
    if(ChipID > SX127x_CHIP_MAX)
        return -1;
  // Prepare frame to be sent
  out_buff[0] = (BIT7 | addr); // Write address  
  out_buff[1] = write_data;    // Write data 
  Drv_SX127x_IOCtl_Out(strSX127xIOCtl_Pin[ChipID].Pin_NSS, DRV_SX127x_IOCTL_LOW);
  result = Drv_Spix_Transfer(strSX127xIOCtl_Pin[ChipID].PortID, out_buff, 2, 
                             NULL, 0, true);
  Drv_SX127x_IOCtl_Out(strSX127xIOCtl_Pin[ChipID].Pin_NSS, DRV_SX127x_IOCTL_HIG);
  return result;   
}

//======================================
// Decrip: Read more than one data from SX127x.
//======================================
sint8 Drv_SX127x_BurstRead(SX127x_CHIP_ENUM ChipID, const uint8 addr, 
                           uint8 *read_buff, uint8 len)
{
    if(ChipID > SX127x_CHIP_MAX)
        return -1;
    uint32 tempLen;
    if (len > SX127X_BUFF_LEN)
    { // We only read out <= SX127X_BUFF_LEN data(including addr, so available data len is SX127X_BUFF_LEN - 1)
#ifdef DEBUG 
        printk("Drv_SX127x_BurstRead: len > SX127X_BUFF_LEN!\r\n");
#endif    
        tempLen = SX127X_BUFF_LEN;
    }
    else
    {
        tempLen = len;
    }
    sint8 result; 
    Drv_SX127x_IOCtl_Out(strSX127xIOCtl_Pin[ChipID].Pin_NSS, DRV_SX127x_IOCTL_LOW);
    Drv_Spix_ReadWriteByte( strSX127xIOCtl_Pin[ChipID].PortID, addr);
    result = Drv_Spix_Transfer(strSX127xIOCtl_Pin[ChipID].PortID, strSX127x[ChipID].buff, 
                               tempLen, read_buff, tempLen, true);
    Drv_SX127x_IOCtl_Out(strSX127xIOCtl_Pin[ChipID].Pin_NSS, DRV_SX127x_IOCTL_HIG);
    return result;
}

//======================================
// Decrip: Write more than one data to SX127x.
//======================================
sint8 Drv_SX127x_BurstWrite(SX127x_CHIP_ENUM ChipID, const uint8 addr, 
                            uint8 *write_buff, uint8 len)
{
    sint8 result;
    if(ChipID > SX127x_CHIP_MAX)
        return -1;
    // --- Start transfer --- 
    Drv_SX127x_IOCtl_Out(strSX127xIOCtl_Pin[ChipID].Pin_NSS, DRV_SX127x_IOCTL_LOW);
    Drv_Spix_ReadWriteByte( strSX127xIOCtl_Pin[ChipID].PortID, (BIT7 | addr));
    result = Drv_Spix_Transfer(strSX127xIOCtl_Pin[ChipID].PortID, 
                               write_buff, len, NULL, 0, true);
    Drv_SX127x_IOCtl_Out(strSX127xIOCtl_Pin[ChipID].Pin_NSS, DRV_SX127x_IOCTL_HIG);
    return result; 
}

//======================================
// Decrip: Set freq param from SX127X_PUB_RegFrfMsb, SX127X_PUB_RegFrfMid, SX127X_PUB_RegFrfLsb
//======================================
void Drv_SX127x_SetFreq(SX127x_CHIP_ENUM ChipID)
{
    if(ChipID > SX127x_CHIP_MAX)
        return  ;
    uint32 freq = strSX127x[ChipID].radio.freq;
    uint8 frfReg[3]; // Write buff
    freq      = (uint32)((float64)freq / (float64)FREQ_STEP);
    frfReg[0] = (uint8)((freq >> 16) & 0xFF); // MSB
    frfReg[1] = (uint8)((freq >> 8) & 0xFF);  // Mid
    frfReg[2] = (uint8)(freq & 0xFF);         // LSB 
    Drv_SX127x_BurstWrite(ChipID, SX127X_PUB_RegFrfMsb, frfReg, 3); // Write freq param
}

//======================================
// Decrip: Read out its version_reg to checkout the chip. 
//======================================
sint8 Drv_SX127x_CheckVersion(SX127x_CHIP_ENUM ChipID)
{
    uint8 read_value = 0;
    if(ChipID >= SX127x_CHIP_MAX)
        return -1;
    Drv_SX127x_SingleRead(ChipID, SX127X_PUB_RegVersion, &read_value); // Read out its version 
    if (read_value != SX127X_CHIP_VERSION)
    {
#ifdef USE_DEBUG_ERR    
        Debug_Error("Drv_SX127x_CheckVersion: read_value(%d) not right!\r\n", 
                    read_value);
#endif
#ifdef DEBUG 
        printk("Drv_SX127x_CheckVersion: read_value(%d) not right!\r\n", 
               read_value);
#endif     
        return 0;
    }
    else
    { // Right value
        if (strSX127x[ChipID].flag.bits.chip_type == SX127X_TYPE_76)
        { // SX1276
            strSX127x[ChipID].radio.chip_descrip = SX1276_DESCRIP;
        }
        else if (strSX127x[ChipID].flag.bits.chip_type == SX127X_TYPE_77)
        { // SX1277
            strSX127x[ChipID].radio.chip_descrip = SX1277_DESCRIP;
        }
        else
        { // SX1278
            strSX127x[ChipID].radio.chip_descrip = SX1278_DESCRIP;
        }
    }
    return 1;
}

//======================================
// Perform Rx chain calibration
//======================================
void Drv_SX127X_RxChainCalibration(SX127x_CHIP_ENUM ChipID)
{
    uint8 reg_value = 0;
    uint8 temp_value = 0;
    uint32 save_freq = 0;
    if(ChipID > SX127x_CHIP_MAX)
        return  ;
    // Save contex
    Drv_SX127x_SingleRead(ChipID, SX127X_PUB_RegPaConfig, &reg_value); // Read out its version 
    // Cut the PA just in case, RFO output, power = -1 dBm
    Drv_SX127x_SingleWrite(ChipID, SX127X_PUB_RegPaConfig, 0x00);
    save_freq = strSX127x[ChipID].radio.freq;
    if (strSX127x[ChipID].radio.freq > 700000000)
    {
        strSX127x[ChipID].radio.freq = 868000000;
        Drv_SX127x_SetFreq(ChipID);
    }
    // Launch Rx chain calibration 
    Drv_SX127x_SingleRead(ChipID, SX127X_FSK_RegImageCal, &temp_value);
    Drv_SX127x_SingleWrite(ChipID, SX127X_FSK_RegImageCal, ((temp_value & 0xBF) | 0x40));
    Drv_SX127x_SingleRead(ChipID, SX127X_FSK_RegImageCal, &temp_value);
    while ((temp_value & 0x20) != 0)
    {
        Drv_SX127x_SingleRead(ChipID, SX127X_FSK_RegImageCal, &temp_value);
    }
    // Restore contex
    Drv_SX127x_SingleWrite(ChipID, SX127X_PUB_RegPaConfig, reg_value); 
    strSX127x[ChipID].radio.freq = save_freq;
}

//======================================
// Decrip: Set SX127X RegOpMode field. LoRa run mode
//======================================
void Drv_SX127x_LoRa_SetMode(SX127x_CHIP_ENUM ChipID, SX127X_LORA_MODE setMode)
{
    uint8 reg_value = 0;
    if(ChipID > SX127x_CHIP_MAX)
        return  ;
    Drv_SX127x_SingleRead(ChipID, SX127X_PUB_RegOpMode, &reg_value);    // Read current OPMODE reg value
    reg_value = ((reg_value & SX127X_RegOpMode_MODE_MASK) | setMode); // Set mode
    Drv_SX127x_SingleWrite(ChipID, SX127X_PUB_RegOpMode, reg_value);    // Set new OPMODE
    strSX127x[ChipID].flag.bits.mode = setMode;         // Set new mode
    Drv_SX127x_Control(ChipID, SX127X_CONTROL_OPT_RX); // Switch to Rx(by default)
    // --- Set up radio channel mode ---
    if (setMode == LORA_MODE_SLEEP)  
    { // Sleep
        strSX127x[ChipID].radio.flag.bits.state = RADIO_STATE_OFF;
    }
    else if (setMode == LORA_MODE_STDBY)
    { // Standby
        strSX127x[ChipID].radio.flag.bits.state = RADIO_STATE_IDLE;
    }
    else if ((setMode == LORA_MODE_FSTX) || (setMode == LORA_MODE_TX))
    { // Tx
        Drv_SX127x_Control(ChipID, SX127X_CONTROL_OPT_TX);       // Switch to Tx
        strSX127x[ChipID].radio.flag.bits.state = RADIO_STATE_TX;
    }
    else if ((setMode == LORA_MODE_FSRX)         || 
        (setMode == LORA_MODE_RXCONTINUOUS) || 
        (setMode == LORA_MODE_RXSINGLE))  
    { // Rx
        strSX127x[ChipID].radio.flag.bits.state = RADIO_STATE_RX;
    }
    else // if (setMode == LORA_MODE_CAD)
    { // CAD listening
        strSX127x[ChipID].radio.flag.bits.state = RADIO_STATE_LISTENING;
    }
}

//======================================
// Decrip: Set the modulation of SX127X, LoRa or FSK.
//             Before use this func, make sure you have setup the modulation type (radio_channel.flag.bits.modulation).
//======================================
sint8 Drv_SX127x_SetModulation(SX127x_CHIP_ENUM ChipID)
{
    uint8 reg_value = 0;
    if(ChipID > SX127x_CHIP_MAX)
        return -1;
    if (strSX127x[ChipID].radio.modulation == RADIO_MTYPE_LORA)
    { // LoRa
        Drv_SX127x_LoRa_SetMode(ChipID, LORA_MODE_SLEEP);                // Set in sleep mode
        Drv_SX127x_SingleRead(ChipID, SX127X_PUB_RegOpMode, &reg_value); // Read out current value
        reg_value |= SX127X_RegOpMode_LORA_ON;                         // Set LoRa on
        Drv_SX127x_SingleWrite(ChipID, SX127X_PUB_RegOpMode, reg_value); // Write new value
    }
    else if ((strSX127x[ChipID].radio.modulation == RADIO_MTYPE_2GFSK) || 
        (strSX127x[ChipID].radio.modulation == RADIO_MTYPE_4GFSK))
    { // FSK
        Drv_SX127x_FSK_SetMode(ChipID, FSK_MODE_SLEEP);                  // Set in sleep mode
        Drv_SX127x_SingleRead(ChipID, SX127X_PUB_RegOpMode, &reg_value); // Read out current value
        reg_value &= ~SX127X_RegOpMode_LORA_ON;                        // Set FSK or OOK modulation 
        // Set modulation type: FSK
        reg_value &= SX127X_RegOpMode_MTYPE_MASK;                      // Set FSK mode (00 -> FSK)
        Drv_SX127x_SingleWrite(ChipID, SX127X_PUB_RegOpMode, reg_value); // Write new value
    }
    else
    {
#ifdef USE_DEBUG_ERR    
        Debug_Error("Drv_SX127x_SetModulation: modulation type not supported!\r\n", 
                    strSX127x[ChipID].radio.modulation);
#endif
#ifdef DEBUG  
        printk("Drv_SX127x_SetModulation: modulation type not supported!\r\n", 
               strSX127x[ChipID].radio.modulation);     
#endif // DEBUG 
//         return (-EPERM);    
        return -2;
    }

    return 1;
}

//======================================
// Decrip: Set LoRa detection optimize. 0x03, SF7 to SF12; 0x05, SF6.
//======================================
void Drv_SX127x_LoRa_SetNbtrigPeaks(SX127x_CHIP_ENUM ChipID, uint8 value)
{
    uint8 reg_value = 0;
    if(ChipID > SX127x_CHIP_MAX)
        return  ;
    Drv_SX127x_SingleRead(ChipID, SX127X_LORA_RegDetectOptimize, &reg_value); // Read out its current value
    reg_value = ((reg_value & 0xF8) | value);
    Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegDetectOptimize, reg_value); // Set new value 
}

//======================================
// Decrip: When the SF of current channel is SF11 or SF12, set the SX127X_LORA_RegModemConfig3
//             BIT3[LowDataRateOptimize] to be 1 of , or if less than SF11, clear to be 0.
//======================================
void Drv_SX127x_LoRa_SetLowDataRateOptimize(SX127x_CHIP_ENUM ChipID)
{
    uint8 reg_value = 0;
    if(ChipID > SX127x_CHIP_MAX)
        return  ;
    Drv_SX127x_SingleRead(ChipID, SX127X_LORA_RegModemConfig3, &reg_value); // Read out its current value
    if (strSX127x[ChipID].flag.bits.sf > LORA_PARAM_SF_10 && strSX127x[ChipID].flag.bits.low_speed)
    { // Enbale low datarate
        reg_value |= BIT3; 
        Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegModemConfig3, reg_value); // Set bit
    }
    else
    { // Disable low datarate
        reg_value &= (~BIT3);
        Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegModemConfig3, reg_value); // Clear bit
    }
}

//======================================
// Decrip: Setup its spreading factor(SF).
//======================================
void Drv_SX127x_LoRa_SetSF(SX127x_CHIP_ENUM ChipID)
{
    uint8 reg_value = 0;  
    if(ChipID > SX127x_CHIP_MAX)
        return  ;
    if (strSX127x[ChipID].flag.bits.sf == LORA_PARAM_SF_6)
    {
        Drv_SX127x_LoRa_SetNbtrigPeaks(ChipID, 5);
    }
    else
    {
        Drv_SX127x_LoRa_SetNbtrigPeaks(ChipID, 3);
    }
    Drv_SX127x_SingleRead(ChipID, SX127X_LORA_RegModemConfig2, &reg_value); // Read out its current value
    reg_value = ((reg_value & SX127X_RegModemConfig2_SF_MASK) 
                 | (strSX127x[ChipID].flag.bits.sf << 4));
    Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegModemConfig2, reg_value); // Set new SF 
    Drv_SX127x_LoRa_SetLowDataRateOptimize(ChipID); // Set low data rate optimize 
}

void Drv_SX127x_LoRa_SetInvertIQ(SX127x_CHIP_ENUM ChipID)
{
    uint8 reg_value = 0;  
    if(ChipID > SX127x_CHIP_MAX)
        return;
     Drv_SX127x_SingleRead(ChipID, SX127X_LORA_RegInvertIQ, &reg_value);
    if (strSX127x[ChipID].flag.bits.rx_IQInvert)
        reg_value |= 1<<6;
    if (strSX127x[ChipID].flag.bits.tx_IQInvert)
        reg_value &= ~1;
    Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegInvertIQ, reg_value);
}

//======================================
// Decrip: Setup its coderate.
//======================================
void Drv_SX127x_LoRa_SetCodeRate(SX127x_CHIP_ENUM ChipID)
{
    uint8 reg_value = 0;
    if(ChipID > SX127x_CHIP_MAX)
        return  ;
    Drv_SX127x_SingleRead(ChipID, SX127X_LORA_RegModemConfig1, &reg_value); // Read out its current value
    reg_value = ((reg_value & SX127X_RegModemConfig1_CR_MASK) 
                 | (strSX127x[ChipID].flag.bits.coderate << 1));
    Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegModemConfig1, reg_value); // Set new code rate 
}

//======================================
// Decrip: Set CRC on or off.
//======================================
void Drv_SX127x_LoRa_SetCRC(SX127x_CHIP_ENUM ChipID)
{
    uint8 reg_value = 0;
    if(ChipID > SX127x_CHIP_MAX)
        return  ;
    Drv_SX127x_SingleRead(ChipID, SX127X_LORA_RegModemConfig2, &reg_value); // Read out its current value
    if (strSX127x[ChipID].radio.flag.bits.isCRC == 1)
    { // CRC on
        reg_value |= SX127X_RegModemConfig2_CRC; 
    }
    else 
    { // CRC off
        reg_value &= ~SX127X_RegModemConfig2_CRC;
    }
    Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegModemConfig2, reg_value); // Set new CRC param
}

//======================================
// Decrip: Set Bandwidth.
//======================================
void Drv_SX127x_LoRa_SetBandwidth(SX127x_CHIP_ENUM ChipID)
{
    uint8 reg_value = 0;
    if(ChipID > SX127x_CHIP_MAX)
        return  ;
    Drv_SX127x_SingleRead(ChipID, SX127X_LORA_RegModemConfig1, &reg_value); // Read out its current value
    if (strSX127x[ChipID].radio.bandwidth == 125000)
    { // 125k
        reg_value = ((reg_value & SX127X_RegModemConfig1_BW_MASK) | SX127X_RegModemConfig1_BW_125K);
    }
    else if (strSX127x[ChipID].radio.bandwidth == 62500)
    { // 62.5k
        reg_value = ((reg_value & SX127X_RegModemConfig1_BW_MASK) | SX127X_RegModemConfig1_BW_62D5K);
    }
    else if (strSX127x[ChipID].radio.bandwidth == 41700)
    { // 41.7k
        reg_value = ((reg_value & SX127X_RegModemConfig1_BW_MASK) | SX127X_RegModemConfig1_BW_41D7K);
    }
    else if (strSX127x[ChipID].radio.bandwidth == 31250)
    { // 31.25k
        reg_value = ((reg_value & SX127X_RegModemConfig1_BW_MASK) | SX127X_RegModemConfig1_BW_31D25K);
    }
    else
    {
#ifdef USE_DEBUG_ERR    
        Debug_Error("Drv_SX127x_LoRa_SetBandwidth: radio.bandwidth(%d) not supported!\r\n", strSX127x[ChipID].radio.bandwidth);
#endif
#ifdef DEBUG 
        printk("Drv_SX127x_LoRa_SetBandwidth: radio.bandwidth(%d) not supported!\r\n", strSX127x[ChipID].radio.bandwidth);
#endif

        return;    
    }
    Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegModemConfig1, reg_value); // Set new bandwidth
}

//======================================
// 
//======================================
void Drv_SX127x_LoRa_SetSymbTimeout(SX127x_CHIP_ENUM ChipID, uint16 value)
{
    uint8 reg_value = 0;
    uint8 reg_value2 = 0;
    if(ChipID > SX127x_CHIP_MAX)
        return  ;
    // --- Set Symble Timeout ---
    // MSB
    Drv_SX127x_SingleRead(ChipID, SX127X_LORA_RegModemConfig2, &reg_value); // Read out its current value
    reg_value = ((reg_value & SX127X_RegModemConfig2_SMBT_MASK) | ((value >> 8) & (~SX127X_RegModemConfig2_SMBT_MASK))); 
    Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegModemConfig2, reg_value); // Set new symble timeout MSB
    // LSB
    reg_value2 = (value & 0xFF);
    Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegSymbTimeoutLsb, reg_value2); // Set new symble timeout LSB
}

//======================================
// Decrip: Setup its preamble length.
//======================================
void Drv_SX127x_LoRa_SetPreambleLength(SX127x_CHIP_ENUM ChipID)
{
    if(ChipID > SX127x_CHIP_MAX)
        return  ;
    Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegPreambleMsb, 0);                                             // MSB always set 0
    Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegPreambleLsb, strSX127x[ChipID].radio.flag.bits.preambleLen); // LSB
}

void Drv_SX127x_LoRa_SetSyncWord(SX127x_CHIP_ENUM ChipID)
{
    if(ChipID > SX127x_CHIP_MAX)
        return;
    Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegSyncWord, strSX127x[ChipID].radio.syncword); // Set Syncword
}

//======================================
// Decrip: Set SX127X RegOpMode field. FSK run mode.
//======================================
void Drv_SX127x_FSK_SetMode(SX127x_CHIP_ENUM ChipID, SX127X_FSK_MODE setMode)
{
    uint8 reg_value = 0;
    if(ChipID > SX127x_CHIP_MAX)
        return  ;
    Drv_SX127x_SingleRead(ChipID, SX127X_PUB_RegOpMode, &reg_value);    // Read current OPMODE reg value
    reg_value = ((reg_value & SX127X_RegOpMode_MODE_MASK) | setMode); // Set mode
    Drv_SX127x_SingleWrite(ChipID, SX127X_PUB_RegOpMode, reg_value);    // Set new OPMODE
    strSX127x[ChipID].flag.bits.mode = setMode;         // Set new mode
    Drv_SX127x_Control(ChipID, SX127X_CONTROL_OPT_RX); // Switch to Rx(by default)
    // --- Set up radio channel mode ---
    if (setMode == FSK_MODE_SLEEP)   
    {// Sleep
        strSX127x[ChipID].radio.flag.bits.state = RADIO_STATE_OFF;
    }
    else if (setMode == FSK_MODE_STDBY)
    { // Standby
        strSX127x[ChipID].radio.flag.bits.state = RADIO_STATE_IDLE;
    }
    else if ((setMode == FSK_MODE_FSTX) || (setMode == FSK_MODE_TX))
    { // Tx
        Drv_SX127x_Control(ChipID, SX127X_CONTROL_OPT_TX);       // Switch to Tx
        strSX127x[ChipID].radio.flag.bits.state = RADIO_STATE_TX;
    }
    else //if ((setMode == FSK_MODE_FSRX) || (setMode == FSK_MODE_RX))  
    { // Rx
        strSX127x[ChipID].radio.flag.bits.state = RADIO_STATE_RX;
    }  
}

//======================================
// Decrip: Setup FSK default param (after reset, when power up or setup from LoRa mode to FSK mode).
//======================================
void Drv_SX127x_FSK_SetDefaultParam(SX127x_CHIP_ENUM ChipID)
{
    if(ChipID > SX127x_CHIP_MAX)
        return  ;
    Drv_SX127x_SingleWrite(ChipID, SX127X_PUB_RegPaRamp, 0x20);     // Gaussian filter BT = 1.0
    Drv_SX127x_SingleWrite(ChipID, SX127X_FSK_RegRxConfig, 0x1E);   // Rx config: AFC auto, AGC, trigger on preamble
    Drv_SX127x_SingleWrite(ChipID, SX127X_FSK_RegRssiConfig, 0xD2); //
    Drv_SX127x_SingleWrite(ChipID, SX127X_FSK_RegSyncConfig, 0x93); 
    // 3 Sync word
    Drv_SX127x_SingleWrite(ChipID, SX127X_FSK_RegSyncValue1, 0x69);
    Drv_SX127x_SingleWrite(ChipID, SX127X_FSK_RegSyncValue2, 0x81);
    Drv_SX127x_SingleWrite(ChipID, SX127X_FSK_RegSyncValue3, 0x7E);
    Drv_SX127x_SingleWrite(ChipID, SX127X_FSK_RegSyncValue4, 0x96);
    Drv_SX127x_SingleWrite(ChipID, SX127X_FSK_RegPreambleDetect, 0x12); 
    Drv_SX127x_SingleWrite(ChipID, SX127X_FSK_RegFifoThresh, 0x89); 
    // Set packet config
    Drv_SX127x_SingleWrite(ChipID, SX127X_FSK_RegPacketConfig1, 0xD0); 
    Drv_SX127x_SingleWrite(ChipID, SX127X_FSK_RegPacketConfig2, 0x40); // packet mode
    Drv_SX127x_SingleWrite(ChipID, SX127X_FSK_RegPayloadLength, 
                           strSX127x[ChipID].radio.avail_buff_size); // Max length in Rx
}

//======================================
// Decrip: Setup FSK bitrate (in bps).
//             BitRate = FXOSC / (BitRate(15, 0) + (BitRateFrac / 16)), BitRateFrac = 0 (default value),
//             and BitRate = FXOSC / (BitRate(15, 0).
//======================================
void Drv_SX127x_FSK_SetBitrate(SX127x_CHIP_ENUM ChipID)
{
    if(ChipID > SX127x_CHIP_MAX)
        return  ;
    uint16 temp = (uint16)((float64)XTAL_FREQ / ((float64)strSX127x[ChipID].radio.bitrate));
    Drv_SX127x_SingleWrite(ChipID, SX127X_FSK_RegBitratMsb, (uint8)(temp >> 8));
    Drv_SX127x_SingleWrite(ChipID, SX127X_FSK_RegBitrateLsb, (uint8)(temp & 0xFF));
}

//======================================
// Decrip: Check bitrate and bandwidth and than calculate and setup Fdev.
//             1. Fdev + BR/2 <= 250KHz
//             2. 0.5 <= (Beta = 2 * Fdev / BR) <= 10
//             3. Fdev = (BW - BR) / 2, So BW > 2 * BR 
//======================================
sint8 Drv_SX127x_FSK_SetFdev(SX127x_CHIP_ENUM ChipID)
{
    if(ChipID > SX127x_CHIP_MAX)
        return -1;
    uint32 fdev = (strSX127x[ChipID].radio.bandwidth - strSX127x[ChipID].radio.bitrate) / 2; // Calculate Fdev
    // --- Setup Fdev ---
    uint16 temp = (uint16)((float64)fdev / (float64)FREQ_STEP);
    uint8 reg_value = 0;
    Drv_SX127x_SingleRead(ChipID, SX127X_FSK_RegFdevMsb, &reg_value); 
    Drv_SX127x_SingleWrite(ChipID, SX127X_FSK_RegFdevMsb, ((reg_value & SX127X_RegFdevMsb_MASK) | 
        (((uint8)(temp >> 8)) & (~SX127X_RegFdevMsb_MASK)) & 0xFF));
    Drv_SX127x_SingleWrite(ChipID, SX127X_FSK_RegFdevLsb, (uint8)(temp & 0xFF));  
    // ------------------
    return 1;
}

//======================================
// Decrip: Set CRC on or off.
//======================================
void Drv_SX127x_FSK_SetCRC(SX127x_CHIP_ENUM ChipID)
{
    uint8 reg_value = 0;
    if(ChipID > SX127x_CHIP_MAX)
        return  ;
    Drv_SX127x_SingleRead(ChipID, SX127X_FSK_RegPacketConfig1, &reg_value); // Read current reg value
    if (strSX127x[ChipID].radio.flag.bits.isCRC == 1)
    { // CRC on
        reg_value |= BIT4; // Set bit
    }
    else
    { // CRC off
        reg_value &= ~BIT4; // Clear bit
    }
    Drv_SX127x_SingleWrite(ChipID, SX127X_FSK_RegPacketConfig1, reg_value); // Reset reg value
}

//======================================
// Decrip: Check and find out the fitable bandwdith reg value.
//======================================
uint8 Drv_SX127x_FSK_GetBandwidthRegVal(uint32 bandwidth)
{
    uint16 i;
    for (i = 0; i < LEN_FSK_BANDWIDTH_TBL; i++)
    {
        if ((bandwidth >= fskBandwidth[i].bandwidth)     && 
            (bandwidth < fskBandwidth[i + 1].bandwidth))
        {
            return i;
        }
    }
#ifdef USE_DEBUG_ERR    
    Debug_Error("Drv_SX127x_FSK_GetBandwidthRegVal: not supported bandwidth (%d)!\r\n", bandwidth);
#endif
#ifdef DEBUG  
    printk("Drv_SX127x_FSK_GetBandwidthRegVal: not supported bandwidth (%d)!\r\n", bandwidth);    
#endif // DEBUG  
    return 0;
}

//======================================
// Decrip: Setup RXBW and AFCBW.
//======================================
void Drv_SX127x_FSK_SetRxBandwidth(SX127x_CHIP_ENUM ChipID)
{
    if(ChipID > SX127x_CHIP_MAX)
        return  ;
    uint8 i = Drv_SX127x_FSK_GetBandwidthRegVal(strSX127x[ChipID].radio.bandwidth / 2 + 1000); // Bandwidth / 2 + 1000 Hz = Rx_Bandwidth
    if ((i != 0) && (i < (LEN_FSK_BANDWIDTH_TBL - 1)))
    {
        Drv_SX127x_SingleWrite(ChipID, SX127X_FSK_RegRxBw, fskBandwidth[i].reg_val); 
    }
    else
    {
#ifdef USE_DEBUG_ERR    
        Debug_Error("Drv_SX127x_FSK_SetRxBandwidth: cannot get suitable bandwidth param!\r\n");
#endif
#ifdef DEBUG  
        printk("Drv_SX127x_FSK_SetRxBandwidth: cannot get suitable bandwidth param!\r\n");     
#endif // DEBUG
        return;      
    }
    Drv_SX127x_SingleWrite(ChipID, SX127X_FSK_RegAfcBw, fskBandwidth[i + 1].reg_val); // AFC bandwidth must be bigger than Rx bandwidth
}

//======================================
// Decrip: Setup preamble length.
//======================================
void Drv_SX127x_FSK_SetPreambleLength(SX127x_CHIP_ENUM ChipID)
{
    if(ChipID > SX127x_CHIP_MAX)
        return  ;
    Drv_SX127x_SingleWrite(ChipID, SX127X_FSK_RegPreambleMsb, (uint8)((strSX127x[ChipID].radio.flag.bits.preambleLen >> 8) & 0xFF));
    Drv_SX127x_SingleWrite(ChipID, SX127X_FSK_RegPreambleLsb, (uint8)(strSX127x[ChipID].radio.flag.bits.preambleLen & 0xFF));
}

//======================================
// Decrip: Select the PA_BOOST in PaSelect field of RegPaConfig.
//======================================
void Drv_SX127x_SetPABOOST(SX127x_CHIP_ENUM ChipID)
{
    uint8 reg_value = 0;
    if(ChipID > SX127x_CHIP_MAX)
        return  ;
    Drv_SX127x_SingleRead(ChipID, SX127X_PUB_RegPaConfig, &reg_value); // Read out its current value
    reg_value |= BIT7;
    Drv_SX127x_SingleWrite(ChipID, SX127X_PUB_RegPaConfig, reg_value); // Set new value
}

//======================================
// Decrip: Setup output power.
//======================================
void Drv_SX127x_SetOutPower(SX127x_CHIP_ENUM ChipID)
{
    if(ChipID > SX127x_CHIP_MAX)
        return  ;
    uint8 reg_paconfig = 0;
    uint8 reg_padac = 0;
    Drv_SX127x_SingleRead(ChipID, SX127X_PUB_RegPaConfig, &reg_paconfig); // Read out its current value
    // --- Setup PADAC ---
    if (reg_paconfig & BIT7)
    { // PA_BOOST
        if (strSX127x[ChipID].radio.send_power > 14)
        {
            reg_padac = 0x87;
        }
    }
    else
    { // RFO
        reg_padac = 0x84;
    }
    Drv_SX127x_SingleWrite(ChipID, SX127X_PUB_RegPaDac, reg_padac); // Set PA DAC
    // -------------------
    if (reg_paconfig & BIT7)
    { // PA_BOOST: output power is limited to +20dBm
        if (reg_padac == 0x87)
        { // Must in 5~20
            if (strSX127x[ChipID].radio.send_power < 5)
            {
                strSX127x[ChipID].radio.send_power = 5;
            }
            else if (strSX127x[ChipID].radio.send_power > 20)
            {
                strSX127x[ChipID].radio.send_power = 20;
            }
            reg_paconfig = ((reg_paconfig & 0x8F) | 0x70);                                                 // MaxPower
            reg_paconfig = ((reg_paconfig & 0xF0) | (uint8)((uint16)(strSX127x[ChipID].radio.send_power - 5) & 0x0F)); // OutputPower
        }
        else
        { // must in 2~17
            if (strSX127x[ChipID].radio.send_power < 2)
            {
                strSX127x[ChipID].radio.send_power = 2;
            }
            else if (strSX127x[ChipID].radio.send_power > 17)
            {
                strSX127x[ChipID].radio.send_power = 17;
            }
            reg_paconfig = ((reg_paconfig & 0x8F) | 0x70);                                                 // MaxPower
            reg_paconfig = ((reg_paconfig & 0xF0) | (uint8)((uint16)(strSX127x[ChipID].radio.send_power - 2) & 0x0F)); // OutputPower
        }
    }
    else
    { // RFO. output power is limited to +14dBm
        if (strSX127x[ChipID].radio.send_power < -1)
        {
            strSX127x[ChipID].radio.send_power = -1;
        }
        else if (strSX127x[ChipID].radio.send_power > 14)
        {
            strSX127x[ChipID].radio.send_power = 14;
        }
        reg_paconfig = ((reg_paconfig & 0x8F) | 0x70);                                                 // MaxPower
        reg_paconfig = ((reg_paconfig & 0xF0) | (uint8)((uint16)(strSX127x[ChipID].radio.send_power + 1) & 0x0F)); // OutputPower
    }
    
    Drv_SX127x_SingleWrite(ChipID, SX127X_PUB_RegPaConfig, reg_paconfig); // Setup power param
}

//======================================
// Decrip: Flush the RSSI and SNR value of SX127x.
// Input : isCurrentRSSI - true, read out current RSSI value(RegRssiValue), without SNR flush;
//     false, read out packet received RSSI(RegPktRssiValue) and SNR(RegPktSnrValue).
//======================================
void Drv_SX127x_LoRa_Flush_RSSIandSNR(SX127x_CHIP_ENUM ChipID, bool isCurrentRSSI)
{
    if(ChipID > SX127x_CHIP_MAX)
        return  ;
    uint8 reg_value = 0;
    if (isCurrentRSSI == true)
    { // Current RSSI (RegRssiValue)
        Drv_SX127x_SingleRead(ChipID, SX127X_LORA_RegRssiValue, &reg_value); // Readout RegRssiValue value
        if (strSX127x[ChipID].radio.freq < 860000000)
        { // LF
            strSX127x[ChipID].radio.RSSI = (sint16)(-155.0 + (float64)reg_value);
        }
        else
        { // HF
            strSX127x[ChipID].radio.RSSI = (sint16)(-150.0 + (float64)reg_value);
        }
    }
    else
    { // Receive PKT RSSI and SNR
        // SNR
        Drv_SX127x_SingleRead(ChipID, SX127X_LORA_RegPktSnrValue, &reg_value); // Readout RegPktSnrValue value
        if (reg_value & 0x80)
        { // the SNR sign bit is 1, Invert and divide by 4
            strSX127x[ChipID].radio.SNR = (((~reg_value + 1) & 0xFF) >> 2);
            strSX127x[ChipID].radio.SNR = -(strSX127x[ChipID].radio.SNR);
        }
        else
        { // Divide by 4
            strSX127x[ChipID].radio.SNR = ((reg_value & 0xFF) >> 2);
        }
        // RSSI
        if (strSX127x[ChipID].radio.freq < 860000000)
        { // LF
            if (strSX127x[ChipID].radio.SNR < 0)
            { // 
                strSX127x[ChipID].radio.RSSI = (sint16)(SX127X_NOISE_ABSOLUTE_ZERO + 10.0 * SignalBwLog[0] + 
                    SX127X_NOISE_FIGURE_LF + (float64)strSX127x[ChipID].radio.SNR);  
            }
            else
            {
                reg_value = 0;
                Drv_SX127x_SingleRead(ChipID, SX127X_LORA_RegPktRssiValue, &reg_value);   // Readout RegPktRssiValue value
                strSX127x[ChipID].radio.RSSI = (sint16)(RssiOffsetLF[0] + (float64)reg_value);  
            }
        }
        else
        { // HF
            if (strSX127x[ChipID].radio.SNR < 0)
            {
                strSX127x[ChipID].radio.RSSI = (sint16)(SX127X_NOISE_ABSOLUTE_ZERO + 10.0 * SignalBwLog[0] + 
                    SX127X_NOISE_FIGURE_HF + (float64)strSX127x[ChipID].radio.SNR);         
            }
            else
            {
                reg_value = 0;
                Drv_SX127x_SingleRead(ChipID, SX127X_LORA_RegPktRssiValue, &reg_value);   // Readout RegPktRssiValue value
                strSX127x[ChipID].radio.RSSI = (sint16)(RssiOffsetHF[0] + (float64)reg_value);
            }
        }
    }
}

//======================================
// Decrip: Flush the RSSI of SX127x in FSK mode.
//======================================
void Drv_SX127x_FSK_Flush_RSSI(SX127x_CHIP_ENUM ChipID)
{
    if(ChipID > SX127x_CHIP_MAX)
        return  ;
    uint8 reg_value = 0;
    Drv_SX127x_SingleRead(ChipID, SX127X_FSK_RegRssiValue, &reg_value); // Read out current RSSI reg value
    strSX127x[ChipID].radio.RSSI = -((sint16)reg_value >> 1); // Save new RSSI value
}

//======================================
// Decrip: Setup DIO_0 mapping.
//======================================
void Drv_SX127x_LoRa_SetMapping_DIO_0(SX127x_CHIP_ENUM ChipID, SX127X_DIO0_MAP mapType)
{
    if(ChipID > SX127x_CHIP_MAX)
        return  ;
    uint8 reg_value;
    reg_value = (mapType << 6);
    Drv_SX127x_SingleWrite(ChipID, SX127X_PUB_RegDioMapping1, reg_value);
}


//======================================
// Decrip: Setup DIO_0 mapping.
//======================================
void Drv_SX127x_LoRa_SetMapping_DIO_1(SX127x_CHIP_ENUM ChipID, SX127X_DIO0_MAP mapType)
{
    if(ChipID > SX127x_CHIP_MAX)
        return  ;
    uint8 reg_value;
    reg_value = (mapType << 6);
    Drv_SX127x_SingleWrite(ChipID, SX127X_PUB_RegDioMapping1, reg_value);
}

uint8 Drv_SX127x_Radio_SymTimeout(SX127x_CHIP_ENUM ChipID)
{
    if(ChipID > SX127x_CHIP_MAX) {
      return 0;
    }
    
    if (strSX127x[ChipID].flag.bits.rxContinous == 1) {
          return 0;
    }
    
    uint8 reg_value = 0;
    
    if (strSX127x[ChipID].radio.modulation == RADIO_MTYPE_LORA)
    { // LoRa modulation
        Drv_SX127x_SingleRead(ChipID, SX127X_LORA_RegIrqFlagsMask, &reg_value);
        Drv_SX127x_SingleRead(ChipID, SX127X_LORA_RegIrqFlags, &reg_value); // Read and clear IRQ flags

        if (reg_value & SX127X_RegIrqFlagsMask_RxTimeout) {
             //Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegIrqFlags, reg_value&(~SX127X_RegIrqFlagsMask_RxTimeout));
             return 1;
        }
    }
    else
    { // FSK modulation

    }  
        
    return 0;
}

//======================================
// Decrip: Pickout received data frame, after listen() success.
//         When this radio channel is RADIO_READ_ONLY or RADIO_READ_WRITE, listen() and read() are available;
//         when this radio channel is RADIO_WRITE_ONLY or RADIO_READ_WRITE, write() is available.
//======================================
uint8 Drv_SX127x_Radio_Read(SX127x_CHIP_ENUM ChipID, uint8 *buff, uint8 len)
{
    if(ChipID > SX127x_CHIP_MAX)
        return -1;
    uint8 reg_value = 0;
    if (strSX127x[ChipID].radio.modulation == RADIO_MTYPE_LORA)
    { // LoRa modulation
        Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegFifoAddrPtr, reg_value); // Fifo always start from 0
        Drv_SX127x_SingleRead(ChipID, SX127X_LORA_RegRxNbBytes, &reg_value);   // Readout rev data length
        if (reg_value > 0)
        { // Got received payload length
            Drv_SX127x_LoRa_Flush_RSSIandSNR(ChipID, false); // Flush receive packet RSSI and SNR
            if (reg_value <= len)
            {
                Drv_SX127x_BurstRead(ChipID, SX127X_PUB_RegFifo, buff, reg_value ); // Fetch rev data frame
                strSX127x[ChipID].radio.flag.bits.isRxAvail = 0;                                          // Clear flag
                Drv_SX127x_LoRa_Flush_RSSIandSNR(ChipID, false);                       // Flush received RSSI
                Drv_SX127x_LoRa_SetMode(ChipID, LORA_MODE_SLEEP);                      // Put in sleep mode 
                return reg_value;
            }
            else
            {
                return (uint8)(-2);      
            }
        }
        else
        { // No available rev data
            Drv_SX127x_LoRa_Flush_RSSIandSNR(ChipID, true);   // Flush current RSSI
            Drv_SX127x_LoRa_SetMode(ChipID, LORA_MODE_SLEEP); // Put in sleep mode
            strSX127x[ChipID].radio.flag.bits.isRxAvail = 0;
            return (uint8)(-2);    
        }
    }
    else
    { // FSK modulation
        Drv_SX127x_SingleRead(ChipID, SX127X_PUB_RegFifo, &reg_value); // Readout current FIFO length
        Drv_SX127x_FSK_Flush_RSSI(ChipID); // Flush current RSSI
        if (reg_value > 0)
        { // Got received payload length
            if (reg_value <= len)
            {
                Drv_SX127x_BurstRead(ChipID, SX127X_PUB_RegFifo, buff, reg_value ); // Fetch rev data frame
                strSX127x[ChipID].radio.flag.bits.isRxAvail = 0;                                          // Clear flag
                Drv_SX127x_FSK_SetMode(ChipID, FSK_MODE_SLEEP);                        // Set in sleep mode
                return reg_value;        
            }
            else
            {
#ifdef USE_DEBUG_ERR    
                Debug_Error("Radio_Read: the rev buff length(%d) is smaller than rev data frame length(%d) in FSK rev mode!\r\n", len - 1, reg_value);
#endif
#ifdef DEBUG 
                printk("Radio_Read: the rev buff length(%d) is smaller than rev data frame length(%d) in FSK rev mode!\r\n", len - 1, reg_value);
#endif 
                return (uint8)(-2);        
            }
        }
        else
        { // No available rev data
            strSX127x[ChipID].radio.flag.bits.isRxAvail = 0;                   // Clear flag
            Drv_SX127x_FSK_SetMode(ChipID, FSK_MODE_SLEEP); // Set in sleep mode
            return (uint8)(-2);      
        }
    }  
}

//======================================
// Decrip: Push data in send buffer.
//======================================
uint8 Drv_SX127x_Radio_Write(SX127x_CHIP_ENUM ChipID, const uint8 *buff, const uint8 len)
{
    if(ChipID > SX127x_CHIP_MAX)
        return -1;
    uint8 reg_value = 0;
    if (strSX127x[ChipID].radio.modulation == RADIO_MTYPE_LORA)
    { // LoRa modulation
        Drv_SX127x_LoRa_SetMode(ChipID, LORA_MODE_STDBY); // Put in standby mode
        // Tx payload length
        reg_value = len;
        Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegPayloadLength, reg_value); // Setup payload length
        // Full buffer used for Tx, start from 0
        reg_value = 0;
        Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegFifoTxBaseAddr, reg_value); 
        Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegFifoAddrPtr, reg_value);
        Drv_SX127x_BurstWrite(ChipID, SX127X_PUB_RegFifo, (uint8 *)buff, len); // Write send data
    }
    else
    { // FSK modulation
        Drv_SX127x_FSK_SetMode(ChipID, FSK_MODE_STDBY); // Set in standby mode
        Drv_SX127x_SingleWrite(ChipID, SX127X_PUB_RegFifo, len);         // Tx payload length
        Drv_SX127x_BurstWrite(ChipID, SX127X_PUB_RegFifo, (uint8 *)buff, len); // Write send data
    } 
    return len;  
}

//======================================
// Decrip: Send out data frame after use write() to finish data push.
//             NOTE! If not use sem_irq, this func will return immediately after trigger send,
//             That means when this func return, the chip will still in Tx mode.
//======================================
sint8 Drv_SX127x_Radio_Send(SX127x_CHIP_ENUM ChipID, uint32 time_dly)
{
    if(ChipID > SX127x_CHIP_MAX)
        return -1;
    uint8 reg_value = 0;
    uint8 err;
    
    if (strSX127x[ChipID].radio.modulation == RADIO_MTYPE_LORA)
    { // LoRa modulation
        Drv_SX127x_LoRa_SetMode(ChipID, LORA_MODE_STDBY); // Put in standby mode
        Drv_SX127x_LoRa_SetMapping_DIO_0(ChipID, DIO0_MAP_TxDone); // Enable DIO_0: Tx Done
        // --- IRQ Init ---
//         reg_value = SX127X_RegIrqFlagsMask_RxTimeout   |
        reg_value = 0   |
            SX127X_RegIrqFlagsMask_RxDone    |  
            SX127X_RegIrqFlagsMask_PayloadCrcError |
            SX127X_RegIrqFlagsMask_ValidHeader |
//             SX127X_RegIrqFlagsMask_TxDone      | // Enable
            SX127X_RegIrqFlagsMask_CadDone     |
            SX127X_RegIrqFlagsMask_FhssChange  |
            SX127X_RegIrqFlagsMask_CadDetected;
    Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegIrqFlags, 0xFF);
        Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegIrqFlagsMask, reg_value); // Setup IRQ mask    
//         Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegIrqFlagsMask, 0); // Setup IRQ mask    
//         Drv_SX127x_SingleRead(ChipID, SX127X_LORA_RegIrqFlags, &reg_value); // Read and clear IRQ flags
//         Drv_SX127x_Control(ChipID, SX127X_CONTROL_OPT_EN_DIO0_INT); // Enable DIO_0 interrupt
        Drv_SX127x_LoRa_SetMode(ChipID, LORA_MODE_TX); // Put in TX mode
#if 0
        if (strSX127x[ChipID].radio.flag.bits.isPend == 1)
        { // Pend mode
            // --- Wait for DIO_0 interrupt handler to sendout radio_event_sem ---
            err = Drv_SX127x_WAIT_DIO0ms(ChipID, time_dly);
//             Drv_SX127x_Control(ChipID, SX127X_CONTROL_OPT_DIS_DIO0_INT);                 // Disable DIO_0 interrupt, when we finish sem_irq pend
            Drv_SX127x_SingleRead(ChipID, SX127X_LORA_RegIrqFlags, &reg_value); // Read and clear IRQ flags
            Drv_SX127x_LoRa_SetMode(ChipID, LORA_MODE_SLEEP);                   // Put in sleep mode
            if (err == 1)
            { // Timeout
#ifdef USE_DEBUG_ERR    
                Debug_Error("Radio_Send: cannot sendout frame!\r\n");
#endif
#ifdef DEBUG 
                printk("Radio_Send: cannot sendout frame!\r\n");
#endif 
                SX127x_IOCtl_IRQClear();
                return (-2);
            }      
        }
#endif
    }
    else
    { // FSK modulation    
        Drv_SX127x_Control(ChipID, SX127X_CONTROL_OPT_EN_DIO0_INT); // Enable DIO_0 interrupt
        Drv_SX127x_FSK_SetMode(ChipID, FSK_MODE_TX);       // Set in Tx mode
        if (strSX127x[ChipID].radio.flag.bits.isPend == 1)
        { // Pend mode
            // --- Wait for DIO_0 interrupt handler to sendout radio_event_sem ---
            err = Drv_SX127x_WAIT_DIO0ms(ChipID, time_dly);
            Drv_SX127x_Control(ChipID, SX127X_CONTROL_OPT_DIS_DIO0_INT); // Disable DIO_0 interrupt, when we finish sem_irq pend
            Drv_SX127x_SingleRead(ChipID, SX127X_FSK_RegIrqFlags1, &reg_value); // Read and clear IRQ flags 1
            Drv_SX127x_SingleRead(ChipID, SX127X_FSK_RegIrqFlags2, &reg_value); // Read and clear IRQ flags 2
            Drv_SX127x_FSK_SetMode(ChipID, FSK_MODE_SLEEP); // Set in sleep mode 
            if (err == 1)
            { // Timeout
#ifdef USE_DEBUG_ERR    
                Debug_Error("Radio_Send: cannot sendout frame in FSK mode!\r\n");
#endif
#ifdef DEBUG 
                printk("Radio_Send: cannot sendout frame in FSK mode!\r\n");
#endif 
                SX127x_IOCtl_IRQClear();
                return (-2);
            }      
        }
    }
    
    SX127x_IOCtl_IRQClear();
    return 1;  
}

sint8 Drv_SX127x_Radio_DioIntHandle(SX127x_CHIP_ENUM ChipID, uint8_t *len)
{
    sint8 ret = 0;

    if(ChipID > SX127x_CHIP_MAX)
        return -1;
    
    uint8 reg_value = 0;

    if (strSX127x[ChipID].radio.modulation == RADIO_MTYPE_LORA)
    { // LoRa modulation
        Drv_SX127x_LoRa_SetMode(ChipID, LORA_MODE_STDBY); // Put in standby mode

        Drv_SX127x_SingleRead(ChipID, SX127X_LORA_RegIrqFlags, &reg_value); // Read and clear IRQ flags

        if (reg_value & (SX127X_RegIrqFlagsMask_RxTimeout|SX127X_RegIrqFlagsMask_PayloadCrcError)) {
             ret = 0;
             *len = 0;
        }
        else if (reg_value & SX127X_RegIrqFlagsMask_RxDone) {
             ret = 1;
             *len = Drv_SX127x_Radio_Read(ChipID, strSX127x[ChipID].buff, 255);
        }
        else if (reg_value & SX127X_RegIrqFlagsMask_TxDone) {
             ret = 2;
        }
        
        Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegIrqFlags, 0xFF); /* clear all interrupt */

        Drv_SX127x_LoRa_SetMode(ChipID, LORA_MODE_SLEEP); // Put in sleep mode

        return ret;
    }
    else
    { // FSK modulation    

    }

    SX127x_IOCtl_IRQClear();

    return ret;  
}

//======================================
// Decrip: Use this func to listen current channel (flush RSSI and SNR or receive data).
//             NOTE! This func is only available when this channel is readable.
//======================================
sint8 Drv_SX127x_Radio_Listen(SX127x_CHIP_ENUM ChipID, uint32 timeout, bool isRev)
{
    if(ChipID > SX127x_CHIP_MAX)
            return -1;
    uint8 reg_value = 0;
    uint8 err;
    
    if (strSX127x[ChipID].radio.modulation == RADIO_MTYPE_LORA)
    { // LoRa 
        Drv_SX127x_LoRa_SetMode(ChipID, LORA_MODE_STDBY); // Set in standby mode 
        Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegMaxPayloadLength, strSX127x[ChipID].size);
        if (isRev == true)
        { // Receive data frame    
            // --- Enable DIO_0 interrupt ---
            Drv_SX127x_LoRa_SetMapping_DIO_0(ChipID, DIO0_MAP_RxDone); // Enable DIO_0: Rx Done
            Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegIrqFlags, 0xFF);
            if (strSX127x[ChipID].flag.bits.rxContinous == 0) {
                // --- IRQ init ---
              //             reg_value = SX127X_RegIrqFlagsMask_RxTimeout   |
                  reg_value = 0   |
              //                 SX127X_RegIrqFlagsMask_RxDone    |  // Enable
                      SX127X_RegIrqFlagsMask_PayloadCrcError |
                      SX127X_RegIrqFlagsMask_ValidHeader |
                      SX127X_RegIrqFlagsMask_TxDone      |
                      SX127X_RegIrqFlagsMask_CadDone     |
                      SX127X_RegIrqFlagsMask_FhssChange  |
                      SX127X_RegIrqFlagsMask_CadDetected; 
                  Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegIrqFlagsMask, reg_value); // Setup IRQ mask
                //Drv_SX127x_SingleRead(ChipID, SX127X_LORA_RegIrqFlags, &reg_value); // Read and clear IRQ flags  
          //             Drv_SX127x_Control(ChipID, SX127X_CONTROL_OPT_EN_DIO0_INT); // Enable DIO_0 interrupt    
                // ------------------------------
                  Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegFifoAddrPtr, 0); // FIFO always start from 0    
                  Drv_SX127x_LoRa_SetMode(ChipID, LORA_MODE_RXSINGLE);           // Start to receive data frame
            }
            else {
                reg_value = 0   |
          //                 SX127X_RegIrqFlagsMask_RxDone    |  // Enable
                  SX127X_RegIrqFlagsMask_RxTimeout   |
                  SX127X_RegIrqFlagsMask_PayloadCrcError |
                  SX127X_RegIrqFlagsMask_ValidHeader |
                  SX127X_RegIrqFlagsMask_TxDone      |
                  SX127X_RegIrqFlagsMask_CadDone     |
                  SX127X_RegIrqFlagsMask_FhssChange  |
                  SX127X_RegIrqFlagsMask_CadDetected; 
                Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegIrqFlagsMask, reg_value); // Setup IRQ mask
                //Drv_SX127x_SingleRead(ChipID, SX127X_LORA_RegIrqFlags, &reg_value); // Read and clear IRQ flags  
          //             Drv_SX127x_Control(ChipID, SX127X_CONTROL_OPT_EN_DIO0_INT); // Enable DIO_0 interrupt    
                // ------------------------------
                Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegFifoAddrPtr, 0); // FIFO always start from 0    
                Drv_SX127x_LoRa_SetMode(ChipID, LORA_MODE_RXCONTINUOUS);           // Start to receive data frame
            }
        }
        else
        { // Flush rx channel param
                Drv_SX127x_LoRa_SetMapping_DIO_0(ChipID, DIO0_MAP_RxDone);
                Drv_SX127x_LoRa_SetMapping_DIO_0(ChipID, DIO0_MAP_RxDone);
                Drv_SX127x_LoRa_SetMode(ChipID, LORA_MODE_RXSINGLE); // Start to receive data frame
                Drv_SX127x_LoRa_Flush_RSSIandSNR(ChipID, true);   // Flush current RSSI
                Drv_SX127x_LoRa_SetMode(ChipID, LORA_MODE_SLEEP); // Put in sleep mode       
        }
    } // if (strSX127x[ChipID].flag.bits.modulation == RADIO_MTYPE_LORA)
    else
    { // FSK 
            Drv_SX127x_FSK_SetMode(ChipID, FSK_MODE_STDBY); // Set in standby mode
            if (isRev == true)
            { // Receive data frame
                    Drv_SX127x_Control(ChipID, SX127X_CONTROL_OPT_EN_DIO0_INT); // Enable DIO_0 interrupt 
                    Drv_SX127x_FSK_SetMode(ChipID, FSK_MODE_RX);       // Set in Rx mode
                    if (strSX127x[ChipID].radio.flag.bits.isPend == 1)
                    {
                            err = Drv_SX127x_WAIT_DIO0ms(ChipID, timeout);
                            Drv_SX127x_Control(ChipID, SX127X_CONTROL_OPT_DIS_DIO0_INT); // Disable DIO_0 interrupt, when we finish radio_event_sem pend  
                            Drv_SX127x_FSK_Flush_RSSI(ChipID); // Flush current RSSI
                            if (err == 1)
                            { // Timeout, no data receive
                                    Drv_SX127x_FSK_SetMode(ChipID, FSK_MODE_SLEEP); // Set in sleep mode
                                    return 0;
                            }
                            else
                            { // Got DIO_0 int event which means data frame received           
                                    strSX127x[ChipID].radio.flag.bits.isRxAvail = 1; // Setup RX avail flag(we can fetch rev data out)
#ifdef LORA_DEBUG
#ifdef DEBUG    
                                    printk("Radio_Listen: receive one frame in FSK mode!\r\n");
#endif 
#endif // LORA_DEBUG 
                            }        
                    }      
            }
            else
            { // Flush rx channel param
                    Drv_SX127x_FSK_SetMode(ChipID, FSK_MODE_RX); // Set in Rx mode
                    if (timeout < 1000)
                    {
                            Drv_SX127x_WAIT_MS(1); // Wait at least 1ms
                    }
                    else
                    {
                            Drv_SX127x_WAIT_MS(timeout / 1000);
                    }
                    Drv_SX127x_FSK_Flush_RSSI(ChipID);              // Flush current RSSI  
                    Drv_SX127x_FSK_SetMode(ChipID, FSK_MODE_SLEEP); // Set in sleep mode      
            }
    }
    return 1;
}

void Drv_SX127x_Radio_ReadWideRssi(SX127x_CHIP_ENUM ChipID)
{
    uint8_t reg_value;
    Drv_SX127x_SingleRead(ChipID, SX127X_LORA_RegRssiWideband, &reg_value);
    strSX127x[ChipID].radio.RSSI=-reg_value;
}


//======================================
//======================================
// Decrip: Use this func to control RF chip, like open, close, setup param, check status. See opt define
//======================================
sint8 Drv_SX127x_Radio_Control(SX127x_CHIP_ENUM ChipID, uint32 opt)
{
    if(ChipID > SX127x_CHIP_MAX)
        return -1;
    uint8 reg_value = 0;
//     struct sx127x *ChipID = (struct sx127x *)priv;
    if (opt == RADIO_CONTROL_OPT_CHECK_RX)
    { // Check if radio has finish receive one data frame (after use listen() in non-pend mode)
        if (strSX127x[ChipID].radio.flag.bits.state != RADIO_STATE_RX)
        {
#ifdef USE_DEBUG_ERR    
            Debug_Error("Radio_Control: radio state(%d) is not RX!\r\n", strSX127x[ChipID].flag.bits.state);
#endif
#ifdef DEBUG 
            printk("Radio_Control: radio state(%d) is not RX!\r\n", strSX127x[ChipID].flag.bits.state);
#endif 
            return 0;      
        }
#if 0
        if (OSSemAccept(strSX127x[ChipID].sem_irq) > 0)
        { // DIO_0 interrupt has post the radio_event_sem 
            Drv_SX127x_Control(ChipID, SX127X_CONTROL_OPT_DIS_DIO0_INT); // Disable DIO_0 interrupt 
            if (strSX127x[ChipID].modulation == RADIO_MTYPE_LORA)
            { // LoRa
                Drv_SX127x_SingleRead(ChipID, SX127X_LORA_RegIrqFlags, &reg_value); // Read and clear IRQ flags      

                if (reg_value & SX127X_RegIrqFlagsMask_PayloadCrcError)
                { // CRC payload error, drop the frame
                    Drv_SX127x_LoRa_Flush_RSSIandSNR(ChipID, false); // Flush receive packet RSSI and SNR
                    Drv_SX127x_LoRa_SetMode(ChipID, LORA_MODE_SLEEP); // Put in sleep mode
#ifdef LORA_DEBUG
#ifdef DEBUG    
                    printk("Radio_Control: CRC Payload Error, drop the received frame!\r\n");
#endif 
#endif // LORA_DEBUG
                    return 0;
                }
                else
                { // Receive one data frame
                    strSX127x[ChipID].flag.bits.isRxAvail = 1; // Setup RX avail flag(we can fetch rev data out)
#ifdef LORA_DEBUG
#ifdef DEBUG   
                    printk("Radio_Control: receive one frame in LoRa mode!\r\n");
#endif 
#endif // LORA_DEBUG        
                }        
            }
            else
            { // FSK
                Drv_SX127x_FSK_Flush_RSSI(ChipID); // Flush current RSSI
                strSX127x[ChipID].flag.bits.isRxAvail = 1; // Setup RX avail flag(we can fetch rev data out)

#ifdef LORA_DEBUG
#ifdef DEBUG    
                printk("Radio_Control: receive one frame in FSK mode!\r\n");
#endif 
#endif // LORA_DEBUG        
            }
        }
        else
        { // strSX127x[ChipID].sem_irq not eccur
#ifdef LORA_DEBUG
#ifdef DEBUG   
            printk("Radio_Listen: strSX127x[ChipID].sem_irq not eccur!\r\n");
#endif 
#endif // LORA_DEBUG       
            return 0;
        }
#endif
    }
    else if (opt == RADIO_CONTROL_OPT_CHECK_TX)
    { // Check if radio has finish send one data frame (after use send() in non-pend mode)
        if (strSX127x[ChipID].radio.flag.bits.state != RADIO_STATE_TX)
        {
#ifdef USE_DEBUG_ERR    
            Debug_Error("Radio_Control: radio state(%d) not in TX!\r\n", strSX127x[ChipID].flag.bits.state);
#endif
#ifdef DEBUG 
            printk("Radio_Control: radio state(%d) not in TX!\r\n", strSX127x[ChipID].flag.bits.state);
#endif 
            return 0;      
        }
#if 0
        if (OSSemAccept(strSX127x[ChipID].sem_irq) > 0)
        { // DIO_0 interrupt has post the radio_event_sem      
            Drv_SX127x_Control(ChipID, SX127X_CONTROL_OPT_DIS_DIO0_INT); // Disable DIO_0 interrupt

            if (strSX127x[ChipID].modulation == RADIO_MTYPE_LORA)
            { // LoRa
                Drv_SX127x_SingleRead(ChipID, SX127X_LORA_RegIrqFlags, &reg_value); // Read and clear IRQ flags
                Drv_SX127x_LoRa_SetMode(ChipID, LORA_MODE_SLEEP);                   // Put in sleep mode        
            }
            else
            { // FSK
                Drv_SX127x_SingleRead(ChipID, SX127X_FSK_RegIrqFlags1, &reg_value); // Read and clear IRQ flags 1
                Drv_SX127x_SingleRead(ChipID, SX127X_FSK_RegIrqFlags2, &reg_value); // Read and clear IRQ flags 2      
                Drv_SX127x_FSK_SetMode(ChipID, FSK_MODE_SLEEP);                     // Set in sleep mode         
            }
        }
        else
        { // strSX127x[ChipID].sem_irq not occur
            return 0;
        }    
#endif
    }
    else if (opt == RADIO_CONTROL_OPT_RESET)
    { // Reset
        Drv_SX127x_Control(ChipID, SX127X_CONTROL_OPT_RESET);
        if (strSX127x[ChipID].radio.modulation == RADIO_MTYPE_LORA)
        { // LoRa modulation
            Drv_SX127x_LoRa_SetMode(ChipID, LORA_MODE_SLEEP); // Set in sleep mode
        }
        else
        { // FSK modulation
            Drv_SX127x_FSK_SetMode(ChipID, FSK_MODE_SLEEP); // Set in sleep mode
        }
    }
    else if (opt == RADIO_CONTROL_OPT_OFF)
    { // Off
        Drv_SX127x_Control(ChipID, SX127X_CONTROL_OPT_RX); // Switch to RX

        if (strSX127x[ChipID].radio.modulation == RADIO_MTYPE_LORA)
        { // LoRa modulation
            Drv_SX127x_LoRa_SetMode(ChipID, LORA_MODE_SLEEP); // Set in sleep mode
        }
        else
        { // FSK modulation
            Drv_SX127x_FSK_SetMode(ChipID, FSK_MODE_SLEEP); // Set in sleep mode 
        }
    }
    else if (opt == RADIO_CONTROL_OPT_ON)
    { // On
        if (strSX127x[ChipID].radio.modulation == RADIO_MTYPE_LORA)
        { // LoRa modulation
            Drv_SX127x_LoRa_SetMode(ChipID, LORA_MODE_STDBY); // Set in standby mode
        }
        else
        { // FSK modulation
            Drv_SX127x_FSK_SetMode(ChipID, FSK_MODE_STDBY); // Set in standby mode
        }    
    }
    else if (opt == RADIO_CONTROL_OPT_CALIBRAT)
    { // Calibrate
        // ??
    }
    else if (opt == RADIO_CONTROL_OPT_SET_MOD)
    { // Modulation
        Drv_SX127x_SetModulation(ChipID);
    }
    else if (opt == RADIO_CONTROL_OPT_SET_BANDW)
    { // Set bandwidth
        if (strSX127x[ChipID].radio.modulation == RADIO_MTYPE_LORA)
        { // LoRa
            Drv_SX127x_LoRa_SetBandwidth(ChipID); 
        }
        else
        { // FSK
            Drv_SX127x_FSK_SetFdev(ChipID);        // Setup Fdev
            Drv_SX127x_FSK_SetRxBandwidth(ChipID); // Setup Rx bandwidth
        }
    }
    else if (opt == RADIO_CONTROL_OPT_SET_FREQ)
    { // Freq
        Drv_SX127x_SetFreq(ChipID);
    }
    else if (opt == RADIO_CONTROL_OPT_SET_BITRATE)
    { // Bitrate or SF
        if (strSX127x[ChipID].radio.modulation == RADIO_MTYPE_LORA)
        { // LoRa
            Drv_SX127x_LoRa_SetSF(ChipID); // Set SF
        }
        else
        { // FSK
            Drv_SX127x_FSK_SetFdev(ChipID);    // Setup Fdev
            Drv_SX127x_FSK_SetBitrate(ChipID); // Setup bitrate
        }    
    }
    else if (opt == RADIO_CONTROL_OPT_SET_POWER)
    { // Power
        Drv_SX127x_SetOutPower(ChipID); 
    }
    else if (opt == RADIO_CONTROL_OPT_SET_PARAM)
    { // Setup whole param
        Drv_SX127x_SetModulation(ChipID); // Setup modulation
#ifdef RF_USE_TCXO  
        Drv_SX127x_SingleWrite(ChipID, SX127X_PUB_RegTcxo, 0x09 | BIT4); // Use TCXO
#endif // RF_USE_TCXO     
        Drv_SX127x_SetFreq(ChipID);       // Setup freq
        Drv_SX127x_SetOutPower(ChipID);   // Setup output power
        if (strSX127x[ChipID].radio.modulation == RADIO_MTYPE_LORA)
        { // LoRa
            Drv_SX127x_LoRa_SetMode(ChipID, LORA_MODE_STDBY); // Set in standby mode
            // --- Channel param --- 
            Drv_SX127x_LoRa_SetSF(ChipID);                  // Set default SF
            Drv_SX127x_LoRa_SetCodeRate(ChipID);            // Set coderate
            Drv_SX127x_LoRa_SetCRC(ChipID);                 // Set CRC on or off
            Drv_SX127x_LoRa_SetBandwidth(ChipID);           // Set bandwidth
            Drv_SX127x_LoRa_SetSymbTimeout(ChipID, strSX127x[ChipID].radio.symtimeout);  // Set RX Symble Time-out
            Drv_SX127x_LoRa_SetPreambleLength(ChipID);      // Set default preamble length
                        Drv_SX127x_LoRa_SetSyncWord(ChipID);            // Set Syncword
                        Drv_SX127x_LoRa_SetInvertIQ(ChipID);            // Set InvertIQ
            // --------------------- 
        }
        else
        { // FSK
            Drv_SX127x_FSK_SetMode(ChipID, FSK_MODE_STDBY); // Set in standby mode
            // --- Channel param ---
            Drv_SX127x_FSK_SetDefaultParam(ChipID);   // First set default param
            Drv_SX127x_FSK_SetBitrate(ChipID);        // Set bitrate
            Drv_SX127x_FSK_SetFdev(ChipID);           // Set Fdev
            Drv_SX127x_FSK_SetCRC(ChipID);            // Set CRC
            Drv_SX127x_FSK_SetRxBandwidth(ChipID);    // Set Rx bandwidth
            Drv_SX127x_FSK_SetPreambleLength(ChipID); // Set preamble length    
        }
        // Output power
        Drv_SX127x_SetPABOOST(ChipID);  // Select PA_BOOST pin
        Drv_SX127x_SetOutPower(ChipID); // Set output power
        // --- 2015-12-27-Season: Close OCP and enable LNA ---
        // Disable OCP(over current protection)
        Drv_SX127x_SingleRead(ChipID, SX127X_PUB_RegOcp, &reg_value); // Read out current value
        reg_value &= (~BIT5);
        Drv_SX127x_SingleWrite(ChipID, SX127X_PUB_RegOcp, reg_value); // Write new value
        if (strSX127x[ChipID].radio.modulation == RADIO_MTYPE_LORA)
        {
            Drv_SX127x_SingleWrite(ChipID, SX127X_PUB_RegLna, 0x23); // Enable LNA
        }
        else
        {
            Drv_SX127x_SingleWrite(ChipID, SX127X_PUB_RegLna, 0x20); // Enable LNA
        }    
    }
    else if (opt == RADIO_CONTROL_OPT_READ_TEMP)
    { // Read tempreture
        // ??
    }
    else if ((opt == RADIO_CONTROL_OPT_LED_ON) || (opt == RADIO_CONTROL_OPT_LED_OFF))
    { // LED indication
#if 0
        if (strSX127x[ChipID].flag.bits.led == 1)
        { // LED available
            if (opt == RADIO_CONTROL_OPT_LED_ON)
            { // LED on
                strSX127x[ChipID].control(SX127X_CONTROL_OPT_LED_ON);
            }
            else
            { // LED off
                strSX127x[ChipID].control(SX127X_CONTROL_OPT_LED_OFF);
            }
        }
        else
        {     
#ifdef DEBUG    
            printk("Radio_Control: LED indication not available!\r\n");
#endif
            return 0;
        }
#endif
    }
    else
    {
#ifdef USE_DEBUG_ERR    
        Debug_Error("Radio_Control: opt(%d) not supported!\r\n", opt);
#endif     
#ifdef DEBUG    
        printk("Radio_Control: opt(%d) not supported!\r\n", opt);
#endif
        return (-2);    
    }   
    return 1;    
}
// -------------------------

//======================================
// Decrip: Check if input freq value is fitable for its freq_band.
//======================================
sint8 Drv_SX127x_Radio_CheckFreqBand(uint16 freq_band, uint32 freq)
{
    if (freq_band == RADIO_FREQ_BAND_433MHZ)
    { // Support 430MHz ~ 434MHz
        if ((freq >= RADIO_FREQ_433MHZ_MIN) && (freq <= RADIO_FREQ_433MHZ_MAX))
        {
            return 1;
        }
    }
    else if (freq_band == RADIO_FREQ_BAND_470MHZ)
    { // Support China 470MHz ~ 510MHz
        if ((freq >= RADIO_FREQ_470MHZ_MIN) && (freq <= RADIO_FREQ_470MHZ_MAX))
        {
            return 1;
        }    
    }
    else if (freq_band == RADIO_FREQ_BAND_868MHZ)
    { // Support 863MHz ~ 870MHz
        if ((freq >= RADIO_FREQ_868MHZ_MIN) && (freq <= RADIO_FREQ_868MHZ_MAX))
        {
            return 1;
        }    
    }
    else if (freq_band == RADIO_FREQ_BAND_915MHZ)
    { // Support U.S ISM 902MH ~ 928MHz
        if ((freq >= RADIO_FREQ_915MHZ_MIN) && (freq <= RADIO_FREQ_915MHZ_MAX))
        {
            return 1;
        }    
    } 
    else if (freq_band == RADIO_FREQ_BAND_169MHZ)
    { // Support Europe 169.400MHz ~ 169.475MHz
        if ((freq >= RADIO_FREQ_169MHZ_MIN) && (freq <= RADIO_FREQ_169MHZ_MAX))
        {
            return 1;
        }    
    }  
    return (-2);
}




#if DRV_SX127x_TEST_EN
#define TEST_BUFF_LEN        (100)
#include <stdlib.h>
//======================================
// RadioFIFO测试
//======================================
void Drv_SX127x_Test_RadioFIFO(SX127x_CHIP_ENUM ChipID)
{
    uint8 test_WriteBuff[TEST_BUFF_LEN]; // write buff
    uint8 test_ReadBuff[TEST_BUFF_LEN];  // read buff
    if(ChipID > SX127x_CHIP_MAX)
        return  ;
    if (strSX127x[ChipID].radio.modulation != RADIO_MTYPE_LORA)
    { // Set modulation
        strSX127x[ChipID].radio.modulation = RADIO_MTYPE_LORA;
        Drv_SX127x_SetModulation(ChipID);
    }
    uint32 i,j;
    uint8 reg_value;
    Drv_SX127x_LoRa_SetMode(ChipID, LORA_MODE_STDBY); // set in standby mode
    // Create random value
    for (i = 0; i < TEST_BUFF_LEN; i++)
    {
        test_WriteBuff[i] = (rand() & 0xFF);
        test_ReadBuff[i] = 0;
    }
    // Write data
    reg_value = 0;
    Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegFifoAddrPtr, reg_value); // start addr
    Drv_SX127x_BurstWrite(ChipID, SX127X_PUB_RegFifo, test_WriteBuff, TEST_BUFF_LEN); // burst write payload
    // Read data
    reg_value = 0;
    Drv_SX127x_SingleWrite(ChipID, SX127X_LORA_RegFifoAddrPtr, reg_value); // start addr
    Drv_SX127x_BurstRead(ChipID, SX127X_PUB_RegFifo, test_ReadBuff, TEST_BUFF_LEN); // read out data
    // Compare
    for (i = 0,j=0; i < TEST_BUFF_LEN; i++)
    {
        if (test_ReadBuff[i] != test_WriteBuff[i])
        {//有错误
            j ++;
        }
    }
    while(j)
    {
        ;
    }
}

//======================================
#endif    //DRV_SX127x_TEST_EN





//======================================
// 
//======================================
// void Drv_SX127x_(SX127x_CHIP_ENUM ChipID)
// {
//     if(ChipID > SX127x_CHIP_MAX)
//         return -1;
// }

//======================================
// 
//======================================
// void Drv_SX127x_(SX127x_CHIP_ENUM ChipID)
// {
//     if(ChipID > SX127x_CHIP_MAX)
//         return -1;
// }


//======================================
// 
//======================================
// void Drv_SX127x_(SX127x_CHIP_ENUM ChipID)
// {
//     if(ChipID > SX127x_CHIP_MAX)
//         return -1;
// }





//======================================
//======================================




