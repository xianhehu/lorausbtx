/******************************* 文件信息 **************************************
--- 文件名：  app.c
--- 创建人：  许珍
--- 创建日期：2014年12月22日
--- 说明：    模板文件
--- 当前版本：V1.0.0
--- 版本确定日期：
--------------------- 修订记录 ------------------------------
--- 第一次修订(时间：修订人：修订后版本)：
。。。。
*******************************************************************************/
/****
包含的头文件
****/
#include "app.h" 
#include "lib_mem.h"
#include "bsp.h"

#include "ucos_ii.h"
#include "app_cfg.h"
#include "cpu.h"
#include <stdio.h>

/****
局部宏定义
****/

/****
局部变量
****/
// --- Task_Start ---
static OS_STK TaskStk_Start[APP_CFG_TASK_START_STK_SIZE];

/****
全局变量
****/

/****
局部函数
****/

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
    printf("Lib_Mem_Test: Cannot create memory pool!\n");
    if (err == LIB_MEM_ERR_HEAP_EMPTY)
    {
      printf("Lib_Mem_Test: Heap empty, need %d more octets!\n", octets_reqd);
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
  printf("\n");
  
  while (1)
  {
    for (j = 0; j < 100; j++)
    {
      printf("------------------- No%d Test ----------------\n", j + 1);
      for (i = 0; i < 10; i++)
      {
        pmem_blk[i] = Mem_PoolBlkGet((MEM_POOL *)&MemPool,
                                     (CPU_SIZE_T)100,
                                     (LIB_ERR  *)&err);
        if (err != LIB_MEM_ERR_NONE)
        {
          printf("Cannot get No%d memory block!\n", i + 1);
        }
        else
        {
          printf("success get No%d memory block, Base_Addr: 0x%x!\n", i + 1, pmem_blk[i]);
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
          printf("Cannot free No%d Mem block!\n", i + 1);
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
日　期:  2014年12月22日
------------------------------ 修订记录 ------------------------------------- 
修改记录: 

==============================================================================*/
static void Task_Start(void *p_arg)
{
  (void)p_arg;
  
  // --- 初始化 ---
  Mem_Init(); // 内存池初始化
  
  OS_CPU_SysTickInit(DEFAULT_SYSTEM_CLOCK/(uint32)OS_TICKS_PER_SEC); // 初始化操作系统System Tick
  
  BSP_Init(); // 底层驱动初始化 
  
  Lib_Mem_Test();
  
  while (1)
  {
    RGB_GREEN_ON();
    OSTimeDly(OS_TICKS_PER_SEC);
    RGB_DARK(); // Turn off RGB LED
    OSTimeDly(OS_TICKS_PER_SEC);
  }
}
/****
全局函数
****/
/*==============================================================================
函数名称: App_Init
功能描述: 初始化内部看门狗
输  入:  
输  出:  无
调  用:  app.c
作　者:  许珍
日　期:  2014年12月22日
------------------------------ 修订记录 ------------------------------------- 
修改记录: 

==============================================================================*/ 
void App_Init(void)
{
#if (OS_TASK_NAME_EN > 0)
  CPU_INT08U  err;
#endif

  OSInit();

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
  
  OSStart(); 
}