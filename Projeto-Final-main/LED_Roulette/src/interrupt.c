/*
 * =====================================================================================
 *
 *       Filename:  interrupt.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/07/2022 20:05:55
 *       Revision:  none
 *       Compiler:  arm-none-eabi-gcc
 *
 *         Author:  Hugo Bessa
 *   Organization:  UFC-Quixadá
 *
 * =====================================================================================
 */
#include "interrupt.h"


void intcSetInterrupt(ARMCortex_A8Interrupts typeInterrupt){
    unsigned addr_temp;
    unsigned int INTCReg = typeInterrupt >> 5;
    unsigned int bit = typeInterrupt & 0x1F;

    switch (INTCReg){
    case 0:
        addr_temp = INTC_MIR_CLEAR0;
        break;
    case 1:
        addr_temp = INTC_MIR_CLEAR1;
    case 2:
        addr_temp = INTC_MIR_CLEAR2;
    case 3:
        addr_temp = INTC_MIR_CLEAR3;
    default:
        break;
    }
    HWREG(addr_temp) = 1 << bit;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  IntcGetActivyNumber
 *  Description:  
 * =====================================================================================
 */
unsigned int intcGetActivyNumber(void){
    return  HWREG(INTC_SIR_IRQ) & 0x7f;
} 

void intcAcknowledgeIRQ(void){
    HWREG(INTC_CONTROL) = 0x1;
}


