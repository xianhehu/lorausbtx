// #include "Drv_Usart.h"
#include "Drv_Usart_QShell.h"
#include "stdio.h"
#include "Q_Shell.h"

unsigned char  Var1;
unsigned short Var2;
unsigned int   Var3;

QSH_VAR_REG(Var1,"unsigned char  Var1","u8");
QSH_VAR_REG(Var2,"unsigned char  Var2","u16");
QSH_VAR_REG(Var3,"unsigned char  Var3","u32");

unsigned int Add(unsigned a,unsigned b)
{
	return (a+b);
}

QSH_FUN_REG(Add, "unsigned int Add(unsigned a,unsigned b)");

unsigned int PutString(char *Str)
{
	if(*Str=='\0')
		return 0;
	printf("%s\r\n",Str);
	return 1;
}

QSH_FUN_REG(PutString, "unsigned int PutString(char *Str)");

//==============================================================================
#define USART_BUF_MAX    256
uint8 usart_buf[PORT_MAX][USART_BUF_MAX];//串口缓存定义
uint8 qshellbuf[UART_BUF_LEN];
//==============================================================================

int main(void)
{
	Var1=1;
	Var2=2;
	Var3=3;
	//======================================
#if DRV_USART_USED1
	Drv_Usart_Init(usart_buf[PORT_USART1], USART_BUF_MAX, USART_BUF_MAX, PORT_USART1, 115200, USART_Parity_No);
// 	Drv_Usart_Write("Usart1,115200,n,8,1\n", 20, PORT_USART1);
#endif  //DRV_USART_USED1
	//======================================
	printf("Usart1,115200,n,8,1\n");
	//======================================
	QShell_Init( qshellbuf, UART_BUF_LEN);
	//======================================
// 	QSH_VAR_REG(Var1,"unsigned char  Var1","u8");
// 	QSH_VAR_REG(Var2,"unsigned char  Var2","u16");
// 	QSH_VAR_REG(Var3,"unsigned char  Var3","u32");
	//======================================
	while(1)
	{
		QShell_OnTimerScan();
	}
}

#if 1
//======================================
//SysTick中断
//======================================
void On_SysTick(void)
{
	//--------------------------
// 	RunStatFlg &= ~RSF_TIM_TICK;
// 	cntsec ++;
	//--------------------------
}
#endif



