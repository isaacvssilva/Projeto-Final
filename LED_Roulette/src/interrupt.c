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

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ISR_Handler
 *  Description:  
 * =====================================================================================
 */
void ISR_Handler(void){
	/* Verify active IRQ number */	
	switch (intcGetActivyNumber()){
    case GPIOINT3A:
		gpio3A_IsrHandler(); 
		break;
	case GPIOINT3B:
		gpio3B_IsrHandler(); 
		break;    
    case TINT7:
        timerIrqHandler();
	default:
		break;
	}
	/*AcknowledgeIRQ */
	intcAcknowledgeIRQ();
}
