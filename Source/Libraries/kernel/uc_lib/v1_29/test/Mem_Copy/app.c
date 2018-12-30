/******************************* 文件信息 **************************************
--- 文件名：  app.c
--- 创建人：  许珍
--- 创建日期：2013年01月09日
--- 说明：    应用层接口 
--- 当前版本：V1.0.0
--- 版本确定日期：
--------------------- 修订记录 ------------------------------
--- 第一次修订(时间：修订人：修订后版本)：
。。。。
*******************************************************************************/
/****
包含的头文件
****/ 
#include "bsp.h"
//#include "sys_led.h"
#include "sys_log.h"
#include "my_sys.h" 
#include "emFile.h"
#include "run_speed.h"
#include "lib_mem.h"
#include "app.h"

#ifdef NET_LWIP
#include "lwip/tcpip.h"

#endif // NET_LWIP

// --- module ---
#include "module_OSMonitor.h"
#include "module_net_tcpecho.h"
#include "module_net_httpserver.h" 
/****
局部宏定义
****/
// --- sys param ---
const char sys_name[] = "AT91SAM9X25EK_EmbededSys";
// version
#define SYS_MAIN_VERSION    (1)
#define SYS_SUB1_VERSION    (0)
#define SYS_SUB2_VERSION    (0)

/****
局部变量
****/
// --- Task_Start ---
static OS_STK TaskStk_Start[APP_CFG_TASK_START_STK_SIZE];

// --- Task_Main ---
#define  APP_CFG_TASK_MAIN_STK_SIZE                 500   
static OS_STK TaskStk_Main[APP_CFG_TASK_MAIN_STK_SIZE];

// --- Module_Node ---
struct sList_node SysMNode_Monitor;
struct sList_node SysMNode_NetTcpEcho;
struct sList_node SysMNode_NetHttpServer;
// -------------------
/****
全局变量
****/


/****
局部函数
****/
/*==============================================================================
函数名称: Task_Main
功能描述: 系统主任务        
输  入:  
输  出:  无
调  用:  
作　者:  许珍
日　期:  2013年06月03日
------------------------------ 修订记录 ------------------------------------- 
修改记录: 

==============================================================================*/
static void Task_Main(void *p_arg)
{
  (void)p_arg;
  uint32 i;
  struct sList_node *pNode;
  struct module *pModule; 
  
  //SBoot_PrintBootLogo();
  
  // --- 启动设置的相关模块 ---
  for (i = 0; i < my_sys.module_slist.num; i++)
  {
    pNode = SList_PickN(&(my_sys.module_slist), i + 1);
    pModule = (struct module *)pNode->node;
    (pModule->pStart)(pModule); // 使能该模块功能运行
  }   
  
  while (1)
  {
    OSTimeDly(OS_TICKS_PER_SEC);
  }
}

