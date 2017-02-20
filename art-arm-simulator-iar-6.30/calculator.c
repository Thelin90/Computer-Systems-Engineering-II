/*-------------------------------------------------------------------------*/
/* calculator.c                                                            */
/* @author: Simon Thelin, Oskar Svensson                                   */
/* @date: 2016-04-10                                                      ´*/
/* @version  1.0                                                           */                                                                     
/*-------------------------------------------------------------------------*/
#include "kernel.h"
#include "dlist.h"
#include "assert.h"
#include "limits.h"
#include "kernel_hwdep.h"
#include <math.h>

void runCalc();
mailbox *sendMB, *getMB;
volatile int firstRun=0;
/*
This task will calculate with the given values
*/
void calculate(){
  int valOne=0;
  int valTwo=0;
  int sum;
  
  receive_no_wait(sendMB,&valOne);
  receive_no_wait(sendMB,&valTwo);
  
  sum = valOne*valTwo;
  send_wait(getMB, &sum);
  
  terminate();
}

/*
This task will send the vals for the calculation
*/
void sendVals(){
  int rnd1 = (rand()%4)+1;
  int rnd2 = (rand()%4)+10;
  
  send_no_wait(sendMB,&rnd1);
  send_no_wait(sendMB,&rnd2);
  terminate();
}

/*
This task will print out the result from the calculation
*/
void printRes(){
  
  int res;
  
  recieve_wait(getMB,&res);
  
  printf("Calculator\n");
  printf("----------\n");
  printf("Result:");
  printf("%d", res);
  printf("\n----------");
  
  wait(300);
  
  terminate();
}




/*
This function creates the necessary units for the API to function.
It will init the kernel and after that create tasks and mailboxes.
*/
void runCalc() {
  
  if(firstRun==0) {
    if (init_kernel() !=OK){
      while(1);
    }
  } 
  if (create_task(calculate, 200) != OK){
    while(1) {
      terminate();
    }
  }
  if (create_task(sendVals, 100) != OK){
    while(1) {
      terminate();
    }
  }
  if (create_task(printRes, 300) != OK){
    while(1) {
      terminate();
    }
  }
  if ((sendMB=create_mailbox(2,sizeof(int))) == NULL){
    while(1) {
      terminate();
    }
  }
  if ((getMB=create_mailbox(1,sizeof(int))) == NULL){
    while(1) {
      terminate();
    }
  }
  run();
}
