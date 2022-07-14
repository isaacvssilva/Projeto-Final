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
//#include 	"system_services.h"

/*****************************************************************************
**                INTERNAL MACRO DEFINITIONS
*****************************************************************************/
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


/*****************************************************************************
**                INTERNAL FUNCTION PROTOTYPES
*****************************************************************************/

void gpioSetup();
void ledON(gpioMod  ,ucPinNumber );
void ledOFF(gpioMod ,ucPinNumber );
void sweep();
void pulse();
void move_led();
void button_pressed();
//void selectDifficulty(unsigned int op);
unsigned int leds[18]={GPIO1_12,GPIO1_13,GPIO1_14,GPIO1_15,GPIO1_16,GPIO1_17,GPIO1_28,GPIO1_29,GPIO2_1};
unsigned int difficulty = 0;
unsigned int current_led = 0;
unsigned int delay_time =1000;
bool dir_flag = true; //true goes right, false goes left
bool game_ended = false;
bool is_win = false;
bool is_selecting = true;

/*-----------------------FUNCOES PROVISORIAS--------------------------------------------*/


void gpio3A_IsrHandler(void){
	gpioClearStatusIRQ(GPIO3, BUTTON_GPIO3_19, GRUPO_A);
	uartClearBuffer(UART0); 
	/*menuUser();
	char op = uartGetC(UART0);
	uartPutC(UART0, op);
	uartPutC(UART0, '\n');
	uartPutC(UART0, '\r');
	selectDifficulty(op-'0');
	*/
	if(is_selecting){
		if(difficulty<8)
			difficulty++;
		else
			difficulty=0;
	}else if(!is_selecting && !game_ended){
		uartPutString(UART0, "Selecao de dificuldade:\n", 24);
		is_selecting=true;
	}

}

void gpio3B_IsrHandler(void){
	gpioClearStatusIRQ(GPIO3, 	BUTTON_GPIO3_21, GRUPO_B);
	if(is_selecting){
		uartPutString(UART0, "Dificuldade selecionada:\n", 25);
		uartPutC(UART0, difficulty+'0');
		delay_time=(1000-(difficulty*100));
		is_selecting=false;
		game_ended=false;
	}else if(!is_selecting && !game_ended){
		button_pressed();
	}

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
	// for(unsigned int i = BUTTON_GPIO3_19; i <= BUTTON_GPIO3_21; i+=2){
	// 	gpioPinMuxSetup(GPIO3, i, INPUT);
	// 	gpioSetDirection(GPIO3, i, INPUT);
	// 	gpioSetEdge(GPIO3, i, RISING);
	// }
	gpioPinMuxSetup(GPIO3, BUTTON_GPIO3_19, INPUT);
	gpioPinMuxSetup(GPIO3, BUTTON_GPIO3_21, INPUT);
	gpioSetDirection(GPIO3, BUTTON_GPIO3_19, INPUT);
	gpioSetDirection(GPIO3, BUTTON_GPIO3_21, INPUT);
	gpioSetEdge(GPIO3, BUTTON_GPIO3_19, RISING);
	gpioSetEdge(GPIO3, BUTTON_GPIO3_21, RISING);
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
	timerSetup(true);
	Leds_Init();
	Buttons_Init();
	sweep();
	pulse();

	while(true){
		//nivel de dificuldade
		while(is_selecting){
			ledON(leds[(difficulty*2)],leds[(difficulty*2)+1]);
			delay(500);	
		}
        if(game_ended==false){
			move_led();
			delay(delay_time);
		}else if(game_ended){	
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
}/* ----------  end of function main  ---------- */

void gpioSetup(){
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
	
	for(int i=17;i>=0;i-=2){
		ledON(leds[i-1],leds[i]);
		delay(50);
		ledOFF(leds[i-1],leds[i]);
	}
}

void pulse(){
	for(unsigned int i=0; i<=17;i+=2)
		ledON(leds[i],leds[i+1]);
	delay(100);
	for(unsigned int i=0; i<=17;i+=2)
		ledOFF(leds[i],leds[i+1]);
}

void move_led(){
  	ledOFF(leds[current_led*2],leds[(current_led*2)+1]);
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
	ledON(leds[current_led*2],leds[(current_led*2)+1]);
}

void button_pressed(){
  //Serial.println("Button pressed on LED: "+String(current_led));
	uartPutString(UART0, "Button pressed on LED: ", 23);
	uartPutC(UART0, current_led+'0');
	uartPutString(UART0, "\n\r", 2);
	game_ended = true;
	if(current_led==4){
		is_win = true;
	}
	else if(current_led != 4){
		is_win = false;
	}
 	current_led = 0;
 	delay(500);
}
/*
void selectDifficulty(unsigned int op){
	switch (op){
		case 0:
			difficulty = 0;
		break;
		case 1:
			difficulty =difficulty*10;
		break;
		case 2:
			difficulty = 200;
		break;
		case 3:
			difficulty = 300;
		break;
		case 4:
			difficulty = 400;
		break;
		case 5:
			difficulty = 500;
		break;
		case 6:
			difficulty = 600;
		break;
		case 7:
			difficulty = 700;
		break;
		case 8:
			difficulty = 800;
		break;
		case 9:
			difficulty = 900;
		break;	
	default:
		uartPutString(UART0,"Please, enter a number from 0 to 9!\n\r", 37);
		gpio3A_IsrHandler();
		return;
	}
}
*/
