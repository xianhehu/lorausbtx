/*
********************************************************************************
                                 File info

                     (c) Copyright 2015; Season's work
         All rights reserved.  Protected by international copyright laws

File          :  test_mem.c
Decrip        :  test cases for uc_mem.
Programmer(s) :  Season
Version       :  V0.1.0
Date          :  2015-10-22
---------------------------------- Records -------------------------------------
--- 2015-10-22-Season: 

********************************************************************************
*/

/****
Include
****/
#include "test_mem.h"  

#include "lib_mem.h"
#include "test_scheduler.h" 
#ifdef USE_DEBUG_ERR
#include "debug_err.h"
#endif
#include "common.h"

#include <stdlib.h>

/****
Local defines
****/

/****
Local variable
****/
// --- Obj for testCase_ucMem_clr ---
static struct test_case testCase_ucMem_clr;                                // Create test case
static const char testCaseName_ucMem_clr[] = "TestCaseName_ucMem_ClrFunc"; // Test case name
static struct sList_node testCaseNode_ucMem_clr;                           // Test case node
// ------------------------------------

// --- Obj for TestCase_ucMem_Set ---
static struct test_case testCase_ucMem_set;                                // Create test case
static const char testCaseName_ucMem_set[] = "TestCaseName_ucMem_SetFunc"; // Test case name
static struct sList_node testCaseNode_ucMem_set;                           // Test case node
// ------------------------------------

// --- Obj for TestCase_ucMem_Copy ---
static struct test_case testCase_ucMem_copy;                                 // Create test case
static const char testCaseName_ucMem_copy[] = "TestCaseName_ucMem_CopyFunc"; // Test case name
static struct sList_node testCaseNode_ucMem_copy;                            // Test case node
// ------------------------------------
#if 0
// --- Obj for TestCase_ucMem_Move ---
static struct test_case testCase_ucMem_move;                                 // Create test case
static const char testCaseName_ucMem_move[] = "TestCaseName_ucMem_MoveFunc"; // Test case name
static struct sList_node testCaseNode_ucMem_move;                            // Test case node
// ------------------------------------
#endif
// --- Obj for TestCase_ucMem_Cmp ---
static struct test_case testCase_ucMem_cmp;                                // Create test case
static const char testCaseName_ucMem_cmp[] = "TestCaseName_ucMem_CmpFunc"; // Test case name
static struct sList_node testCaseNode_ucMem_cmp;                           // Test case node
// ------------------------------------

// --- Obj for TestCase_ucMem_Pool ---
static struct test_case testCase_ucMem_pool;                                 // Create test case
static const char testCaseName_ucMem_pool[] = "TestCaseName_ucMem_PoolFunc"; // Test case name
static struct sList_node testCaseNode_ucMem_pool;                            // Test case node
// ------------------------------------

/****
Global variable
****/

/****
Local func
****/
/*==============================================================================
Name  : TestCaseFunc_ucMemClr
Decrip: 
Input :  
Output: TEST_RESULT_SUCCESS, success; TEST_RESULT_FAIL, fail
Caller: Local
By    : Season
Date  : 2015-06-26
---------------------------------- Records -------------------------------------


==============================================================================*/
static int TestCaseFunc_ucMemClr(struct test_case *priv)
{
  uint8 test_mem[10] = {0};
  uint16 i;
  
  for (i = 0; i < 10; i++)
  {
    test_mem[i] = (uint8)rand();
  }
  
  Mem_Clr(test_mem, 10);
  
  for (i = 0; i < 10; i++)
  {
    if (test_mem[i] != 0)
    {
      return TEST_RESULT_FAIL;
    }
  }
  
  return TEST_RESULT_SUCCESS;
}

/*==============================================================================
Name  : TestCaseFunc_ucMemSet
Decrip: 
Input :  
Output: TEST_RESULT_SUCCESS, success; TEST_RESULT_FAIL, fail
Caller: Local
By    : Season
Date  : 2015-06-26
---------------------------------- Records -------------------------------------


==============================================================================*/
static int TestCaseFunc_ucMemSet(struct test_case *priv)
{
  uint8 test_mem[10] = {0};
  uint16 i;

  Mem_Set(test_mem, 0x55, 10);

  for (i = 0; i < 10; i++)
  {
    if (test_mem[i] != 0x55)
    {
      return TEST_RESULT_FAIL;
    }
  }
  
  return TEST_RESULT_SUCCESS;    
}

