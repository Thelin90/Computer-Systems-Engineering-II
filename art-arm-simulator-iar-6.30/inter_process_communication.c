#include "kernel.h"
#include "dlist.h"
#include "assert.h"
#include "limits.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "kernel_hwdep.h"

/*
external void *secureCalloc(unsigned int a, unsigned int size);
external void freeWith_isr_off_on(void *free_obj);
*/

/**
This call will create a Mailbox. The Mailbox is a FIFO
communication structure used for asynchronous and
synchronous communication between tasks.
Argument
nof_msg: Maximum number of Messages the Mailbox
can hold.
Size_of msg: The size of one Message in the Mailbox.
Return parameter
Mailbox*: a pointer to the created mailbox or NULL.
*/
mailbox *create_mailbox(uint nof_msg, uint size_of_msg) {
  /*
  Allocate memory for the Mailbox
  Initialize Mailbox structure
  Return Mailbox*
  */
  mailbox * myMailList = (mailbox *)secureCalloc(1, sizeof(mailbox));
  if (myMailList == NULL) {
    return NULL;
  }
  
  myMailList->pHead = (msg *)secureCalloc(1, sizeof(msg));
  if (myMailList->pHead == NULL) {
    freeWith_isr_off_on(myMailList);
    return NULL;
  }
  
  myMailList->pTail = (msg *)secureCalloc(1, sizeof(msg));
  if (myMailList->pTail == NULL) {
    freeWith_isr_off_on(myMailList->pHead);
    freeWith_isr_off_on(myMailList);
    return NULL;
  }
  
  myMailList->pHead->pPrevious = myMailList->pHead;
  myMailList->pTail->pPrevious = myMailList->pHead; // tail.prev pekar på head box, good shit
  myMailList->pHead->pNext = myMailList->pTail;  // head.prev pekar på tail box
  myMailList->pTail->pNext = myMailList->pTail;
  
  myMailList->nDataSize=size_of_msg;
  myMailList->nMaxMessages=nof_msg;
  
  return myMailList;
}


/**
This function will add a message to the mailbox in FIFO style.
Argument
*list: a pointer to the specified Mailbox.
*message: a pointer to the structure of msg
Return parameter
NaN
*/
exception insertMsgBox(mailbox *list, msg *message) {
  exception status=OK;
  if(message!=NULL) {
    msg *temp = list->pHead;
    message->pNext=temp->pNext;
    message->pPrevious=temp;
    temp->pNext->pPrevious = message;
    temp->pNext = message;
    return status;
  } else {
  status=FAIL;
  }
  return status;
}

/**
Check if the mailbox contains a message
Argument 
*mailbox
Return parameter
True/False
*/
int empty_Mailbox(mailbox* mBox ) {
  int volatile flag=FALSE;
  
  if(mBox->pHead->pNext!=mBox->pTail) {
    flag=TRUE;
  }
  
  return flag;
}

exception remove_mailbox(mailbox* mBox) {
  exception status=OK;
  if(mBox->pHead->pNext==mBox->pTail) {
    freeWith_isr_off_on(mBox->pHead);
    freeWith_isr_off_on(mBox->pTail);
    freeWith_isr_off_on(mBox);
    return status;
  }
  return NOT_EMPTY;  
}

msg * freeLastFromMB(mailbox *mBox) {
  msg *tempo;
  if(mBox->pTail->pPrevious != mBox->pHead) {
    //tempo = mBox->pHead->pNext;
    tempo = mBox->pTail->pPrevious;
    
    //tempo->pPrevious->pNext = tempo->pNext; // head.next = head.next.next
    tempo->pNext->pPrevious = tempo->pPrevious;
    
    // tempo->pNext->pPrevious = tempo->pPrevious; 
    tempo->pPrevious->pNext= tempo->pNext; 
    
    /*
    noden som extraheras refererar till sig självt
    */
    tempo->pNext = tempo;
    tempo->pPrevious = tempo;
    
    return tempo;
  }
  return NULL;
}

msg * extractionAnyMsg(msg * object) {
  if(object != NULL) { 
    object->pPrevious->pNext = object->pNext;
    object->pNext->pPrevious = object->pPrevious; 
    
    object->pNext = object;
    object->pPrevious = object;
    
    return object;
  }
  return NULL;
}

