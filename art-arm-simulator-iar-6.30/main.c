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
#include "kernel_hwdep.h"

volatile int leak=0;
volatile int upperLeak=0;
volatile int downLeak=0;
#define TEST_PATTERN_1 0xAA
#define TEST_PATTERN_2 0x55

mailbox *mb,*mbGet;

int nTest1=0, nTest2=0, nTest3=0, t1_OK=0;

void task1(void){
  int nData_t1 = TEST_PATTERN_1;
  int a=1; 
  int b=2;
  int c=3;
  
  send_no_wait(mb,&a);
  send_no_wait(mb,&b);
  send_no_wait(mb,&c);
  
  send_wait(mbGet,&nData_t1);
//  
//  wait(10);
//  
//  
//  /** 
//  Mailbox should not be empty
//  1=not empty, 0=empty
//  nMessage should be -1, 
//  nBlockedMsg should be 0
//  */ 
//  if(empty_Mailbox(mb) != 1) {
//    terminate();
//  }
//  
//  if(mb->nMessages != -1) {
//    terminate();
//  }
//  
//  if(mb->nBlockedMsg!=-1) {
//    terminate();
//  }
//  
//  if(send_wait(mb,&nData_t1) == DEADLINE_REACHED) {
//    terminate();
//  }
//  
//  wait(10);
//  
//  /* test 2 */
//  nData_t1 = TEST_PATTERN_2;
//  
//  if(send_wait(mb,&nData_t1) == DEADLINE_REACHED) {
//    terminate();
//  }
//  
//  wait(10);
//  
//  /* test 3 */
//  if(send_wait(mb,&nData_t1)==DEADLINE_REACHED){
//    
//    // Mailbox should be empty
//    if(empty_Mailbox(mb) != 0) { 
//      terminate();
//    }
//    
//    nTest3 = 1;
//    if(nTest1*nTest2*nTest3){
//      //printf("%s","Test ok");
//      t1_OK=TRUE;
//    }
//    
//    remove_mailbox(mb);
//  //  test_no_funcs();
////    runLeakTest();
//    terminate();
//  }
//  else
//  {
//    terminate();
//  }
  terminate();
  
}

void task2(void){
  int nData_t2=0;
  int one=0;
  int two=0;
  
  receive_no_wait(mb,&one);
  receive_no_wait(mb,&two);
  
  
  
//  if(recieve_wait(mb,&nData_t2) == DEADLINE_REACHED) {
//    terminate();
//  }
//  
//  /** 
//  Mailbox should be empty
//  1=not empty, 0=empty
//  nMessage should be 0
//  nBlockedMsg should be 1
//  */ 
//  if(empty_Mailbox(mb) != 0) { 
//    terminate();
//  }
//  
//  if(nData_t2 == TEST_PATTERN_1) {
//    nTest1 = 1;
//  }
//  
//  wait(100);
//  
//  //test 2
//  
//  /** 
//  Mailbox should not be empty
//  1=not empty, 0=empty
//  nMessage should be -1, 
//  nBlockedMsg should be 0
//  */
//  if (empty_Mailbox(mb) !=1) {
//    terminate();
//  }
//  
//  if(mb->nMessages != 1) {
//    terminate();
//  }
//  
//  if(mb->nBlockedMsg!=1) {
//    terminate();
//  }
//  
//  if(recieve_wait(mb,&nData_t2) == DEADLINE_REACHED) {
//    terminate();
//  }
//  
//  //no_messages
//  if(empty_Mailbox(mb) !=0) {
//    terminate();
//  }
//  
//  if (nData_t2 == TEST_PATTERN_2) {
//    nTest2 = 1;
//  }
//  
//  if(empty_Mailbox(mb)!=0) {
//    terminate();
//  }
//  
//  //  remove_mailbox(mb);
//  //  remove_list(readyList);
//  //  remove_list(timerList);
//  //  remove_list(waitList);
//  
//  //  runLeakTest();
//  //  
//  //  test_no_funcs();
  
  terminate();
}

void main(void) {
  
  //runElevator();
  
  runCalc();
   
//  if (init_kernel() !=OK){
//  while(1);
//  }
//  
//  //runLeakTest();
////  
//  if (create_task(task1, 2000) != OK){
//    while(1) {
//    }
//  }
//  
//  if (create_task(task2, 4000) != OK){
//    while(1) {
//    }
//  }
//  
//  if ((mb=create_mailbox(2,sizeof(int))) == NULL){
//    while(1) {
//    }
//  }
//    
//  if ((mbGet=create_mailbox(2,sizeof(int))) == NULL){
//    while(1) {
//    }
//    
//  
//  
//  }
//  
////  set_deadline(1900);
////  if(readyList->pHead->pNext->pTask->DeadLine == 1900){
////    set_deadline(1500);
////  }
////  if(readyList->pHead->pNext->pTask->DeadLine == 1500) {
////    set_deadline(1000);
////  }
////  
////  if(readyList->pHead->pNext->pTask->DeadLine == 1000) {
////    int good = 1;
////  }
//  
//  
//  //runLeakTest();
//  leak=0;
//  upperLeak=0;
//  downLeak=0;
//  run();
  
//  terminate();
  
  
  /**
  Alternativt läckagetest, testas vid avkommentering
  */
}
