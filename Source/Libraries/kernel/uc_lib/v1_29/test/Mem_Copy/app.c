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

// --- Task_Main ---
#define  APP_CFG_TASK_MAIN_STK_SIZE                 500   
static OS_STK TaskStk_Main[APP_CFG_TASK_MAIN_STK_SIZE];

// --- Module_Node ---
struct sList_node SysMNode_Monitor;
struct sList_node SysMNode_NetTcpEcho;
struct sList_node SysMNode_NetHttpServer;
// -------------------
/****
ȫ�ֱ���
****/


/****
�ֲ�����
****/
/*==============================================================================
��������: Task_Main
��������: ϵͳ������        
��  ��:  
��  ��:  ��
��  ��:  
������:  ����
�ա���:  2013��06��03��
------------------------------ �޶���¼ ------------------------------------- 
�޸ļ�¼: 

==============================================================================*/
static void Task_Main(void *p_arg)
{
  (void)p_arg;
  uint32 i;
  struct sList_node *pNode;
  struct module *pModule; 
  
  //SBoot_PrintBootLogo();
  
  // --- �������õ����ģ�� ---
  for (i = 0; i < my_sys.module_slist.num; i++)
  {
    pNode = SList_PickN(&(my_sys.module_slist), i + 1);
    pModule = (struct module *)pNode->node;
    (pModule->pStart)(pModule); // ʹ�ܸ�ģ�鹦������
  }   
  
  while (1)
  {
    OSTimeDly(OS_TICKS_PER_SEC);
  }
}

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
#if (OS_TASK_NAME_EN > 0)
  CPU_INT08U  err;
#endif
  
  // --- ���������� ---
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
  SYS_VERSION tmp_ver; // ��ϵͳ�汾
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
  
  // --- ������ʼ�� ---
  /*
  Sys_Param_Init(&(my_sys.param)); // ��ʼ��sys_param
  Sys_Param_ConfigParam(&(my_sys.param), (void *)&sboot_param, SYS_PARAM_CONFPARAM_OPT_PRIV); // ����sboot_param
  Sys_Param_ConfigParam(&(my_sys.param), (void *)SBoot_Param_Init, SYS_PARAM_CONFPARAM_OPT_PINIT); // ����init����ָ��
  Sys_Param_ConfigParam(&(my_sys.param), (void *)SBoot_Param_Set, SYS_PARAM_CONFPARAM_OPT_PSET); // ����set����ָ��
  Sys_Param_ConfigParam(&(my_sys.param), (void *)SBoot_Param_Read, SYS_PARAM_CONFPARAM_OPT_PREAD); // ����read����ָ��
  (my_sys.param.pInit)(&(my_sys.param)); // ��ʼ��, ��ȡ����  
  */
  
  // --- to do: ���moduleģ��(����ɳ�ʼ�����ٽ��м���) ---
  Module_OSMonitor_Init(); // ��ʼ��
  SysMNode_Monitor.node = (void *)&m_OSMonitor; // ���ģ��ڵ�
  SysMNode_Monitor.next = NULL;
  Sys_ConfigParam(&my_sys, (void *)(&SysMNode_Monitor), SYS_CONFPARAM_OPT_MODULE); // ���Moduleģ��
  // --- Net ---
  // TCP Echo
  Module_Net_TcpEcho_Init();
  SysMNode_NetTcpEcho.node = (void *)&m_net_tcpecho; // ���ģ��ڵ�
  SysMNode_NetTcpEcho.next = NULL;
  Sys_ConfigParam(&my_sys, (void *)(&SysMNode_NetTcpEcho), SYS_CONFPARAM_OPT_MODULE); // ���Moduleģ��
  // Http Server
  Module_Net_HttpServer_Init();
  SysMNode_NetHttpServer.node = (void *)&m_net_httpserver; // ���ģ��ڵ�
  SysMNode_NetHttpServer.next = NULL;
  Sys_ConfigParam(&my_sys, (void *)(&SysMNode_NetHttpServer), SYS_CONFPARAM_OPT_MODULE); // ���Moduleģ��  

  // --- to do: ���hwDevģ��(����ɳ�ʼ�����ٽ��м���) ---
  // ??  
  
  // --- ���start����ָ�� ---
  Sys_ConfigParam(&my_sys, (void *)Sys_Start, SYS_CONFPARAM_OPT_PSTART);
  
  // --- flag ---
  tmp_flag.flag = 0;
  tmp_flag.bits.init = 1; // ��ɳ�ʼ��
  tmp_flag.bits.run = 1; // ϵͳ��ʼ����
  tmp_flag.bits.use_os = 1; // ʹ�ò���ϵͳ
  tmp_flag.bits.sys_led = 1; // ʹ��ϵͳ��
#ifdef USE_DEBUG_ERR  
  tmp_flag.bits.debug_err = 1; // ʹ��debug_err
#endif
  tmp_flag.bits.use_cli = 1; // ʹ��cliģ��  
  Sys_ConfigParam(&my_sys, &tmp_flag, SYS_CONFPARAM_OPT_FLAG);
  
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
  }
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
// mem_cpy���� 
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
  // --- ��һ����ʼ�� ---
  Mem_Init(); // �ڴ�س�ʼ��
#ifdef USE_DEBUG_ERR    
  Debug_Err_Init(&debug_err_info); // debug_err��ʼ��
#endif
#ifdef USE_GIO  
  IO_DEV_Init(); // ��ʼ��ͨ��IO�豸����
#endif // USE_GIO
  BSP_Init(); // �ײ�������ʼ��
  OS_CPU_InitExceptVect(); // Initialize the vector table
  BSP_OS_TmrTickInit(OS_TICKS_PER_SEC); // Initialize the uC/OS-II ticker
  Console_Open(); // ��console
  RunSpeed_DefaultInit(&sys_RunSpeed); // run_speed��ʼ��
  Sys_Log_Init(&sys_log_info); // ��ʼ��sys_log
  //emFile_Init(); // ��ʼ���ļ�ϵͳ
  // --- �����ʼ�� ---
#ifdef NET_LWIP  
  //tcpip_init(NULL, NULL); // ��ʼ��lwipЭ��ջ
#endif 
  //Sys_Init(); // ϵͳ(my_sys)��ʼ��
  //BSP_Init2(); // BSP�߼��豸��ʼ��  
  // --------------------  
#if (OS_TASK_STAT_EN > 0)
  OSStatInit(); // Determine CPU capacity                 
#endif
  
  // --- mem_cpy���� ---
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