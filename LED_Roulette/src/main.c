/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/07/2022 20:05:55
 *       Revision:  none
 *       Compiler:  arm-none-eabi-gcc
 *
 *         Author:  Isaac Vinicius, Hugo Bessa, Gislan Souza
 *   Organization:  UFC-Quixadá
 *
 * =====================================================================================
 */

#include	"gpio.h"
#include	"uart.h"
#include	"interrupts.h"


/*****************************************************************************
**                INTERNAL MACRO DEFINITIONS
*****************************************************************************/
#define TIME		2000000

//leds - expansor P9
#define GPIO1_28	28
#define GPIO1_16	16
#define GPIO1_17	17

//botoes - expansor P9
#define BUTTON_GPIO3_21 21
#define BUTTON_GPIO3_19 19

//leds - expansor P8
#define GPIO1_13 	13
#define GPIO1_12 	12
#define GPIO1_15	15
#define GPIO1_14	14
#define GPIO2_1		1
#define GPIO1_29	29

bool flag_gpio;

// state machine
typedef enum _state{
	SEQ1=1,
	SEQ2,
	SEQ3,
	SEQ4,
	SEQ5
}state;

/*****************************************************************************
**                INTERNAL FUNCTION PROTOTYPES
*****************************************************************************/
static void delay(int);
void ledON(gpioMod  ,ucPinNumber );
void ledOFF(gpioMod ,ucPinNumber );
	
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int main(void){
	/* Hardware setup */
	disableWdt();
	
	unsigned int count=0; 
	ucPinNumber pin=22;
	
	/*-----------------------------------------------------------------------------
	 *  initialize GPIO and UART modules
	 *-----------------------------------------------------------------------------*/
	gpioInitModule(GPIO1);
	gpioInitModule(GPIO2);
	gpioInitModule(GPIO3);

   	uartInitModule(UART0, 115200, STOP1, PARITY_NONE, FLOW_OFF);

	/*-----------------------------------------------------------------------------
	 *  initialize pin of mudule
	 *-----------------------------------------------------------------------------*/
	for(count=pin; count<25; count++){
		gpioPinMuxSetup(GPIO1, pin, OUTPUT);
		delay(1000);
	}

	gpioPinMuxSetup(GPIO1, 12, INPUT);

	//multiplexacao do modulo do pino referente aos leds como saida
	gpioPinMuxSetup(GPIO1, GPIO1_28, OUTPUT);
	gpioPinMuxSetup(GPIO1, GPIO1_16, OUTPUT);
	gpioPinMuxSetup(GPIO1, GPIO1_17, OUTPUT);
	gpioPinMuxSetup(GPIO1, GPIO1_12, OUTPUT);
	gpioPinMuxSetup(GPIO1, GPIO1_13, OUTPUT);
	gpioPinMuxSetup(GPIO1, GPIO1_15, OUTPUT);
	gpioPinMuxSetup(GPIO1, GPIO1_14, OUTPUT);
	gpioPinMuxSetup(GPIO2, GPIO2_1, OUTPUT);
	gpioPinMuxSetup(GPIO1, GPIO1_29, OUTPUT);

	//multiplexacao do modulo do pino referente aos botoes
	gpioPinMuxSetup(GPIO3, BUTTON_GPIO3_21, INPUT);
	gpioPinMuxSetup(GPIO3, BUTTON_GPIO3_19, INPUT);

	/*-----------------------------------------------------------------------------
	 *  set pin direction 
	 *-----------------------------------------------------------------------------*/
	for(count=pin; count<25; count++){
		gpioSetDirection(GPIO1, pin, OUTPUT);
		delay(1000);
	}

	gpioSetDirection(GPIO1, 12, INPUT);
	//leds
	gpioSetDirection(GPIO1, GPIO1_28, OUTPUT);
	gpioSetDirection(GPIO1, GPIO1_16, OUTPUT);
	gpioSetDirection(GPIO1, GPIO1_17, OUTPUT);
	gpioSetDirection(GPIO1, GPIO1_12, OUTPUT);
	gpioSetDirection(GPIO1, GPIO1_13, OUTPUT);
	gpioSetDirection(GPIO1, GPIO1_15, OUTPUT);
	gpioSetDirection(GPIO1, GPIO1_14, OUTPUT);
	gpioSetDirection(GPIO2, GPIO2_1, OUTPUT);
	gpioSetDirection(GPIO1, GPIO1_29, OUTPUT);
	
	//botoes
	gpioSetDirection(GPIO3, BUTTON_GPIO3_21, INPUT);
	gpioSetDirection(GPIO3, BUTTON_GPIO3_19, INPUT);

    /*-----------------------------------------------------------------------------
     *  set pin in LOW level
     *-----------------------------------------------------------------------------*/
    for(count=pin; count<25; count++){
        ledOFF(GPIO1, count);
		delay(1000);
	}
	
	uartPutString(UART0,"GPIO INPUT Initialized\n",22);

	while(true){
		if(HWREG(SOC_GPIO_3_REGS + GPIO_DATAIN) & (1<<BUTTON_GPIO3_19)){
			ledON(GPIO1, pin);
			ledON(GPIO2,GPIO2_1);
		}else{
			ledOFF(GPIO1, pin);
			ledOFF(GPIO2, GPIO2_1);
		}
	}

	return(0);
} /* ----------  end of function main  ---------- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ledON
 *  Description:  
 * =====================================================================================
 */
