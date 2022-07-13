#include "system_services.h"

void menuUser(void){
	uartPutString(UART0,"\033[H\033[J\r", 8);
	uartPutString(UART0, "----------------------------------------\n\r \
	MENU USER\n\r----------------------------------------\n\r", 97);
	uartPutString(UART0,"0. 1000ms\n\r1. 100ms\n\r2. 200ms\n\r3. 300ms\n\r4. 400ms\n\r", 51);
	uartPutString(UART0,"5. 500ms\n\r6. 600ms\n\r7. 700ms\n\r8. 800ms\n\r9. 900ms\n\r", 50);
}

// void selectOperation(unsigned int op){
// 	switch (op){
// 		case 0:
// 			periodoMs = 1000;
// 		break;
// 		case 1:
// 			periodoMs = 100;
// 		break;
// 		case 2:
// 			periodoMs = 200;
// 		break;
// 		case 3:
// 			periodoMs = 300;
// 		break;
// 		case 4:
// 			periodoMs = 400;
// 		break;
// 		case 5:
// 			periodoMs = 500;
// 		break;
// 		case 6:
// 			periodoMs = 600;
// 		break;
// 		case 7:
// 			periodoMs = 700;
// 		break;
// 		case 8:
// 			periodoMs = 800;
// 		break;
// 		case 9:
// 			periodoMs = 900;
// 		break;	
// 	default:
// 		uartPutString(UART0,"Please, enter a number from 0 to 9!\n\r", 37);
// 		flagBlinkLED = false;
// 		return;
// 	}
// 	flagBlinkLED = true;
// }
