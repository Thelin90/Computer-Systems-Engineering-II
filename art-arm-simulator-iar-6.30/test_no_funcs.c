/*-------------------------------------------------------------------------*/
/* main.c                                                                  */
/* @author: Simon Thelin, Oskar Svensson                                   */
/* @date: 2016-03-24                                                       */
/* @version  3.0                                                           */                                                                     
/*-------------------------------------------------------------------------*/

#include "kernel.h"
#include "dlist.h"
#include "assert.h"
#include "limits.h"
#include "kernel.h"


#define TEST_PATTERN_11 0xAA
#define TEST_PATTERN_22 0x55

mailbox *mb2;
int t2_OK=0,no_nTest1=0, no_nTest2=0, no_nTest3=0;

void task3(void) {
  int nData_t3 = TEST_PATTERN_11;
  
  /*
  Should be empty
  */
  if(empty_Mailbox(mb2) != 0) {
    terminate();
  }
  
  if(send_no_wait(mb2, &nData_t3)==DEADLINE_REACHED) {
    terminate();
  }

  /*
  Shoulb not be empty
  */
  if(empty_Mailbox(mb2) != 1) {
    terminate();
  }
  
  terminate();
}

void task4(void) {
  int nData_t4=0;
  
  
  
  if(receive_no_wait(mb2, &nData_t4)!=DEADLINE_REACHED) {
    
  
  
  if(empty_Mailbox(mb2) != 0) {
    terminate();
  }
  
  if(nData_t4 == TEST_PATTERN_11) {
    no_nTest1=1;
  }


  terminate();
  }
  else {
    terminate();
  }
  
}

void task5(void) {
  remove_mailbox(mb2);

  runLeakTest();
  
  
  terminate();
  
}


void test_no_funcs(void) {

  if (create_task(task3, 5000) != OK){
    while(1) {
    }
  }
  if (create_task(task4, 6000) != OK){
    while(1) {
    }
  }
  
  if (create_task(task5, 7000) != OK){
    while(1) {
    }
  }
  if ((mb2=create_mailbox(1,sizeof(int))) == NULL){
    while(1) {
    }
  }
}