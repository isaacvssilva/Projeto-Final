#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#include "soc_AM335x.h"
#include "hw_types.h"
#include "uart.h"
#include "gpio.h"
#include "timer.h"

typedef enum _ARMCortex_A8Interrupts{
    EMUINT,
    COMMTX,
    COMMRX,
    BENCH,
    ELM_IRQ,
    SSM_WFI_IRQ,
    SSM_IRQ,
    NMI,
    SEC_EVNT,
    L3DEBUG,
    L3APPINT,
    PRCMINT,
    EDMACOMPINT,
    EDMAMPERR,
    EDMAERRINT,
    WDT0INT,
    ADC_TSC_GENINT,
    USBSSINT,
    USBINT0,
    USBINT1,
    PRU_ICSS_EVTOUT0,
    PRU_ICSS_EVTOUT1,
    PRU_ICSS_EVTOUT2,
    PRU_ICSS_EVTOUT3,
    PRU_ICSS_EVTOUT4,
    PRU_ICSS_EVTOUT5,
    PRU_ICSS_EVTOUT6,
    PRU_ICSS_EVTOUT7,
    MMCSD1INT,
    MMCSD2INT,
    I2C2INT,
    eCAP0INT,
    GPIOINT2A,
    GPIOINT2B,
    USBWAKEUP,
    PCIeWAKEUP,
    LCDCINT,
    GFXINT,
    //Reserved,
    ePWM2INT=39,
    _3PGSWRXTHR0,//(RX_THRESH_PULSE)
    _3PGSWRXINT0,//(RX_THRESH_PULSE)
    _s3PGSWTXINT0, //TX_PULSE)
    _3PGSWMISC0,//(MISC_PULSE)
    UART3INT,
    UART4INT,
    UART5INT,
    eCAP1INT,
    PCIINT0,
    PCIINT1,
    PCIINT2,
    PCIINT3,
    DCAN0_INT0,
    DCAN0_INT1,
    DCAN0_PARITY,
    DCAN1_INT0,
    DCAN1_INT1,
    DCAN1_PARITY,
    ePWM0_TZINT,
    ePWM1_TZINT,
    ePWM2_TZINT,
    eCAP2INT,
    GPIOINT3A,
    GPIOINT3B,
    MMCSD0INT,
    McSPI0INT,
    TINT0,
    TINT1_1MS,
    TINT2,
    TINT3,
    I2C0INT,
    I2C1INT,
    UART0INT,
    UART1INT,
    UART2INT,
    RTCINT,
    RTCALARMINT,
    MBINT0,
    M3_TXEV,
    eQEP0INT,
    MCATXINT0,
    MCARXINT0,
    MCATXINT1,
    MCARXINT1,
    //Reserved,
    //Reserved,
    ePWM0INT=86,
    ePWM1INT,
    eQEP1INT,
    eQEP2INT,
    DMA_INTR_PIN2,
    WDT1INT,//(Public Watchdog)
    TINT4,
    TINT5,
    TINT6,
    TINT7,
    GPIOINT0A,
    GPIOINT0B,
    GPIOINT1A,
    GPIOINT1B,
    GPMCINT,
    DDRERR0,
    AES0_IRQ_S,
    AES0_IRQ_P,
    //Reserved,
    //Reserved,
    //Reserved,
    //Reserved,
    SHA_IRQ_S=108,
    SHA_IRQ_P,
    FPKA_SINTREQUEST_S,
    RNG_IRQ,
    TCERRINT0,
    TCERRINT1,
    TCERRINT2,
    ADC_TSC_PENINT,
    //Reserved,
    //Reserved,
    //Reserved,
    //Reserved,
    SMRFLX_MPU=120,//(subsystem)
    SMRFLX_Core,
    Reserved,
    DMA_INTR_PIN0,
    DMA_INTR_PIN1,
    McSPI1INT,
    //Reserved,
    //Reserved
}ARMCortex_A8Interrupts;

#define INTC_BASE       						0x48200000
#define INTC_SIR_IRQ    						0x48200040
#define INTC_CONTROL    						0x48200048
#define INTC_ILR        						0x48200100 
#define INTC_MIR_CLEAR0 						0x48200088
#define INTC_MIR_CLEAR1 						0x482000A8
#define INTC_MIR_CLEAR2 						0x482000C8
#define INTC_MIR_CLEAR3 						0x482000E8
#define INTC_SYSCONFIG							0x48200010
#define INTC_SYSSTATUS							0x48200014
#define INTC_THRESHOLD							0x48200068
//#include "bbb_regs.h"


/*-----------------------------------------------------------------------------
 *  Function prototype
 *-----------------------------------------------------------------------------*/
unsigned int intcGetActivyNumber(void);
void intcAcknowledgeIRQ(void);
void intcSetInterrupt(ARMCortex_A8Interrupts );
void ISR_Handler(void);
// Functions weak
void gpio3A_IsrHandler(void);
void gpio3B_IsrHandler(void);
void timerIrqHandler(void);
#endif