/*==============================================================================
Name  : TestCaseFunc_ucMemCopy
Decrip: 
Input :  
Output: TEST_RESULT_SUCCESS, success; TEST_RESULT_FAIL, fail
Caller: Local
By    : Season
Date  : 2015-06-26
---------------------------------- Records -------------------------------------


==============================================================================*/
static int TestCaseFunc_ucMemCopy(struct test_case *priv)
{
  uint8 test_mem_1[10] = {0};
  uint8 test_mem_2[10] = {0};
  uint16 i;

  for (i = 0; i < 10; i++)
  {
    test_mem_1[i] = (uint8)rand();
  }
  
  Mem_Copy(test_mem_2, test_mem_1, 10);
  
  for (i = 0; i < 10; i++)
  {
    if (test_mem_1[i] != test_mem_2[i])
    {
      return TEST_RESULT_FAIL;
    }
  }
  
  return TEST_RESULT_SUCCESS;
}

#if 0
/*==============================================================================
Name  : TestCaseFunc_ucMemMove
Decrip: 
Input :  
Output: TEST_RESULT_SUCCESS, success; TEST_RESULT_FAIL, fail
Caller: Local
By    : Season
Date  : 2015-06-27
---------------------------------- Records -------------------------------------


==============================================================================*/
static int TestCaseFunc_ucMemMove(struct test_case *priv)
{
  uint8 test_mem_1[10] = {0};
  uint8 test_mem_2[10] = {0};
  uint16 i;

  for (i = 0; i < 10; i++)
  {
    test_mem_1[i] = (uint8)rand();
  }

  Mem_Move(test_mem_2, test_mem_1, 10);
  
  for (i = 0; i < 10; i++)
  {
    if (test_mem_1[i] != test_mem_2[i])
    {
      return TEST_RESULT_FAIL;
    }
  }
  
  return TEST_RESULT_SUCCESS;  
}
#endif
/*==============================================================================
Name  : TestCaseFunc_ucMemCmp
Decrip: 
Input :  
Output: TEST_RESULT_SUCCESS, success; TEST_RESULT_FAIL, fail
Caller: Local
By    : Season
Date  : 2015-06-27
---------------------------------- Records -------------------------------------


==============================================================================*/
static int TestCaseFunc_ucMemCmp(struct test_case *priv)
{
  uint8 test_mem_1[10] = {0};
  uint8 test_mem_2[10] = {0};
  uint16 i;

  for (i = 0; i < 10; i++)
  {
    test_mem_1[i] = (uint8)rand();
  }
  
  Mem_Copy(test_mem_2, test_mem_1, 10);
  
  test_mem_1[0] = 0x55;
  test_mem_2[0] = 0x12;  
  if (Mem_Cmp(test_mem_1, test_mem_2, 10) == DEF_YES)
  {
    return TEST_RESULT_FAIL;
  }
  
  test_mem_2[0] = 0x55;
  
  test_mem_1[4] = 0x55;
  test_mem_2[4] = 0x12;
  if (Mem_Cmp(test_mem_1, test_mem_2, 10) == DEF_YES)
  {
    return TEST_RESULT_FAIL;
  }  
  
  test_mem_2[4] = 0x55;
  
  test_mem_1[9] = 0x55;
  test_mem_2[9] = 0x12;
  if (Mem_Cmp(test_mem_1, test_mem_2, 10) == DEF_YES)
  {
    return TEST_RESULT_FAIL;
  }  

  return TEST_RESULT_SUCCESS;  
}

