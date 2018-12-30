/******************************* �ļ���Ϣ **************************************
--- �ļ�����  app.c
--- �����ˣ�  ����
--- �������ڣ�2014��12��22��
--- ˵����    ģ���ļ�
--- ��ǰ�汾��V1.0.0
--- �汾ȷ�����ڣ�
--------------------- �޶���¼ ------------------------------
--- ��һ���޶�(ʱ�䣺�޶��ˣ��޶���汾)��
��������
*******************************************************************************/
/****
������ͷ�ļ�
****/
#include "app.h" 
#include "lib_mem.h"
#include "bsp.h"

#include "ucos_ii.h"
#include "app_cfg.h"
#include "cpu.h"
#include <stdio.h>

/****
�ֲ��궨��
****/

/****
�ֲ�����
****/
// --- Task_Start ---
static OS_STK TaskStk_Start[APP_CFG_TASK_START_STK_SIZE];

/****
ȫ�ֱ���
****/

/****
�ֲ�����
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
  
  // --- �����ڴ�� ---
  Mem_PoolCreate((MEM_POOL   *)&MemPool,
                 (void       *)  0,       // ��Heap�д����ڴ��
                 (CPU_SIZE_T  )  0,       
                 (CPU_SIZE_T  ) 10,       // �ڴ�Ƭ����
                 (CPU_SIZE_T  )100,       // ÿ���ڴ�Ƭ���
                 (CPU_SIZE_T  )  4,       // �˶���
                 (CPU_SIZE_T *)&octets_reqd,
                 (LIB_ERR    *)&err);
  
  if (err != LIB_MEM_ERR_NONE)
  { // �ڴ�ط������
    printf("Lib_Mem_Test: Cannot create memory pool!\n");
    if (err == LIB_MEM_ERR_HEAP_EMPTY)
    {
      printf("Lib_Mem_Test: Heap empty, need %d more octets!\n", octets_reqd);
    }
  }
  
  /*
  // --- ��ȡ�ڴ�� ---
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
  
  // --- �����ļ�ϵͳ���� ---
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
��������: Task_Start
��������: ϵͳ��ʼ����
          ���������ȼ����, ����ϵͳ��, ����ϵͳ��������.
��  ��:  
��  ��:  ��
��  ��:  
������:  ����
�ա���:  2014��12��22��
------------------------------ �޶���¼ ------------------------------------- 
�޸ļ�¼: 

==============================================================================*/
static void Task_Start(void *p_arg)
{
  (void)p_arg;
  
  // --- ��ʼ�� ---
  Mem_Init(); // �ڴ�س�ʼ��
  
  OS_CPU_SysTickInit(DEFAULT_SYSTEM_CLOCK/(uint32)OS_TICKS_PER_SEC); // ��ʼ������ϵͳSystem Tick
  
  BSP_Init(); // �ײ�������ʼ�� 
  
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
ȫ�ֺ���
****/
/*==============================================================================
��������: App_Init
��������: ��ʼ���ڲ����Ź�
��  ��:  
��  ��:  ��
��  ��:  app.c
������:  ����
�ա���:  2014��12��22��
------------------------------ �޶���¼ ------------------------------------- 
�޸ļ�¼: 

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