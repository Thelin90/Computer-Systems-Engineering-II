#include "kernel.h"
#include "dlist.h"
#include "assert.h"
#include "limits.h"
#include "kernel_hwdep.h"

TCB * Running;
extern volatile unsigned int counter=0;
/*
external void *secureCalloc(unsigned int a, unsigned int size);
external void freeWith_isr_off_on(void *free_obj);
*/

/**
This call will set the tick counter to the given value.
Argument
nTicks: the new value of the tick counter
Return parameter
none
*/
void set_ticks(uint no_of_ticks) {
  counter=no_of_ticks;
}

/**
This call will return the current value of the tick counter
Argument
none
Return parameter
A 32 bit value of the tick counter
*/
uint ticks(void) {
  return counter;
}

/**
This call will return the deadline of the specified task
Argument
none
Return parameter
the deadline of the given task
*/
uint deadline( void ) {
  return readyList->pHead->pNext->pTask->DeadLine;
}

/**
This call will set the deadline for the calling task. The
task will be rescheduled and a context switch might
occur.
Argument
deadline: the new deadline given in number of ticks.
Return parameter
none
*/
void set_deadline(uint deadline) {
  volatile bool firstExe=TRUE;
  listobj *temp;
  //isr_off();
  set_isr(ISR_OFF);
  SaveContext();
  if(firstExe) {
    firstExe=FALSE;
    temp = extractionFirst(readyList);
    temp->pTask->DeadLine=deadline;
    insertSortByDeadLine(readyList, temp);
    Running = readyList->pHead->pNext->pTask; 
    LoadContext();
  }
}

/**
This function is not available for the user to call.
It is called by an ISR (Interrupt Service Routine)
invoked every tick. Note, context is automatically saved
prior to call and automatically loaded on function exit.
*/
void TimerInt(void) {
  listobj *tempo = timerList->pHead->pNext;
 // listobj *temp;
  /*Increment counter*/
  counter++;
  /*
  Check timerlist(FIFO) if ready
  Move to readyList
  */
  while(tempo->nTCnt <= ticks() && tempo != timerList->pTail) {
    timerList->pHead->pNext = tempo->pNext;
    tempo->pNext->pPrevious = timerList->pHead;
    tempo->pNext = tempo;
    tempo->pPrevious = tempo;
    
    insertSortByDeadLine(readyList,tempo);
    tempo = timerList->pHead->pNext;
    /*
    tempo = extractionFirst(timerList);
    insertSortByDeadLine(readyList,tempo);
    */
  }
  /*
  Check waitinglist that have expired deadlines
  Move to readylist, and set the Running pointer.
  */
  //tempo = waitList->pHead->pNext;
  while(waitList->pHead->pNext->pTask->DeadLine <= ticks() && waitList->pHead->pNext != waitList->pTail) {
    //listobj * tempo;
    /*
    waitList->pHead->pNext = tempo->pNext;
    tempo->pNext->pPrevious = waitList->pHead;
    tempo->pNext = tempo;
    tempo->pPrevious = tempo;
    insertSortByDeadLine(readyList,tempo);
    tempo = waitList->pHead->pNext;
    */
    
    //tempo = extractionFirst(waitList);
    insertSortByDeadLine(readyList,extractionFirst(waitList));
    
  }
  /* Snygg lösning */
  Running = readyList->pHead->pNext->pTask;
}


/**
This call will block the calling task until the given
number of ticks has expired.
Argument
nTicks: the duration given in number of ticks
Return parameter
exception: The exception return parameter can have
two possible values:
• OK: Normal function, no exception occurred.
• DEADLINE_REACHED: This return parameter
is given if the receiving tasksí deadline is
reached while it is blocked by the receive_wait call.
*/
exception wait(uint nTicks) {
  int volatile status=OK;
  int volatile first_exe=TRUE;
  int prevISR = set_isr(ISR_OFF);
  SaveContext();

  if(first_exe) {
    listobj * temp;
    first_exe=FALSE;
    temp = extractionFirst(readyList);
    
    temp->nTCnt = ticks() + nTicks;

    insertSortBynTCnt(timerList, temp);
    Running = readyList->pHead->pNext->pTask;
    LoadContext();
  } else {
    /*
    The deadline must be done within the counter time
    */
    if(deadline() <= counter) {
      status=DEADLINE_REACHED;
    } else {
      status=OK;
    }
  }
  return status;
}

/*
Creates the idle task
*/
void idle(void) {
  while(1) {
    SaveContext();
    TimerInt();
    LoadContext();
  }
}

void *secureCalloc(unsigned int a, unsigned int size) {
  void *temp;
  uint status = set_isr(ISR_OFF);
  //obj = (obj *)calloc(a, sizeof(obj));
  temp = calloc(a,size);
  if(temp==NULL) {
    set_isr(status);
    return NULL;
  }
//  upperLeak++;
 // leak++;
  set_isr(status);
  return temp;
}

void freeWith_isr_off_on(void *free_obj){
  uint status = set_isr(ISR_OFF);
  free(free_obj);
  set_isr(status);
 // downLeak--;
  //leak--;
}