/*==============================================================================
函数名称: Sys_Start
功能描述: 系统启动使能
输  入:  priv - my_sys
输  出:  无
调  用:  Sys_Init()
作　者:  许珍
日　期:  2013年01月23日
------------------------------ 修订记录 ------------------------------------- 
修改记录: 

==============================================================================*/
static int Sys_Start(struct sys *priv)
{
#if (OS_TASK_NAME_EN > 0)
  CPU_INT08U  err;
#endif
  
  // --- 启动主任务 ---
  OSTaskCreateExt((void (*)(void *)) Task_Main,                                           
                  (void           *) 0,
                  (OS_STK         *)&TaskStk_Main[APP_CFG_TASK_MAIN_STK_SIZE - 1],
                  (INT8U           ) SYS_MAIN_TASK_PRIO,
                  (INT16U          ) SYS_MAIN_TASK_PRIO,
                  (OS_STK         *)&TaskStk_Main[0],
                  (INT32U          ) APP_CFG_TASK_MAIN_STK_SIZE,
                  (void           *) 0,
                  (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
  
#if (OS_TASK_NAME_EN > 0)
  OSTaskNameSet(SYS_MAIN_TASK_PRIO, "Task_Main", &err);
#endif
 
  return 1;
}

/*==============================================================================
函数名称: Sys_Init
功能描述: 系统初始化函数
          初始化my_sys对象
输  入:  
输  出:  无
调  用:  
作　者:  许珍
日　期:  2013年01月23日
------------------------------ 修订记录 ------------------------------------- 
修改记录: 

==============================================================================*/
void Sys_Init(void)
{
  SYS_VERSION tmp_ver; // 本系统版本
  SYS_FLAG tmp_flag;
  // --- clear ---
  Sys_ConfigParam(&my_sys, NULL, SYS_CONFPARAM_OPT_CLEAR);  
  
  // --- Name ---
  Sys_ConfigParam(&my_sys, (void *)sys_name, SYS_CONFPARAM_OPT_NAME);
  
  // --- version ---
  tmp_ver.version = 0;
  tmp_ver.bits.main = SYS_MAIN_VERSION;
  tmp_ver.bits.sub1 = SYS_SUB1_VERSION;
  tmp_ver.bits.sub2 = SYS_SUB2_VERSION;
  Sys_ConfigParam(&my_sys, &tmp_ver, SYS_CONFPARAM_OPT_VARSION);
  
  // --- 参数初始化 ---
  /*
  Sys_Param_Init(&(my_sys.param)); // 初始化sys_param
  Sys_Param_ConfigParam(&(my_sys.param), (void *)&sboot_param, SYS_PARAM_CONFPARAM_OPT_PRIV); // 挂载sboot_param
  Sys_Param_ConfigParam(&(my_sys.param), (void *)SBoot_Param_Init, SYS_PARAM_CONFPARAM_OPT_PINIT); // 挂载init函数指针
  Sys_Param_ConfigParam(&(my_sys.param), (void *)SBoot_Param_Set, SYS_PARAM_CONFPARAM_OPT_PSET); // 挂载set函数指针
  Sys_Param_ConfigParam(&(my_sys.param), (void *)SBoot_Param_Read, SYS_PARAM_CONFPARAM_OPT_PREAD); // 挂载read函数指针
  (my_sys.param.pInit)(&(my_sys.param)); // 初始化, 获取参数  
  */
  
  // --- to do: 添加module模块(先完成初始化后再进行加载) ---
  Module_OSMonitor_Init(); // 初始化
  SysMNode_Monitor.node = (void *)&m_OSMonitor; // 添加模块节点
  SysMNode_Monitor.next = NULL;
  Sys_ConfigParam(&my_sys, (void *)(&SysMNode_Monitor), SYS_CONFPARAM_OPT_MODULE); // 添加Module模块
  // --- Net ---
  // TCP Echo
  Module_Net_TcpEcho_Init();
  SysMNode_NetTcpEcho.node = (void *)&m_net_tcpecho; // 添加模块节点
  SysMNode_NetTcpEcho.next = NULL;
  Sys_ConfigParam(&my_sys, (void *)(&SysMNode_NetTcpEcho), SYS_CONFPARAM_OPT_MODULE); // 添加Module模块
  // Http Server
  Module_Net_HttpServer_Init();
  SysMNode_NetHttpServer.node = (void *)&m_net_httpserver; // 添加模块节点
  SysMNode_NetHttpServer.next = NULL;
  Sys_ConfigParam(&my_sys, (void *)(&SysMNode_NetHttpServer), SYS_CONFPARAM_OPT_MODULE); // 添加Module模块  

  // --- to do: 添加hwDev模块(先完成初始化后再进行加载) ---
  // ??  
  
  // --- 添加start函数指针 ---
  Sys_ConfigParam(&my_sys, (void *)Sys_Start, SYS_CONFPARAM_OPT_PSTART);
  
  // --- flag ---
  tmp_flag.flag = 0;
  tmp_flag.bits.init = 1; // 完成初始化
  tmp_flag.bits.run = 1; // 系统开始运行
  tmp_flag.bits.use_os = 1; // 使用操作系统
  tmp_flag.bits.sys_led = 1; // 使用系统灯
#ifdef USE_DEBUG_ERR  
  tmp_flag.bits.debug_err = 1; // 使用debug_err
#endif
  tmp_flag.bits.use_cli = 1; // 使用cli模块  
  Sys_ConfigParam(&my_sys, &tmp_flag, SYS_CONFPARAM_OPT_FLAG);
  
  // --- 调用start函数 ---
  if (my_sys.pSys_start != NULL)
  {
    my_sys.pSys_start(&my_sys); // 运行start函数
  }
  else
  {
    Debug_Error("Sys_Init: pSys_start is null!\n");
#ifdef DEBUG
    printk("Sys_Init: pSys_start is null!\n");
#endif    
  }
}

/*==============================================================================
函数名称: Task_Start
功能描述: 系统起始任务
          本任务优先级最高, 运行系统灯, 表明系统正在运行.
输  入:  
输  出:  无
调  用:  
作　者:  许珍
日　期:  2013年01月09日
------------------------------ 修订记录 ------------------------------------- 
修改记录: 

==============================================================================*/
// mem_cpy测试 
#define MEMT_DATALEN   (1000000)
uint8 memT_src[MEMT_DATALEN] = {0};
uint8 memT_Des1[MEMT_DATALEN] = {0};
uint8 memT_Des2[MEMT_DATALEN] = {0};
uint32 memT_time1[10] = {0};
uint32 memT_time2[10] = {0};
uint8 memT_flag = 0;
uint32 counter1;
static void Task_Start(void *p_arg)
{
  (void)p_arg;
  
#ifdef DEBUG
#ifdef DEBUG_L1  
  printk("Task_Start: run in!\n");
#endif // DEBUG_L1     
#endif // DEBUG
  // --- 第一步初始化 ---
  Mem_Init(); // 内存池初始化
#ifdef USE_DEBUG_ERR    
  Debug_Err_Init(&debug_err_info); // debug_err初始化
#endif
#ifdef USE_GIO  
  IO_DEV_Init(); // 初始化通用IO设备链表
#endif // USE_GIO
  BSP_Init(); // 底层驱动初始化
  OS_CPU_InitExceptVect(); // Initialize the vector table
  BSP_OS_TmrTickInit(OS_TICKS_PER_SEC); // Initialize the uC/OS-II ticker
  Console_Open(); // 打开console
  RunSpeed_DefaultInit(&sys_RunSpeed); // run_speed初始化
  Sys_Log_Init(&sys_log_info); // 初始化sys_log
  //emFile_Init(); // 初始化文件系统
  // --- 网络初始化 ---
#ifdef NET_LWIP  
  //tcpip_init(NULL, NULL); // 初始化lwip协议栈
#endif 
  //Sys_Init(); // 系统(my_sys)初始化
  //BSP_Init2(); // BSP高级设备初始化  
  // --------------------  
#if (OS_TASK_STAT_EN > 0)
  OSStatInit(); // Determine CPU capacity                 
#endif
  
  // --- mem_cpy测试 ---
  //uint32 i, j;
  for (counter1 = 0; counter1 < MEMT_DATALEN; counter1++)
  {
    memT_src[counter1] = counter1 + 1;
  }
  
  for (counter1 = 0; counter1 < 10; counter1++)
  {
    sys_RunSpeed.run_start(&sys_RunSpeed);
    memcpy(memT_Des1, memT_src, MEMT_DATALEN);
    memT_time1[counter1] = sys_RunSpeed.run_end(&sys_RunSpeed);
    
    sys_RunSpeed.run_start(&sys_RunSpeed);
    Mem_Copy(memT_Des2, memT_src, MEMT_DATALEN);
    memT_time2[counter1] = sys_RunSpeed.run_end(&sys_RunSpeed);
  }
  
  
  // ===================
  
  while (1)
  {
    LED_D1_OFF();
    OSTimeDly(OS_TICKS_PER_SEC);
    LED_D1_ON();
    OSTimeDly(OS_TICKS_PER_SEC);
  }
}
/****
全局函数
****/
/*==============================================================================
函数名称: App_Init
功能描述: 系统初始化
输  入:  无
输  出:  无
调  用:  main()
作　者:  许珍
日　期:  2013年01月09日
------------------------------ 修订记录 ------------------------------------- 
修改记录: 

==============================================================================*/ 
void App_Init(void)
{
#if (OS_TASK_NAME_EN > 0)
  CPU_INT08U  err;
#endif
  
  BSP_WDT_Disable(); // 禁能片内看门狗
  
  BSP_AIC_DisableAll(); // Disable all interrupts until we are ready to accept them
  
  OSInit(); // 初始化操作系统
  
  // --- Create the start task --- 
  OSTaskCreateExt((void (*)(void *)) Task_Start,                                           
                  (void           *) 0,
                  (OS_STK         *)&TaskStk_Start[APP_CFG_TASK_START_STK_SIZE - 1],
                  (INT8U           ) APP_CFG_TASK_START_PRIO,
                  (INT16U          ) APP_CFG_TASK_START_PRIO,
                  (OS_STK         *)&TaskStk_Start[0],
                  (INT32U          ) APP_CFG_TASK_START_STK_SIZE,
                  (void           *) 0,
                  (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
  
#if (OS_TASK_NAME_EN > 0)
  OSTaskNameSet(APP_CFG_TASK_START_PRIO, "Task_Start", &err);
#endif 

  OSStart(); // Start multitaskin  
}