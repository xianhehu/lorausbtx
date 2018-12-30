/*
********************************************************************************
                                 SEASON_RTOS

                     (c) Copyright 2015; Season's work
         All rights reserved.  Protected by international copyright laws

File        :  test_mem.h
By          :  Season
Version     :  V0.1.0
Createdate  :  2015-10-22
----------------------------- Liscensing terms ---------------------------------

********************************************************************************
*/

#ifndef TEST_MEM_H
#define TEST_MEM_H

/****
Include
****/
#include "hal_typedef.h"

/****
Defines
****/

/****
Global variable
****/

/****
Global func
****/
// Base mem func
extern  void TestCase_ucMem_Clr(void);
extern  void TestCase_ucMem_Set(void);
extern  void TestCase_ucMem_Copy(void);
extern  void TestCase_ucMem_Move(void);
extern  void TestCase_ucMem_Cmp(void);

extern  void TestCase_ucMem_Pool(void);

/******************************************************************************/
#endif