void ledON(gpioMod mod, ucPinNumber pin){
	gpioSetPinValue(mod, pin, HIGH);	
}/* -----  end of function ledON  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ledOFF
 *  Description:  
 * =====================================================================================
 */
void ledOFF(gpioMod mod,  ucPinNumber pin ){
	gpioSetPinValue(mod, pin, LOW);
}/* -----  end of function ledOFF  ----- */

/*FUNCTION*-------------------------------------------------------
*
* Function Name : Delay
* Comments      :
*END*-----------------------------------------------------------*/
static void delay(int iTime){
	volatile unsigned int ra;
	for(ra = 0; ra < iTime; ra ++);
}


/*-----------------------FUNCOES PROVISORIAS--------------------------------------------*/

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  interruptSetup
 *  Description:  
 * =====================================================================================
 */
void interruptSetup(){
	/* Interrupt mask */
	HWREG(SOC_AINTC_REGS + INTC_MIR_CLEAR3) |= (1<<3);//(98 --> Bit 2 do 4º registrador (MIR CLEAR3))
	
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  butConfig
 *  Description:  
 * =====================================================================================
 */
void interruptButton(){
    /* Setting interrupt GPIO pin. */
	HWREG(SOC_GPIO_3_REGS + GPIO_IRQSTATUS_1) |= BUTTON_GPIO3_21;

	/* Enable interrupt generation on detection of a rising edge.*/
	HWREG(SOC_GPIO_3_REGS + GPIO_RISINGDETECT) |= BUTTON_GPIO3_21;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  gpioIsrHandler
 *  Description:  
 * =====================================================================================
 */
void gpioIsrHandler(void){

    /* Clear the status of the interrupt flags */
	HWREG(SOC_GPIO_3_REGS + GPIO_IRQSTATUS_1) = (1<<BUTTON_GPIO3_21);
	flag_gpio = true;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ISR_Handler
 *  Description:  
 * =====================================================================================
 */
void ISR_Handler(void){
	/* Verify active IRQ number */
	unsigned int irq_number = HWREG(SOC_AINTC_REGS + INTC_SIR_IRQ) & 0x7f;

	if(irq_number == GRUPO_B){
		gpioIsrHandler();
	}
	/* acknowledge IRQ */
	HWREG(INTC_CONTROL) = 0x1;
}
