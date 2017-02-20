/*-------------------------------------------------------------------------*/
/* testLeaks.c                                                             */
/* @author: Simon Thelin, Oskar Svensson                                   */
/* @date: 2016-03-24                                                       */
/* @version  3.0                                                           */                                                                     
/*-------------------------------------------------------------------------*/

#include "kernel.h"
#include "dlist.h"
#include "assert.h"
#include "limits.h"
#include "kernel.h"

void tL1(), tL2(), tL3();
int failed=0, finalRuns=0;


int created = 0, deleted = 0, runs = 0, count=0;
mailbox * leakMB;


void tL1(){
  create_task(tL2,ticks()+200);
  for(int i = 1; i < 100; i++){
    if(create_task(tL3,ticks()+201) == NULL){
      created = i-1;
      break;
    }
    
  }
  terminate();
}


void tL2(){
  wait(202);
  if(created!=0){
    if(deleted!=0) {
      assert(created-deleted == 0);
      created = 0;
      deleted = 0;
    }
  }
  else{
    //did not reach 100
    failed=1;
    finalRuns=runs;
    terminate();
  }

  if(failed==0) {
  runs++;
  if(runs <= 100){
    create_task(tL1,ticks()+100);
  }
  terminate();
  }
}

void tL3(){ 
  deleted++;
  terminate(); 
}

void runLeakTest(){
  leak=0;
  upperLeak=0;
  downLeak=0;
  if (create_task(tL1,100) != OK){
    while(1) {
    }
  }
  
  if ((leakMB=create_mailbox(1,sizeof(int))) == NULL){
    while(1) {
    }
  }
}
