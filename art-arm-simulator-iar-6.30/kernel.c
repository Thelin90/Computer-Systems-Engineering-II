#include "kernel.h"
#include "dlist.h"
#include "assert.h"
#include "limits.h"
#include "kernel_hwdep.h"


//TCB * Running;

list *timerList, *waitList, *readyList;


/**
This function initializes the kernel and its data
structures and leaves the kernel in start-up mode. The
init_kernel call must be made before any other call is
made to the kernel.
Argument
none
Return parameter
Int: Description of the functionís status, i.e. FAIL/OK.
*/
exception init_kernel() {
  /*Put counter to 0*/
  set_ticks(0);
  /*Create datastructures*/
  timerList=create_list();
  if(timerList == NULL) {
    remove_list(timerList);
  }
  waitList=create_list();
  if(waitList==NULL) {
    remove_list(waitList);
  }
  readyList=create_list();
  if(readyList==NULL){
    remove_list(readyList);
  }
  /* 
  idle task, put the highest possible value of uint as deadline, told
  by Nicolas during lecture.
  */
  startUp = TRUE; 
  create_task(idle, UINT_MAX);
  //create_task(idle, UINT_MAX); //max uint
  // set kernel in start up mode
  return OK;
}


/**
This function starts the kernel and thus the system of
created tasks. Since the call will start the kernel it will
leave control to the task with tightest deadline.
Therefore, it must be placed last in the application
initialization code. After this call the system will be in
running mode.
Argument
none
Return parameter
none
*/
void run() {
  //init interrupt timer
  timer0_start();
  
  //set the kernel running mode, first node done
  startUp=FALSE;
  
  set_isr(ISR_ON);
  
  Running = readyList->pHead->pNext->pTask;
  
  LoadContext();
}
