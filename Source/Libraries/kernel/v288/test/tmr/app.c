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
#include "sys.h" 
#include "emFile.h"
#include "run_speed.h" 
// -------- test --------
#include "test_comm.h"
// ----------------------
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
  //uint32 i;
  
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
  // ??
  
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
// --- tmr test ---
OS_TMR *myTmr;
bool myTmrStatue;
static void MyTmrCallBack(void *ptmr, void *callback_arg)
{
  printk("MyTmrCallBack: run in!\n");
}

// ----------------

static void Task_Start(void *p_arg)
{
  (void)p_arg;
  
#ifdef DEBUG
    printk("Task_Start: run in!\n");
#endif
  // --- ��һ����ʼ�� ---
  Debug_Err_Init(&debug_ErrInfo); // debug_err��ʼ��
  BSP_Init(); // �ײ�������ʼ��
  OS_CPU_InitExceptVect(); // Initialize the vector table
  BSP_OS_TmrTickInit(OS_TICKS_PER_SEC); // Initialize the uC/OS-II ticker
  SYS_LED_Init(); // ��ʼ��LED
  Console_Open(); // ��console
  RunSpeed_DefaultInit(&sys_RunSpeed); // run_speed��ʼ��
  // Board_NandInit(); // ??��ʼ��nandоƬ
  emFile_Init(); // ��ʼ���ļ�ϵͳ
  // --------------------  
#if (OS_TASK_STAT_EN > 0)
  OSStatInit(); // Determine CPU capacity                 
#endif

  //Sys_Init(); // ϵͳ(my_sys)��ʼ��
 
  // -------- test --------
  //TestComm_Start();
  // ----------------------
  
  // --- tmr test ---
  uint8 err;
  myTmr = OSTmrCreate(10, 
                      50,
                      OS_TMR_OPT_PERIODIC,
                      MyTmrCallBack,
                      (void *)0,
                      "My Test TMR",
                      &err
                      );
  if (err != OS_ERR_NONE)
  {
    printk("Task_Start: Cannot create tmr -- error!\n");
  }
  
  myTmrStatue = OSTmrStart(myTmr, &err);
  if (err != OS_ERR_NONE)
  {
    printk("Task_Start: Cannot start tmr -- error!\n");
  }
  
  // ----------------
  
  
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
��  ��:  
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
  
  BSP_WDT_Disable();
  
  BSP_AIC_DisableAll(); // Disable all interrupts until we are ready to accept them
  
  OSInit(); // Initialize "uC/OS-II, The Real-Time Kernel"
  
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
  OSTaskNameSet(APP_CFG_TASK_START_PRIO, "tStart", &err);
#endif 

  OSStart(); // Start multitaskin  
}