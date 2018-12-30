/******************************* �ļ���Ϣ **************************************
--- �ļ�����  app.c
--- �����ˣ�  ����
--- �������ڣ�2013��01��09��
--- ˵����    Ӧ�ò�ӿ� 
--- ��ǰ�汾��V1.0.0
--- �汾ȷ�����ڣ�
--------------------- �޶���¼ ------------------------------
--- ��һ���޶�(ʱ�䣺�޶��ˣ��޶���汾)��
��������
*******************************************************************************/
/****
������ͷ�ļ�
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
�ֲ��궨��
****/
// --- sys param ---
const char sys_name[] = "AT91SAM9X25EK_EmbededSys";
// version
#define SYS_MAIN_VERSION    (1)
#define SYS_SUB1_VERSION    (0)
#define SYS_SUB2_VERSION    (0)

/****
�ֲ�����
****/
// --- Task_Start ---
static OS_STK TaskStk_Start[APP_CFG_TASK_START_STK_SIZE];

// --- Module_Node ---
struct sList_node SysMNode_SCLI;
// -------------------
/****
ȫ�ֱ���
****/


/****
�ֲ�����
****/
/*==============================================================================
��������: Sys_Start
��������: ϵͳ����ʹ��
��  ��:  priv - my_sys
��  ��:  ��
��  ��:  Sys_Init()
������:  ����
�ա���:  2013��01��23��
------------------------------ �޶���¼ ------------------------------------- 
�޸ļ�¼: 

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

  // --- �������õ����ģ�� ---
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
    
    (pModule->pStart)(pModule); // ʹ�ܸ�ģ�鹦������
  }
  
  return 1;
}

/*==============================================================================
��������: Sys_Init
��������: ϵͳ��ʼ������
          ��ʼ��my_sys����
��  ��:  
��  ��:  ��
��  ��:  
������:  ����
�ա���:  2013��01��23��
------------------------------ �޶���¼ ------------------------------------- 
�޸ļ�¼: 

==============================================================================*/
void Sys_Init(void)
{
  int result;
  SYS_VERSION tmp_ver; // ��ϵͳ�汾
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
  
  // --- to do: ���moduleģ��(����ɳ�ʼ�����ٽ��м���) ---
  // ����CLI
  Module_SerialCLI_Init(); // ��ʼ��
  SysMNode_SCLI.node = (void *)(&m_SerialCli);
  SysMNode_SCLI.next = NULL;
  Sys_ConfigParam(&my_sys, (void *)(&SysMNode_SCLI), SYS_CONFPARAM_OPT_MODULE); // ���Moduleģ��
  
  // --- to do: ���hwDevģ��(����ɳ�ʼ�����ٽ��м���) ---
  // ??  
  
  // --- ���start����ָ�� ---
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
  tmp_flag.bits.init = 1; // ��ɳ�ʼ��
  tmp_flag.bits.run = 1; // ϵͳ��ʼ����
  tmp_flag.bits.use_os = 1; // ʹ�ò���ϵͳ
  tmp_flag.bits.sys_led = 1; // ʹ��ϵͳ��
  tmp_flag.bits.debug_err = 1; // ʹ��debug_err
  tmp_flag.bits.use_cli = 1; // ʹ��cliģ��  
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
  
  // --- ����start���� ---
  if (my_sys.pSys_start != NULL)
  {
    my_sys.pSys_start(&my_sys); // ����start����
  }
  else
  {
    Debug_Error("Sys_Init: pSys_start is null!\n");
#ifdef DEBUG
    printk("Sys_Init: pSys_start is null!\n");
#endif
    return;     
  }
  
  result = result; // ��ֹ�������澯
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
    printk("Lib_Mem_Test: Cannot create memory pool!\n");
    if (err == LIB_MEM_ERR_HEAP_EMPTY)
    {
      printk("Lib_Mem_Test: Heap empty, need %d more octets!\n", octets_reqd);
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
�ա���:  2013��01��09��
------------------------------ �޶���¼ ------------------------------------- 
�޸ļ�¼: 

==============================================================================*/
static void Task_Start(void *p_arg)
{
  (void)p_arg;
  
#ifdef DEBUG
#ifdef DEBUG_L1  
    printk("Task_Start: run in!\n");
#endif // DEBUG_L1     
#endif // DEBUG
  // --- ��һ����ʼ�� ---
#ifdef USE_DEBUG_ERR    
  Debug_Err_Init(&debug_ErrInfo); // debug_err��ʼ��
#endif
#ifdef USE_GIO  
  IO_DEV_Init(); // ��ʼ��ͨ��IO�豸����
#endif // USE_GIO  
  BSP_Init(); // �ײ�������ʼ��
  OS_CPU_InitExceptVect(); // Initialize the vector table
  BSP_OS_TmrTickInit(OS_TICKS_PER_SEC); // Initialize the uC/OS-II ticker
  SYS_LED_Init(); // ��ʼ��LED
  Console_Open(); // ��console
  printk("\n------ SBoot ------\n");   
  RunSpeed_DefaultInit(&sys_RunSpeed); // run_speed��ʼ��
  // Board_NandInit(); // ??��ʼ��nandоƬ
  Mem_Init(); // �ڴ�س�ʼ��
  emFile_Init(); // ��ʼ���ļ�ϵͳ
  //BSP_Init2(); // BSP�߼��豸��ʼ��
  //Module_Net_Init(); 
  
  Sys_Init(); // ϵͳ(my_sys)��ʼ��
  
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
ȫ�ֺ���
****/
/*==============================================================================
��������: App_Init
��������: ϵͳ��ʼ��
��  ��:  ��
��  ��:  ��
��  ��:  main()
������:  ����
�ա���:  2013��01��09��
------------------------------ �޶���¼ ------------------------------------- 
�޸ļ�¼: 

==============================================================================*/ 
void App_Init(void)
{
#if (OS_TASK_NAME_EN > 0)
  CPU_INT08U  err;
#endif
  
  BSP_WDT_Disable(); // ����Ƭ�ڿ��Ź�
  
  BSP_AIC_DisableAll(); // Disable all interrupts until we are ready to accept them
  
  OSInit(); // ��ʼ������ϵͳ
  
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