exception recieve_wait(mailbox *mBox, void *data) {
  volatile bool firstExe=TRUE;
  msg *temp;
  exception status = OK;
  set_isr(ISR_OFF);
  
  SaveContext();
  if(firstExe) {
    firstExe=FALSE;
    
      if(mBox->nMessages > 0 ) {
      //Vill inte peka på hela "sizeof structen" nDataSize=4 bytes då vi skickar int
      memcpy(data, mBox->pHead->pNext->pData, mBox->nDataSize);
      temp = freeLastFromMB(mBox);
      
      if(temp==NULL) {
        status=FAIL;
        LoadContext();
      }
      
      mBox->nMessages--;
      
    //  if(mBox->nBlockedMsg > 0 ) { //är msg wait_type?
      if(temp->pBlock != NULL) {
        
        extractionAnyObj(temp->pBlock);
        
        if(temp==NULL) {
          status = FAIL;
          LoadContext();
        }
        
        insertSortByDeadLine(readyList, temp->pBlock);
        Running = readyList->pHead->pNext->pTask;
        mBox->nBlockedMsg--;
        
//        freeWith_isr_off_on(temp->pData);
//        freeWith_isr_off_on(temp);
      } 
      else {
        freeWith_isr_off_on(temp->pData);
        freeWith_isr_off_on(temp);
       // mBox->nBlockedMsg++;
      }
    } else {
      msg * message = (msg *)secureCalloc(1, sizeof(msg)); //rätt
      
      if(message==NULL) {
        status=FAIL;
        LoadContext();
      }
      
       //VIKTIGT!
      message->pBlock=readyList->pHead->pNext;
      message->pData=data;
      
      insertMsgBox(mBox, message);
      mBox->nMessages--;
      mBox->nBlockedMsg--;
     // listobj *temp;
      //temp = extractionFirst(readyList);
      insertSortByDeadLine(waitList, extractionFirst(readyList));
      Running = readyList->pHead->pNext->pTask;
    }
    LoadContext();
  } else {
    
    if(deadline() <= ticks()) {
      int status = set_isr(ISR_OFF);
      //msg *tempo;
      temp = freeLastFromMB(mBox);
      //freeWith_isr_off_on(tempo->pData);
      freeWith_isr_off_on(temp);
      set_isr(status);
      
      mBox->nMessages++;
      mBox->nBlockedMsg++;
      
      status = DEADLINE_REACHED;
      return status;
    } 
  }
  return status;
}


exception send_wait(mailbox *mBox, void *data) {
  volatile bool firstExe=TRUE;
  msg *temp;
  exception status=OK;
  set_isr(ISR_OFF);;
  SaveContext();
  if(firstExe) {
    
    firstExe=FALSE;
    
    if(mBox->nMessages < 0) {
      temp = freeLastFromMB(mBox);
      
      if(temp==NULL) {
        status = FAIL;
        LoadContext();
      }
      
      memcpy(temp->pData, data, mBox->nDataSize); // rätt
      mBox->nBlockedMsg++;
      mBox->nMessages++;
      
    //  extractionAnyObj(temp->pBlock);
      /*
      Bättre lösning, undviker minnesläcka?
      */
      insertSortByDeadLine(readyList, extractionAnyObj(temp->pBlock));
      
      freeWith_isr_off_on(temp->pData);
      freeWith_isr_off_on(temp);

      Running = readyList->pHead->pNext->pTask;
      
    } else {
      msg * message = (msg *)secureCalloc(1, sizeof(msg));
      
      if(message==NULL) {
        freeWith_isr_off_on(message);
        status = FAIL;
        LoadContext();
      }
      
      message->pData = (void *)secureCalloc(1, mBox->nDataSize); // rätt
      
      if(message->pData==NULL) {
        freeWith_isr_off_on(message->pData);
        freeWith_isr_off_on(message);
        status=FAIL;
        LoadContext();
      }

      /*
      Lägger in "data" in i message pData.
      */
      memcpy(message->pData, data, mBox->nDataSize);
      message->pBlock=readyList->pHead->pNext;
      readyList->pHead->pNext->pMessage=message;
      
      insertMsgBox(mBox, message);
      mBox->nMessages++;
      mBox->nBlockedMsg++;
      
      /*
      listobj *temp;
      temp = extractionFirst(readyList);
      */
      insertSortByDeadLine(waitList, extractionAnyObj(message->pBlock));
      Running = readyList->pHead->pNext->pTask;
    }
    
    LoadContext();
    
  } else {
    
    /*
    //onödig???
    if(status==FAIL){
      return FAIL;
    }
    */
    
    if(deadline() <= ticks()) {
      int set = set_isr(ISR_OFF);;
      
      /*
      //...
      temp = readyList->pHead->pNext->pMessage;
      temp->pPrevious->pNext = temp->pNext;
      temp->pNext->pPrevious = temp->pPrevious;
      */
     
      temp = freeLastFromMB(mBox);
      
      if(temp==NULL) {
        status=FAIL;
        LoadContext();
      }
      
      freeWith_isr_off_on(temp->pData);
      freeWith_isr_off_on(temp);
      
      mBox->nBlockedMsg--;
      mBox->nMessages--;
      
      set_isr(set);
      return DEADLINE_REACHED;
    } 
  }
  return status;
}


