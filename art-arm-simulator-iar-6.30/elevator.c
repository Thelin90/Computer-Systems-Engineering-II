/*-------------------------------------------------------------------------*/
/* elevator.c                                                              */
/* @author: Simon Thelin, Oskar Svensson                                   */
/* @date: 2016-03-24                                                       */
/* @version  4.0                                                           */                                                                     
/*-------------------------------------------------------------------------*/
#include "kernel.h"
#include "dlist.h"
#include "assert.h"
#include "limits.h"
#include "kernel_hwdep.h"

mailbox * mail;

int level = 0;
int countElevatorFloors=0;

/*
The function if the elevator is going up
*/
void elevator_up(){
  int time = 0;
  
  while( time < 100){
    time++; 
  }
  level++;
  countElevatorFloors++;
 
}

/*
The function if the elevator is going down.
*/
void elevator_down(){
  int time = 0;
  while(time < 100){
    time++; 
  }
  level--;
  countElevatorFloors++;
}

/*
The function that will determine the direction of the elevator.
*/
void elevator_direction(int btn){
  if(level < btn){
    elevator_up();
  }else if(level > btn){
    elevator_down(); 
  }
}

void task_1(){
  int btn_elevator;
  recieve_wait(mail,&btn_elevator);
  elevator_direction(btn_elevator);
  wait(200);

  terminate();
}

void task_2(){
  int btn_elevator = (rand()%4)+1;
  send_no_wait(mail,&btn_elevator);

  terminate();
}

void task_3(){
  int btn_elevator;
  recieve_wait(mail,&btn_elevator);
  elevator_direction(btn_elevator);
  wait(2000);

  terminate();
}

void task_4(){
  int btn_elevator = (rand()%4)+1;
  send_no_wait(mail,&btn_elevator);

  terminate();
}

void task_5(){
  int btn_elevator;
  recieve_wait(mail,&btn_elevator);
  elevator_direction(btn_elevator);
  wait(20000);

  terminate();
}

void task_6(){
  int btn_elevator = (rand()%4)+1;
  send_no_wait(mail,&btn_elevator);

  terminate();
}


void task_7(){
  int btn_elevator;
  receive_no_wait(mail,&btn_elevator);
  elevator_direction(btn_elevator);
  wait(200000);

  terminate();
}

void task_8(){
  int btn_elevator = (rand()%4)+1;
  send_wait(mail,&btn_elevator);

  terminate();
}




void runElevator(){
  mail = create_mailbox(4,1);
  init_kernel();
  create_task(task_1,200);
  create_task(task_2,300);
  
  create_task(task_3,2000);
  create_task(task_4,3000);
  
  create_task(task_5,20000);
  create_task(task_6,30000);
  
  create_task(task_7,200000);
  create_task(task_8,300000);
  
  run(); 
}