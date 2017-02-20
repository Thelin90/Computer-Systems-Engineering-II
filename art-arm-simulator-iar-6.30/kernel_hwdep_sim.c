/* 2002-11-19 Wolfgang Svensson*/
#include <stdlib.h>
#include "kernel_hwdep.h"

/*-------------------------------------------------------------------------*/
/* uint set_isr( uint newCSR )  - Change interrupt ON/OFF                  */
/*	ints ON/OFF on entry						   */
/*	ints ON/OFF on exit						   */
/* HW dependent	  							   */
/* Argument: New CSR							   */
/* Returns: Old CSR							   */
/*-------------------------------------------------------------------------*/
void isr_on(void)
{}

void isr_off(void)
{}
unsigned int set_isr( unsigned int newCSR ) {

	return newCSR;
}


void timer0_start(void)
{

}