/*==============================================================================
Name  : TestCaseFunc_ucMemPool
Decrip: 
Input :  
Output: TEST_RESULT_SUCCESS, success; TEST_RESULT_FAIL, fail
Caller: Local
By    : Season
Date  : 2015-06-28
---------------------------------- Records -------------------------------------


==============================================================================*/
static int TestCaseFunc_ucMemPool(struct test_case *priv)
{
  static MEM_POOL memPool_test;
  uint16 i;
  uint8 *pBuff[10];
  uint8 *pTemp;
  
  LIB_ERR err;
  CPU_SIZE_T octets_reqd;

  // --- Create mem pool for debug_err_msg ---
  Mem_PoolCreate((MEM_POOL   *)&memPool_test,
                 (void       *) 0,                            // Create from heap
                 (CPU_SIZE_T  ) 0,       
                 (CPU_SIZE_T  ) 10,                           // The num of mem blk
                 (CPU_SIZE_T  ) 1,                            // The length of mem blk
                 (CPU_SIZE_T  ) 4,                            //  
                 (CPU_SIZE_T *)&octets_reqd,
                 (LIB_ERR    *)&err);  
  if (err != LIB_MEM_ERR_NONE)
  { // Cannot Create mem pool, return     
    return TEST_RESULT_FAIL;
  }

  for (i = 0; i < 10; i++)
  {
    pBuff[i] = Mem_PoolBlkGet((MEM_POOL  *)&memPool_test,
                              (CPU_SIZE_T )1,
                              (LIB_ERR   *)&err);
    if (err != LIB_MEM_ERR_NONE)
    {
      return TEST_RESULT_FAIL;
    }
    
    *pBuff[i] = i + 1;
  }
  
  pTemp = Mem_PoolBlkGet((MEM_POOL  *)&memPool_test,
                         (CPU_SIZE_T )1,
                         (LIB_ERR   *)&err); 
  if (err != LIB_MEM_ERR_POOL_EMPTY)
  {
    return TEST_RESULT_FAIL;
  }
  
  // Check the data saved in pBuff[i]
  for (i = 0; i < 10; i++)
  {
    if (*pBuff[i] != i + 1)
    {
      return TEST_RESULT_FAIL;
    }
  }

  // Free one blk and than check wether we can get a new one
  Mem_PoolBlkFree((MEM_POOL *)&memPool_test,
                  (void     *)pBuff[9],
                  (LIB_ERR  *)&err);
  if (err != LIB_MEM_ERR_NONE)  
  {
    return TEST_RESULT_FAIL;
  }
  
  pTemp = Mem_PoolBlkGet((MEM_POOL  *)&memPool_test,
                         (CPU_SIZE_T )1,
                         (LIB_ERR   *)&err); 
  if (err != LIB_MEM_ERR_NONE)
  {
    return TEST_RESULT_FAIL;
  } 
  
  pTemp = pTemp; // Avoid compiler warning
  
  return TEST_RESULT_SUCCESS; 
}

/****
Global func
****/
/*==============================================================================
Name  : TestCase_ucMem_Clr
Decrip: Init test case for Mem_Clr().
Input :  
Output: None
Caller: app
By    : Season
Date  : 2015-10-26
---------------------------------- Records -------------------------------------


==============================================================================*/
void TestCase_ucMem_Clr(void)
{
  // --- Init test case ---
  testCase_ucMem_clr.flag.flag      = 0;                      // Clear flag param
  testCase_ucMem_clr.test_name      = testCaseName_ucMem_clr; // Add test case name
  testCase_ucMem_clr.run_testCase   = TestCaseFunc_ucMemClr;
  //testCase_ucMem_clr.flag.bits.repeat_num =
  testCase_ucMem_clr.flag.bits.type = TEST_TYPE_UNIT;         // Unit test
  testCase_ucMem_clr.flag.bits.init = 1;                      // Finish init
  
  // --- Init test case node ---
  testCaseNode_ucMem_clr.node = &testCase_ucMem_clr; 
  testCaseNode_ucMem_clr.next = NULL;
  
  TestBench_AddCase(&testCaseNode_ucMem_clr); // Add test case node in test bench   
}

/*==============================================================================
Name  : TestCase_ucMem_Set
Decrip: Init test case for Mem_Set().
Input :  
Output: None
Caller: app
By    : Season
Date  : 2015-10-26
---------------------------------- Records -------------------------------------


==============================================================================*/
void TestCase_ucMem_Set(void)
{  
  // --- Init test case ---
  testCase_ucMem_set.flag.flag      = 0;                      // Clear flag param
  testCase_ucMem_set.test_name      = testCaseName_ucMem_set; // Add test case name
  testCase_ucMem_set.run_testCase   = TestCaseFunc_ucMemSet;
  //testCase_ucMem_set.flag.bits.repeat_num =
  testCase_ucMem_set.flag.bits.type = TEST_TYPE_UNIT;         // Unit test
  testCase_ucMem_set.flag.bits.init = 1;                      // Finish init
  
  // --- Init test case node ---
  testCaseNode_ucMem_set.node = &testCase_ucMem_set; 
  testCaseNode_ucMem_set.next = NULL;
  
  TestBench_AddCase(&testCaseNode_ucMem_set); // Add test case node in test bench  
}