/**
This call will send a Message to the specified Mailbox.
The sending task will continue execution after the call.
When the Mailbox is full, the oldest Message will be
overwritten. The send_no_wait call will imply a new
scheduling and possibly a context switch. Note:
send_wait and send_no_wait Messages shall not be
mixed in the same Mailbox.
Argument
*mBox: a pointer to the specified Mailbox.
*Data: a pointer to a memory area where the data of
the communicated Message is residing.
Return parameter
Description of the functionís status, i.e. FAIL/OK.

Notis:
nBlockedMsg påverkas endast i wait_funktionerna, inkrementeras i send, deinkrementeras i receive
*/
exception send_no_wait(mailbox *mBox, void *data) {
  volatile bool firstExe=TRUE;
  msg * message;
  exception status = OK;
  set_isr(ISR_OFF);
  SaveContext();
  if(firstExe) {
    firstExe=FALSE;
    //använd counters
    if(mBox->nMessages < 0) {
    //  message = mBox->pTail->pPrevious;
      message = freeLastFromMB(mBox);
      
      if(message==NULL) {
        status = FAIL;
        LoadContext();
      }
      
      memcpy(message->pData, data, mBox->nDataSize); // rätt
      
      insertSortByDeadLine(readyList, extractionAnyObj(message->pBlock));
      Running = readyList->pHead->pNext->pTask;
      
      freeWith_isr_off_on(message);
      
      mBox->nBlockedMsg++;
      mBox->nMessages++;
      LoadContext();
    }
    /*
    nBlockedMsg (negativ count)?
    nMessages (positiv count)?
    */
    else{
      message = (msg *)secureCalloc(1, sizeof(msg)); //rätt
      
      if(message==NULL) {
        freeWith_isr_off_on(message);
        status=FAIL;
        LoadContext();
      }
      
      message->pData = (void *)secureCalloc(1, mBox->nDataSize); //rätt
      
      if(message==NULL) {
        freeWith_isr_off_on(message->pData);
        freeWith_isr_off_on(message);
        status=FAIL;
        LoadContext();
      }
      memcpy(message->pData, data, mBox->nDataSize); // rätt
      
      /*
      Eftersom detta är i en "send"-funktion så behövs inte 
      absolutbeloppet. Räcker med if(mBox->nMessages >= mBox->nMaxMessages)
      */
     // if(mBox->nMaxMessages <= (mBox->nMessages)) {
      if((mBox->nMessages) >= mBox->nMaxMessages) {
        msg *tempo;
        tempo = freeLastFromMB(mBox);
        
        mBox->nMessages--; //not sure
        // free data?
        freeWith_isr_off_on(tempo->pData);
        freeWith_isr_off_on(tempo);
      }
      insertMsgBox(mBox, message);
      mBox->nMessages++;
    }
  }
  return status;
}

exception receive_no_wait(mailbox *mBox, void *data) {
  volatile bool firstExe=TRUE;
  exception status = OK;
  isr_off();
  SaveContext();
  if(firstExe) {
    firstExe=FALSE;
    status=FAIL;
    if(mBox->nMessages > 0 ) {
      status=OK;
      msg *temp;
      temp = freeLastFromMB(mBox);
      
      memcpy(data,temp->pData,mBox->nDataSize);
      
      if(temp==NULL) {
        status = FAIL;
        LoadContext();
      }
      
      mBox->nMessages--;
      if(mBox->nBlockedMsg > 0 ) { //är msg wait_type?
       // extractionAnyObj(temp->pBlock);
        insertSortByDeadLine(readyList, extractionAnyObj(temp->pBlock));
        Running = readyList->pHead->pNext->pTask;
        mBox->nBlockedMsg--;
        freeWith_isr_off_on(temp->pData);
        freeWith_isr_off_on(temp);
      } 
      else {
        freeWith_isr_off_on(temp->pData);
        freeWith_isr_off_on(temp);
      }
    }
    LoadContext();
  }
  return status;
}

