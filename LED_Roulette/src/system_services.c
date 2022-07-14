#include "system_services.h"

int selectDificculty(unsigned int op){
	switch (op){
		case 0:
			return 800;
		case 1:
			return 500;
		case 2:
			return 300;
		case 3:
			return 250;
		case 4:
			return 200;
		case 5:
			return 150;
		case 6:
			return 100;
		case 7:
			return 75;
		case 8:
			return 50;			
	}	
	return 0;
}
