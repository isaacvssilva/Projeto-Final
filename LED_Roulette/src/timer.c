#include "timer.h"

bool flag_timer, use_interrupt;
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  disableWdt
 *  Description:  
 * =====================================================================================
 */
void disableWdt(void){
	HWREG(WDT_WSPR) = 0xAAAA;
	while((HWREG(WDT_WWPS) & (1<<4)));
	
	HWREG(WDT_WSPR) = 0x5555;
	while((HWREG(WDT_WWPS) & (1<<4)));
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  timerEnable
 *  Description:  
 * =====================================================================================
 */
void timerEnable(void){
    /* Wait for previous write to complete in TCLR */
	DMTimerWaitForWrite(0x1);

    /* Start the timer */
    HWREG(DMTIMER_TCLR) |= 0x1;
}/* -----  end of function timerEnable  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  timerDisable
 *  Description:  
 * =====================================================================================
 */
void timerDisable(void){
    /* Wait for previous write to complete in TCLR */
	DMTimerWaitForWrite(0x1);

    /* Start the timer */
    HWREG(DMTIMER_TCLR) &= ~(0x1);
}/* -----  end of function timerEnable  ----- */

void timerSetup(bool use_interrupt){
     /*  Clock enable for DMTIMER7 TRM 8.1.12.1.25 */
    HWREG(CM_PER_TIMER7_CLKCTRL) |= 0x2;

	/*  Check clock enable for DMTIMER7 TRM 8.1.12.1.25 */    
    while((HWREG(CM_PER_TIMER7_CLKCTRL) & 0x3) != 0x2);

     /* Interrupt mask TIMER7*/
    if(use_interrupt){
        intcSetInterrupt(TINT7);
        use_interrupt = true;
    }else{
        use_interrupt = false;
    }       
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  timerIrqHandler
 *  Description:  
 * =====================================================================================
 */
void timerIrqHandler(void){

    /* Clear the status of the interrupt flags */
	HWREG(DMTIMER_IRQSTATUS) = 0x2; 

	flag_timer = true;

    /* Stop the DMTimer */
	timerDisable();
}

void delay(unsigned int mSec){
    if(use_interrupt){
        unsigned int countVal = TIMER_OVERFLOW - (mSec * TIMER_1MS_COUNT);

        /* Wait for previous write to complete */
        DMTimerWaitForWrite(0x2);

        /* Load the register with the re-load value */
        HWREG(DMTIMER_TCRR) = countVal;
        
        flag_timer = false;

        /* Enable the DMTimer interrupts */
        HWREG(DMTIMER_IRQENABLE_SET) = 0x2; 

        /* Start the DMTimer */
        timerEnable();

        while(flag_timer == false);

        /* Disable the DMTimer interrupts */
        HWREG(DMTIMER_IRQENABLE_CLR) = 0x2;
    } else{
        while(mSec != 0){
            /* Wait for previous write to complete */
            DMTimerWaitForWrite(0x2);

            /* Set the counter value. */
            HWREG(DMTIMER_TCRR) = 0x0;

            timerEnable();

            while(HWREG(DMTIMER_TCRR) < TIMER_1MS_COUNT);

            /* Stop the timer */
            HWREG(DMTIMER_TCLR) &= ~(0x00000001u);

            mSec--;
        }
    }
}