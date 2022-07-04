#ifndef INTERRUPTS
#define INTERRUPTS

/*
 * =====================================================================================
 *
 *       Filename:  interrupts
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/07/2022 22:52:55
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Isaac Vinicius
 *
 * =====================================================================================
 */

/* Interrupt */
#define INTC_SIR_IRQ    						0x040
#define INTC_CONTROL    						0x048
#define INTC_ILR        						0x100
#define INTC_MIR_CLEAR2 						0x0C8
#define INTC_MIR_CLEAR3 						0x0E8
#define INTC_SYSCONFIG							0x010
#define INTC_SYSSTATUS							0x014
#define INTC_THRESHOLD							0x068

#define GRUPO_A                                 98
#define GRUPO_B                                 99
#endif
