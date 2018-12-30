#include "sx127x.h"
#include "stm32f10x.h"
#include "app.h"
#include "os.h"

#define SX127X_DIO_NUM 4

const  uint8_t       spicspins[]  = {4}; //Æ¬Ñ¡
const  uint8_t       resetpins[]  = {2}; //¸´Î»½Å
const  uint16_t      dioxpins[][SX127X_DIO_NUM]= {{3, 10, 1, 0}};
const  uint8_t       dioexints[][SX127X_DIO_NUM] = {{GPIO_PortSourceGPIOA, GPIO_PortSourceGPIOB, 
                                                     GPIO_PortSourceGPIOB, GPIO_PortSourceGPIOB}};
const  uint8_t       dionvic[][SX127X_DIO_NUM]= {{EXTI3_IRQn, EXTI15_10_IRQn, EXTI1_IRQn, EXTI0_IRQn}};
static GPIO_TypeDef* spicsports[] = {GPIOA};
static GPIO_TypeDef* resetports[] = {GPIOA};
static SPI_TypeDef*  spiports[]   = {SPI1};
static GPIO_TypeDef* dioxports[][SX127X_DIO_NUM]= {{GPIOA, GPIOB, GPIOB, GPIOB}};

static uint8_t  intdio[] = {0};

void SPI1_IOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /*!< sFLASH_SPI_CS_GPIO, sFLASH_SPI_MOSI_GPIO, sFLASH_SPI_MISO_GPIO 
       and sFLASH_SPI_SCK_GPIO Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    /*!< Configure sFLASH_SPI pins: SCK */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*!< Configure sFLASH_SPI pins: MOSI */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*!< Configure sFLASH_SPI pins: MISO */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*!< Configure sFLASH_CS_PIN pin: sFLASH Card CS pin */
    GPIO_InitStructure.GPIO_Pin = 1 << spicspins[0];
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(spicsports[0], &GPIO_InitStructure);
}

void SPI1_SetCS(uint8_t ch, bool high)
{
    if (high)
        GPIO_SetBits(spicsports[ch], 1 << spicspins[ch]);
    else
        GPIO_ResetBits(spicsports[ch], 1 << spicspins[ch]);
}

void SPI1_Init()
{
    SPI_InitTypeDef  SPI_InitStructure;

    SPI1_IOInit();

    /*!< Deselect the FLASH: Chip Select high */
    SPI1_SetCS(0, true);

    /*!< sFLASH_SPI Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    /*!< SPI configuration */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;

    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    
    SPI_Init(spiports[0], &SPI_InitStructure);

    /*!< Enable the sFLASH_SPI  */
    SPI_Cmd(spiports[0], ENABLE);
}

void SX127X_InitIOReset(uint8_t ch)
{
    /* reset */
    GPIO_InitTypeDef GPIO_InitStructure;

    /*!< sFLASH_SPI_CS_GPIO, sFLASH_SPI_MOSI_GPIO, sFLASH_SPI_MISO_GPIO 
       and sFLASH_SPI_SCK_GPIO Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    /*!< Configure sFLASH_SPI pins: SCK */
    GPIO_InitStructure.GPIO_Pin = 1 << resetpins[ch];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(resetports[ch], &GPIO_InitStructure);
}

void SX127X_IOInit(uint8_t ch)
{
    SX127X_InitIOReset(ch);
    
    /* dio */
    for (int i = 0; i < SX127X_DIO_NUM; i++) {
        GPIO_InitTypeDef GPIO_InitStructure;
        
        GPIO_InitStructure.GPIO_Pin = 1 << dioxpins[ch][i];
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
        GPIO_Init(dioxports[ch][i], &GPIO_InitStructure);
        
        GPIO_EXTILineConfig(dioexints[ch][i], dioxpins[ch][i]);
    }
    
    /* dio int */
    for (int i = 0; i < SX127X_DIO_NUM; i++) {
        EXTI_InitTypeDef EXTI_InitStructure;

        EXTI_ClearITPendingBit(1 << dioxpins[ch][i]);
        EXTI_InitStructure.EXTI_Line = 1 << dioxpins[ch][i];
        EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;
        EXTI_Init(&EXTI_InitStructure);
    }
    
    /* 2 bit for pre-emption priority, 2 bits for subpriority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    for (int i = 0; i < SX127X_DIO_NUM; i++) {
        NVIC_InitTypeDef NVIC_InitStructure;
        
        NVIC_InitStructure.NVIC_IRQChannel = dionvic[ch][i];
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        
        NVIC_Init(&NVIC_InitStructure);
    }
}

uint8_t SX127X_SendByte(uint8_t ch, uint8_t byte)
{
    /*!< Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(spiports[ch], SPI_I2S_FLAG_TXE) == RESET);

    /*!< Send byte through the SPI1 peripheral */
    SPI_I2S_SendData(spiports[ch], byte);

    /*!< Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(spiports[ch], SPI_I2S_FLAG_RXNE) == RESET);

    /*!< Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(spiports[ch]);
}

int SX127X_Write(uint8_t ch, uint8_t addr, uint8_t* buf, uint8_t size)
{
    if (ch >= sizeof(spiports)/sizeof(spiports[0]))
        return -1;
    
    SPI1_SetCS(ch, false);
    
    SX127X_SendByte(ch, addr | 0x80);
    
    for (int i = 0; i < size; i++)
    {
        SX127X_SendByte(ch, buf[i]);
    }
    
    SPI1_SetCS(ch, true);
    
    return 0;
}

int SX127X_Read(uint8_t ch, uint8_t addr, uint8_t* buf, uint8_t size)
{
    if (ch >= sizeof(spiports)/sizeof(spiports[0]))
        return -1;
    
    SPI1_SetCS(ch, false);
    
    SX127X_SendByte(ch, addr);
    
    for (int i = 0; i < size; i++)
    {
         buf[i] = SX127X_SendByte(ch, 0);
    }
    
    SPI1_SetCS(ch, true);
    
    return 0;
}

uint8_t SX127X_GetIntDio(uint8_t ch)
{
    return intdio[ch];
}

void SX127X_DioIntHandle(uint8_t pin)
{
    bool valid = false;
    
    for (int ch = 0; ch < 1; ch++)
    {
        for (int i = 0; i < SX127X_DIO_NUM; i++) {
            if (dioxpins[ch][i] != pin)
                continue;
            
            intdio[ch] = i;
            valid = true;
        }
    }

    if (!valid)
        return;
    
    os_post(OS_SEM_SX127X_ISR);
}

void SX127X_EnableInt(uint8_t ch)
{
    os_initevent(OS_SEM_SX127X_ISR, 0);
}

int SX127X_Wait(uint8_t ch, uint32_t timeout)
{
    return os_wait(OS_SEM_SX127X_ISR, timeout);
}

void SX127X_Init(void)
{
    SPI1_Init();
    SX127X_IOInit(0);
    os_initevent(OS_SEM_SX127X_ISR, 0);
    
    uint8_t data = 0;
    SX127X_Reset(0);
    SX127X_Read(0, 1, &data, 1);
    
    while(data != 0) {
        data = 0;
        SX127X_Write(0, 1, &data, 1);
        os_delay(10);
        SX127X_Read(0, 1, &data, 1);
    }
}

void SX127X_Reset(uint8_t ch)
{
    GPIO_ResetBits(resetports[ch], 1 << resetpins[ch]);
    os_delay(10);
    GPIO_SetBits(resetports[ch], 1 << resetpins[ch]);
    os_delay(100);
}