/*==============================================================================
Name  : TestCase_ucMem_Copy
Decrip: Init test case for Mem_Copy().
Input :  
Output: None
Caller: app
By    : Season
Date  : 2015-10-26
---------------------------------- Records -------------------------------------


==============================================================================*/
void TestCase_ucMem_Copy(void)
{
  // --- Init test case ---
  testCase_ucMem_copy.flag.flag      = 0;                       // Clear flag param
  testCase_ucMem_copy.test_name      = testCaseName_ucMem_copy; // Add test case name
  testCase_ucMem_copy.run_testCase   = TestCaseFunc_ucMemCopy;
  //testCase_ucMem_copy.flag.bits.repeat_num =
  testCase_ucMem_copy.flag.bits.type = TEST_TYPE_UNIT;         // Unit test
  testCase_ucMem_copy.flag.bits.init = 1;                      // Finish init
  
  // --- Init test case node ---
  testCaseNode_ucMem_copy.node = &testCase_ucMem_copy; 
  testCaseNode_ucMem_copy.next = NULL;
  
  TestBench_AddCase(&testCaseNode_ucMem_copy); // Add test case node in test bench   
}
#if 0
/*==============================================================================
Name  : TestCase_ucMem_Move
Decrip: Init test case for Mem_Move().
Input :  
Output: None
Caller: app
By    : Season
Date  : 2015-10-27
---------------------------------- Records -------------------------------------


==============================================================================*/
void TestCase_ucMem_Move(void)
{
  // --- Init test case ---
  testCase_ucMem_move.flag.flag      = 0;                       // Clear flag param
  testCase_ucMem_move.test_name      = testCaseName_ucMem_move; // Add test case name
  testCase_ucMem_move.run_testCase   = TestCaseFunc_ucMemMove;
  //testCase_ucMem_move.flag.bits.repeat_num =
  testCase_ucMem_move.flag.bits.type = TEST_TYPE_UNIT;         // Unit test
  testCase_ucMem_move.flag.bits.init = 1;                      // Finish init
  
  // --- Init test case node ---
  testCaseNode_ucMem_move.node = &testCase_ucMem_move; 
  testCaseNode_ucMem_move.next = NULL;
  
  TestBench_AddCase(&testCaseNode_ucMem_move); // Add test case node in test bench   
}
#endif
/*==============================================================================
Name  : TestCase_ucMem_Cmp
Decrip: Init test case for Mem_Cmp().
Input :  
Output: None
Caller: app
By    : Season
Date  : 2015-10-27
---------------------------------- Records -------------------------------------


==============================================================================*/
void TestCase_ucMem_Cmp(void)
{
  // --- Init test case ---
  testCase_ucMem_cmp.flag.flag      = 0;                      // Clear flag param
  testCase_ucMem_cmp.test_name      = testCaseName_ucMem_cmp; // Add test case name
  testCase_ucMem_cmp.run_testCase   = TestCaseFunc_ucMemCmp;
  //testCase_ucMem_cmp.flag.bits.repeat_num =
  testCase_ucMem_cmp.flag.bits.type = TEST_TYPE_UNIT;         // Unit test
  testCase_ucMem_cmp.flag.bits.init = 1;                      // Finish init
  
  // --- Init test case node ---
  testCaseNode_ucMem_cmp.node = &testCase_ucMem_cmp; 
  testCaseNode_ucMem_cmp.next = NULL;
  
  TestBench_AddCase(&testCaseNode_ucMem_cmp); // Add test case node in test bench   
}

/*==============================================================================
Name  : TestCase_ucMem_Pool
Decrip: Init test case for Mem pool.
Input :  
Output: None
Caller: app
By    : Season
Date  : 2015-10-28
---------------------------------- Records -------------------------------------


==============================================================================*/
void TestCase_ucMem_Pool(void)
{
  // --- Init test case ---
  testCase_ucMem_pool.flag.flag      = 0;                       // Clear flag param
  testCase_ucMem_pool.test_name      = testCaseName_ucMem_pool; // Add test case name
  testCase_ucMem_pool.run_testCase   = TestCaseFunc_ucMemPool;
  //testCase_ucMem_pool.flag.bits.repeat_num =
  testCase_ucMem_pool.flag.bits.type = TEST_TYPE_INTER;        // Unit test
  testCase_ucMem_pool.flag.bits.init = 1;                      // Finish init
  
  // --- Init test case node ---
  testCaseNode_ucMem_pool.node = &testCase_ucMem_pool; 
  testCaseNode_ucMem_pool.next = NULL;
  
  TestBench_AddCase(&testCaseNode_ucMem_pool); // Add test case node in test bench   
}