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
#include "sys_led.h"  
#include "my_sys.h" 
#include "emFile.h"
#include "run_speed.h"
#include "app.h"
#include "lib_mem.h"
// --- module ---
#include "module_net.h" 
#include "module_SerialCli.h" 
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

// --- Module_Node ---
struct sList_node SysMNode_SCLI;
// -------------------
/****
全局变量
****/


/****
局部函数
****/
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
  uint32 i;
  struct sList_node *pNode;
  struct module *pModule; 
  
#ifdef ARG_CHECK
  if (priv == NULL)
  {
    Debug_Error("Sys_Start: my_sys cannot clear!\n");
#ifdef DEBUG
    printk("Sys_Start: my_sys cannot clear!\n");
#endif 
    return (-EPERM);    
  } 
#endif

  // --- 启动设置的相关模块 ---
  for (i = 0; i < priv->module_slist.num; i++)
  {
    pNode = SList_PickN(&(priv->module_slist), i + 1);
    pModule = (struct module *)pNode->node;
    
#ifdef ARG_CHECK
    if (pModule == NULL)
    {
      Debug_Error("Sys_Start: pModule(No.%d) cannot be NULL!\n", i);
    #ifdef DEBUG
      printk("Sys_Start: pModule(No.%d) cannot be NULL!\n", i);
    #endif
      return (-EPERM);      
    }
#endif
    
    (pModule->pStart)(pModule); // 使能该模块功能运行
  }
  
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
  int result;
  SYS_VERSION tmp_ver; // 本系统版本
  SYS_FLAG tmp_flag;
  // --- clear ---
  result = Sys_ConfigParam(&my_sys, NULL, SYS_CONFPARAM_OPT_CLEAR);
#ifdef ARG_CHECK
  if (result < 0)
  {
    Debug_Error("Sys_Init: my_sys cannot clear!\n");
#ifdef DEBUG
    printk("Sys_Init: my_sys cannot clear!\n");
#endif
    return;     
  }
#endif  
  
  // --- Name ---
  result = Sys_ConfigParam(&my_sys, (void *)sys_name, SYS_CONFPARAM_OPT_NAME);
#ifdef ARG_CHECK
  if (result < 0)
  {
    Debug_Error("Sys_Init: cannot set sys_name!\n");
#ifdef DEBUG
    printk("Sys_Init: cannot set sys_name!\n");
#endif
    return;     
  }
#endif 
  
  // --- version ---
  tmp_ver.version = 0;
  tmp_ver.bits.main = SYS_MAIN_VERSION;
  tmp_ver.bits.sub1 = SYS_SUB1_VERSION;
  tmp_ver.bits.sub2 = SYS_SUB2_VERSION;
  result = Sys_ConfigParam(&my_sys, &tmp_ver, SYS_CONFPARAM_OPT_VARSION);
#ifdef ARG_CHECK
  if (result < 0)
  {
    Debug_Error("Sys_Init: cannot set version!\n");
#ifdef DEBUG
    printk("Sys_Init: cannot set version!\n");
#endif
    return;     
  }
#endif
  
  // --- to do: 添加module模块(先完成初始化后再进行加载) ---
  // 串口CLI
  Module_SerialCLI_Init(); // 初始化
  SysMNode_SCLI.node = (void *)(&m_SerialCli);
  SysMNode_SCLI.next = NULL;
  Sys_ConfigParam(&my_sys, (void *)(&SysMNode_SCLI), SYS_CONFPARAM_OPT_MODULE); // 添加Module模块
  
  // --- to do: 添加hwDev模块(先完成初始化后再进行加载) ---
  // ??  
  
  // --- 添加start函数指针 ---
  result = Sys_ConfigParam(&my_sys, (void *)Sys_Start, SYS_CONFPARAM_OPT_PSTART);
#ifdef ARG_CHECK
  if (result < 0)
  {
    Debug_Error("Sys_Init: cannot set Sys_Start!\n");
#ifdef DEBUG
    printk("Sys_Init: cannot set Sys_Start!\n");
#endif
    return;     
  }
#endif
  
  // --- flag ---
  tmp_flag.flag = 0;
  tmp_flag.bits.init = 1; // 完成初始化
  tmp_flag.bits.run = 1; // 系统开始运行
  tmp_flag.bits.use_os = 1; // 使用操作系统
  tmp_flag.bits.sys_led = 1; // 使用系统灯
  tmp_flag.bits.debug_err = 1; // 使用debug_err
  tmp_flag.bits.use_cli = 1; // 使用cli模块  
  result = Sys_ConfigParam(&my_sys, &tmp_flag, SYS_CONFPARAM_OPT_FLAG);
#ifdef ARG_CHECK
  if (result < 0)
  {
    Debug_Error("Sys_Init: cannot set flag!\n");
#ifdef DEBUG
    printk("Sys_Init: cannot set flag!\n");
#endif
    return;     
  }
#endif
  
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
    return;     
  }
  
  result = result; // 防止编译器告警
}

// -------------------------- Lib_Mem -----------------------------------


