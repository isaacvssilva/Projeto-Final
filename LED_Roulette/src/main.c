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
#include	"interrupt.h"
#include	"timer.h"

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
void gpioSetup(void);
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


void gpio3A_IsrHandler(void){

	gpioClearStatusIRQ(GPIO3, 12, GRUPO_A);
}

void gpio3B_IsrHandler(void){

	gpioClearStatusIRQ(GPIO3, 12, GRUPO_B);
}

void Leds_Init(){
	//multiplexacao do modulo do pino referente aos leds como saida
	for(unsigned int x=0; x<=17; x+=2){
		gpioPinMuxSetup(leds[x],leds[x+1], OUTPUT);
	}

	//setar direcao do pino
	for(unsigned int x=0; x<=17; x+=2){
		gpioSetDirection(leds[x],leds[x+1], OUTPUT);
		ledOFF(leds[x],leds[x+1]);
	}
}
void Buttons_Init(){
	for(int i = BUTTON_GPIO3_19; i <= BUTTON_GPIO3_21; i+=2){
		gpioPinMuxSetup(GPIO3, i, INPUT);
		gpioSetDirection(GPIO3, i, INPUT);
		gpioSetEdge(GPIO3, i, RISING);
	}
	gpioSetPinIterrupt(GPIO3, BUTTON_GPIO3_19, GRUPO_A);
	gpioSetPinIterrupt(GPIO3, BUTTON_GPIO3_21, GRUPO_B);
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
	gpioSetup();	
	timerSetup(TRUE);
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

void gpioSetup(void){
	for(gpioMod x=GPIO1; x<=GPIO3; x++){
		gpioInitModule(x);
	}
	intcSetInterrupt(GPIOINT3A);
	intcSetInterrupt(GPIOINT3B);
}

void ledON(gpioMod mod, ucPinNumber pin){
	gpioSetPinValue(mod, pin, HIGH);	
}/* -----  end of function ledON  ----- */

void ledOFF(gpioMod mod,  ucPinNumber pin ){
	gpioSetPinValue(mod, pin, LOW);
}/* -----  end of function ledOFF  ----- */

void sweep(){
	for(unsigned int i=0; i<=17;i+=2){
		ledON(leds[i],leds[i+1]);
		delay(50);
		ledOFF(leds[i],leds[i+1]);
	}
	
	for(unsigned int i=17;i>=0;i-=2){
		ledON(leds[i],leds[i+1]);
		delay(50);
		ledOFF(leds[i],leds[i+1]);
	}
}

void pulse(){
	for(unsigned int i=0; i<=17;i++){
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
