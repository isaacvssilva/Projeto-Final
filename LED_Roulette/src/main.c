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
#define GPIO1_28	1,28
#define GPIO1_16	1,16
#define GPIO1_17	1,17
//leds - expansor P8
#define GPIO1_13  	1,13
#define GPIO1_12 	1,12
#define GPIO1_15	1,15
#define GPIO1_14	1,14
#define GPIO1_29	1,29
#define GPIO2_1		2,1
//botoes - expansor P9
#define BUTTON_GPIO3_21 21
#define BUTTON_GPIO3_19 19

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
void sweep();
void pulse();
void move_led();
unsigned int leds[18]={GPIO1_12,GPIO1_13,GPIO1_14,GPIO1_15,GPIO1_16,GPIO1_17,GPIO1_28,GPIO1_29,GPIO2_1};
unsigned int button = 2;
unsigned int difficulty = 0;
unsigned int current_led = 0;
unsigned int delay_time = 0;
bool dir_flag = true; //true goes right, false goes left
bool game_ended = false;
bool is_win = false;
bool is_finished_selecting = false;

/*-----------------------FUNCOES PROVISORIAS--------------------------------------------*/

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  interruptSetup
 *  Description:  
 * =====================================================================================
 */
void interruptSetup(){
	/* Interrupt mask */
	HWREG(SOC_AINTC_REGS + INTC_MIR_CLEAR3) |= (1<<3);//(99 --> Bit 3 do 4º registrador (MIR CLEAR3))
	HWREG(SOC_AINTC_REGS + INTC_MIR_CLEAR3) |= (1<<2);
}

void gpioIsrHandler(void){
    /* Clear the status of the interrupt flags */
	HWREG(SOC_GPIO_3_REGS + GPIO_IRQSTATUS_1) = (1<<BUTTON_GPIO3_21);
	flag_gpio = true;
}
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  butConfig
 *  Description:  
 * =====================================================================================
 */
void interruptButton(){
    /* Setting interrupt GPIO pin. */
	HWREG(SOC_GPIO_3_REGS + GPIO_IRQSTATUS_SET_1) |= BUTTON_GPIO3_21;

	/* Enable interrupt generation on detection of a rising edge.*/
	HWREG(SOC_GPIO_3_REGS + GPIO_RISINGDETECT) |= BUTTON_GPIO3_21;
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
void Leds_Init(){
	//multiplexacao do modulo do pino referente aos leds como saida
	for(unsigned int x=0; x<17; x=+2){
		gpioPinMuxSetup(leds[x],leds[x+1], OUTPUT);
	}

	//setar direcao do pino
	for(unsigned int x=0; x<17; x++){
		gpioSetDirection(leds[x],leds[x+1], OUTPUT);
		ledOFF(leds[x],leds[x+1]);
	}
}
void Buttons_Init(){
	//multiplexacao do modulo do pino referente aos botoes
	gpioPinMuxSetup(GPIO3, BUTTON_GPIO3_21, INPUT);
	gpioPinMuxSetup(GPIO3, BUTTON_GPIO3_19, INPUT);

	//setar direcao botoes
	gpioSetDirection(GPIO3, BUTTON_GPIO3_21, INPUT);
	gpioSetDirection(GPIO3, BUTTON_GPIO3_19, INPUT);

	uartPutString(UART0,"GPIO INPUT Initialized\n",22);
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int main(void){
	/* Hardware setup */
	disableWdt();
	/*-----------------------------------------------------------------------------
	 *	initialize GPIO and UART modules
	 *-----------------------------------------------------------------------------*/
	for(gpioMod x=GPIO1; x<=GPIO3; x++){
		gpioInitModule(x);
	}
   	uartInitModule(UART0, 115200, STOP1, PARITY_NONE, FLOW_OFF);
	Leds_Init();
	Buttons_Init();
	sweep();
	pulse();
	//nivel de dificuldade
	difficulty = 0;
  	ledON(leds[(difficulty*2)],leds[(difficulty*2)+1]);
	delay(1000);

	while(true){
        if(game_ended==false){
		move_led();
		delay(delay_time);
		}
		else if(game_ended){
		uartPutString(UART0,"Game over\n",10);
		if(is_win){
		uartPutString(UART0,"you win\n",8);
			for(int i=0; i<5;i++){
				pulse();
				delay(100);
			}
		}
   		sweep();
		game_ended = false;
		is_win = false;
        delay(2000);
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

void sweep(){
	for(unsigned int i=0; i<17;i++){
		ledON(leds[i],leds[i+1]);
		delay(50);
		ledOFF(leds[i],leds[i+1]);
	}
	
	for(unsigned int i=16; i>=0;i-=2){
		ledON(leds[i],leds[i+1]);
		delay(50);
		ledOFF(leds[i],leds[i+1]);
	}
}

void pulse(){
	for(unsigned int i=0; i<17;i++){
		ledON(leds[i],leds[i+1]);
	}
	delay(100);
	for(unsigned int i=0; i<17;i++){
		ledOFF(leds[i],leds[i+1]);
	}
}

void move_led(){
  	ledOFF(leds[(current_led*2)],leds[(current_led*2)]+1);
	if(current_led == 8){
		dir_flag = false;
		current_led -= 1;
	}
	else if(current_led == 0){
		dir_flag = true;
		current_led += 1;
	}
	else if(dir_flag){
		current_led += 1;
	}
	else if(!dir_flag){
		current_led -= 1;
	}
	ledON(leds[(current_led*2)],leds[(current_led*2)]+1);
}

/*FUNCTION*-------------------------------------------------------
*
* Function Name : Delay
* Comments      :
*END*-----------------------------------------------------------*/
static void delay(int iTime){
	volatile unsigned int ra;
	for(ra = 0; ra < iTime; ra ++);
}