MEM_POOL MemPool;
static void Task_LibMemTest(void *p_arg)
{
  (void)p_arg;
  CPU_SIZE_T octets_reqd;
  LIB_ERR err;
  void *pmem_blk[10];
  uint8 i;
  uint8 j;
  
  // --- 创建内存池 ---
  Mem_PoolCreate((MEM_POOL   *)&MemPool,
                 (void       *)  0,       // 从Heap中创建内存池
                 (CPU_SIZE_T  )  0,       
                 (CPU_SIZE_T  ) 10,       // 内存片个数
                 (CPU_SIZE_T  )100,       // 每个内存片宽度
                 (CPU_SIZE_T  )  4,       // 端对齐
                 (CPU_SIZE_T *)&octets_reqd,
                 (LIB_ERR    *)&err);
  
  if (err != LIB_MEM_ERR_NONE)
  { // 内存池分配错误
    printk("Lib_Mem_Test: Cannot create memory pool!\n");
    if (err == LIB_MEM_ERR_HEAP_EMPTY)
    {
      printk("Lib_Mem_Test: Heap empty, need %d more octets!\n", octets_reqd);
    }
  }
  
  /*
  // --- 获取内存池 ---
  pmem_blk = Mem_PoolBlkGet((MEM_POOL *)&MemPool,
                            (CPU_SIZE_T)100,
                            (LIB_ERR  *)&err);
  if (err != LIB_MEM_ERR_NONE)
  {
    printk("Lib_Mem_Test: Cannot get memory block!\n");
  }*/
  OSTimeDly(OS_TICKS_PER_SEC);
  printk("\n");
  
  while (1)
  {
    for (j = 0; j < 100; j++)
    {
      printk("------------------- No%d Test ----------------\n", j + 1);
      for (i = 0; i < 10; i++)
      {
        pmem_blk[i] = Mem_PoolBlkGet((MEM_POOL *)&MemPool,
                                     (CPU_SIZE_T)100,
                                     (LIB_ERR  *)&err);
        if (err != LIB_MEM_ERR_NONE)
        {
          printk("Cannot get No%d memory block!\n", i + 1);
        }
        else
        {
          printk("success get No%d memory block, Base_Addr: 0x%x!\n", i + 1, pmem_blk[i]);
          //OSTimeDly(OS_TICKS_PER_SEC/2);
        }
      } 
      //OSTimeDly(OS_TICKS_PER_SEC/2);
      for (i = 0; i < 10; i++)
      {
        Mem_PoolBlkFree((MEM_POOL *)&MemPool,
                        (void     *)pmem_blk[i],
                        (LIB_ERR  *)&err);
        if (err != LIB_MEM_ERR_NONE)
        {
          printk("Cannot free No%d Mem block!\n", i + 1);
        }
      }
      //OSTimeDly(OS_TICKS_PER_SEC);      
    } 
    
    while (1)
    {
      OSTimeDly(OS_TICKS_PER_SEC/2); 
    }
  }
}

#define LIBMEM_TEST_TASK_PRIO                35
// --- Task ---
#define  APP_CFG_TASK_LIBMEM_STK_SIZE                 256   
static OS_STK TaskStk_LibMem[APP_CFG_TASK_LIBMEM_STK_SIZE];
void Lib_Mem_Test(void)
{
  INT8U err; 
  
  // --- 创建文件系统任务 ---
  OSTaskCreateExt((void (*)(void *)) Task_LibMemTest,                                           
                  (void           *) 0,
                  (OS_STK         *)&TaskStk_LibMem[APP_CFG_TASK_LIBMEM_STK_SIZE - 1],
                  (INT8U           ) LIBMEM_TEST_TASK_PRIO,
                  (INT16U          ) LIBMEM_TEST_TASK_PRIO,
                  (OS_STK         *)&TaskStk_LibMem[0],
                  (INT32U          ) APP_CFG_TASK_LIBMEM_STK_SIZE,
                  (void           *) 0,
                  (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
  
#if (OS_TASK_NAME_EN > 0)
  OSTaskNameSet(LIBMEM_TEST_TASK_PRIO, "Task_LibMem", &err);
#endif   
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
static void Task_Start(void *p_arg)
{
  (void)p_arg;
  
#ifdef DEBUG
#ifdef DEBUG_L1  
    printk("Task_Start: run in!\n");
#endif // DEBUG_L1     
#endif // DEBUG
  // --- 第一步初始化 ---
#ifdef USE_DEBUG_ERR    
  Debug_Err_Init(&debug_ErrInfo); // debug_err初始化
#endif
#ifdef USE_GIO  
  IO_DEV_Init(); // 初始化通用IO设备链表
#endif // USE_GIO  
  BSP_Init(); // 底层驱动初始化
  OS_CPU_InitExceptVect(); // Initialize the vector table
  BSP_OS_TmrTickInit(OS_TICKS_PER_SEC); // Initialize the uC/OS-II ticker
  SYS_LED_Init(); // 初始化LED
  Console_Open(); // 打开console
  printk("\n------ SBoot ------\n");   
  RunSpeed_DefaultInit(&sys_RunSpeed); // run_speed初始化
  // Board_NandInit(); // ??初始化nand芯片
  Mem_Init(); // 内存池初始化
  emFile_Init(); // 初始化文件系统
  //BSP_Init2(); // BSP高级设备初始化
  //Module_Net_Init(); 
  
  Sys_Init(); // 系统(my_sys)初始化
  
  Lib_Mem_Test();
  
  // --------------------  
#if (OS_TASK_STAT_EN > 0)
  OSStatInit(); // Determine CPU capacity                 
#endif
  
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