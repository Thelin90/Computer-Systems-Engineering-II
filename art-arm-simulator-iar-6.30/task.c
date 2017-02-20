#include "kernel.h"
#include "dlist.h"
#include "assert.h"
#include "limits.h"
#include "kernel_hwdep.h"

//global eller inte tänk
extern volatile unsigned int startUp=0; //, counter=0;

/**
This function creates a task. If the call is made in startup
mode, i.e. the kernel is not running, only the
necessary data structures will be created. However, if
the call is made in running mode, it will lead to a
rescheduling and possibly a context switch.

Argument
*task_body: A pointer to the C function holding the code
of the task.
deadline: The kernel will try to schedule the task so it
will meet this deadline.

Return parameter
Description of the functionís status, i.e. FAIL/OK.
*/
exception create_task(void(*task_body)(), uint deadline) {
  
  int volatile first_exe=1;

  listobj * myobj = (listobj *)secureCalloc(1, sizeof(listobj));
  
  if (myobj == NULL)
  {
    return NULL;
  }
  
  myobj->pTask = (TCB *)secureCalloc(1, sizeof(TCB));
  
  if(myobj->pTask == NULL) {
    freeWith_isr_off_on(myobj);
    return NULL;
  }
  
  myobj->pTask->DeadLine = deadline;
  myobj->pTask->PC = task_body;
  myobj->pTask->SP = &(myobj->pTask->StackSeg[STACK_SIZE-1]);
  
  /**
  IF start-up mode THEN
  Insert new task in Readylist
  Set the Running pointer to the first task
  Return status
  */
  if(startUp == TRUE) {
    insertSortByDeadLine(readyList, myobj);
    Running = readyList->pHead->pNext->pTask;
  }
  else {
//    isr_off();
    set_isr(ISR_OFF);
    SaveContext();
    /*
    If first_exe, then add the obj to the readylist
    Then set the Running pointer to the first task
    Load the context
    */
    if(first_exe) {
      first_exe=FALSE;
      insertSortByDeadLine(readyList, myobj);
      Running = readyList->pHead->pNext->pTask;
      LoadContext();
    }
  }
  return OK;
}


/**
(ReadyList is Fifo)
This call will terminate the running task. All data
structures for the task will be removed. Thereafter,
another task will be scheduled for execution.
Argument
none
Return parameter
none
*/
void terminate() {
  if(readyList->pHead->pNext != NULL) {
   // int intStatus = set_isr(ISR_OFF);
    listobj * temp = extractionFirst(readyList);
    if(temp!=NULL) {
    freeWith_isr_off_on(temp->pTask);
    freeWith_isr_off_on(temp->pMessage);
    }
  //  set_isr(intStatus);
    /*set next task to be running task, load context*/
    Running = readyList->pHead->pNext->pTask;
    LoadContext();
  }
}