#include <stdlib.h>
#include <avr/io.h>
#include "avr/eeprom.h"

#define GRUEN_MINUS !(PINC & (1<<PC0))
#define GRUEN_PLUS !(PINC & (1<<PC1))
#define ROT_MINUS !(PINC & (1<<PC2))
#define ROT_PLUS !(PINC & (1<<PC3))
#define BLAU_MINUS !(PINC & (1<<PC4))
#define BLAU_PLUS !(PINC & (1<<PC5))



int main(void)
{
	uint8_t counterPWM = 0; //counter für PWM
	uint16_t clockteiler = 0;//counter zur verlangsamug der tasterabfragrate auf 1000HZ
	uint8_t blau = 255;//farbwert blau
	uint8_t rot = 255;//farbwert rot
	uint8_t gruen = 255;//farbwert gruen
	uint8_t rotaus = 0;//rot de/aktiviert
	uint8_t blauaus = 0;//blau de/aktiviert
	uint8_t gruenaus = 0;//gruen de/aktiviert
	uint16_t timertasten [6] = {0, 0, 0, 0, 0, 0};
	uint8_t tasten [6] = {0, 0, 0, 0, 0, 0};
	DDRD |= (1<<PD0) | (1<<PD1) | (1<<PD2);//aktivierung ausgänge rgb
	DDRC &= ~((1<<PC0) | (1<<PC1) | (1<<PC2) | (1<<PC3) | (1<<PC4) | (1<<PC5));//aktivierung eingänge taster
	while(1)
	{ 	
		
		
		if (clockteiler == 12500){//reset counter
			clockteiler = 0;
		}
			
			
		if(counterPWM == 0){
			if((rot > 0) & (rotaus == 0)){
				PORTD |= (1<<PD1);
			}//ende rot ein
			if((blau > 0) & (blauaus == 0)){
				PORTD |= (1<<PD2);
			}
			if((gruen > 0) & (gruenaus == 0)){
				PORTD |= (1<<PD0);
			}//ende gruen ein
			
			
			
		}//ende counter pwm
		
		if(counterPWM > gruen){//gruen ausschalten
			PORTD &= ~(1<<PD0);
		}
		
		if(counterPWM > blau){//blau ausschalten
			PORTD &= ~(1<<PD2);
		}
		
		if(counterPWM > rot){//rot ausschalten
			PORTD &= ~(1<<PD1);
		}
		counterPWM++;
		clockteiler++;
		
		
		
		
		
		

		
		if (clockteiler == 12500){//reduzierung auf 650 hz
			
			if ((GRUEN_MINUS == 1) & (ROT_MINUS == 1) & (BLAU_MINUS == 1)){
				//save to eprom------------------------------------------------
			}//ende save eeprom
			else if ((GRUEN_PLUS == 1) & (ROT_PLUS == 1) & (BLAU_PLUS == 1)){
				//clera eeprom-------------------------------------------------
			}//clear eeprom
			else if(1 == 1){
			if (GRUEN_MINUS == 1){
				if((timertasten[0] < 650) & (timertasten[0] != 0)){
					gruenaus = 1;
					timertasten[0] = 0;
				}
				else{
					gruen--;
					tasten[0] = 1;
				}
			}//ende gruen-
			if (GRUEN_PLUS == 1){
				if((timertasten[1] < 650) & (timertasten[1] != 0)){
					gruenaus = 0;
					tasten[0] = 0;
					timertasten[1] = 0;
				}
				else{
					gruen++;
					tasten[1] = 1;
				}
			}//ende gruen+
			if (ROT_MINUS == 1){
				if((timertasten[2] < 650) & (timertasten[2] != 0)){
					rotaus = 1;
					timertasten[2] = 0;
				}
				else{
					rot--;
					tasten[2] = 1;
				}
			}//ende rot-
			if (ROT_PLUS == 1){
				if((timertasten[3] <650) & (timertasten[3] != 0)){
					rotaus = 0;
					timertasten[3] = 0;
				}
				else{
					rot++;
					tasten[3] = 1;
					
				}
			}//ende rot+
			if (BLAU_MINUS == 1){
				if((timertasten[4] < 650) & (timertasten[4] != 0)){
					blauaus = 1;
					timertasten[4] = 0;
				}
				else{
					blau--;
					tasten[4] = 1;
				}
			}//ende blau-
			if (BLAU_PLUS == 1){
				if((timertasten[5] < 650) & (timertasten[5] != 0)){
					blauaus  = 0;
					timertasten[5] = 0;
				}
				else{
					blau++;
					tasten[5] = 1;
				}
			}//ende blau+
		}//end if +/-
		
		if(tasten[0] == 1){
			if(GRUEN_MINUS == 0){
				timertasten[0]++;
			}
			if(timertasten[0] == 10){
				timertasten[0] = 0;
				tasten[0] = 0;
			}
		}
		
		if(tasten[1] == 1){
			if(GRUEN_PLUS == 0){
				timertasten[1]++;
			}
			if(timertasten[1] == 10){
				timertasten[1] = 0;
				tasten[1] = 0;
			}
		}
		
		if(tasten[2] == 1){
			if(ROT_MINUS == 0){
				timertasten[2]++;
			}
			if(timertasten[2] == 10){
				timertasten[2] = 0;
				tasten[2] = 0;
			}
		}
		
		if(tasten[3] == 1){
			if(ROT_PLUS == 0){
				timertasten[3]++;
			}
			if(timertasten[3] == 10){
				timertasten[3] = 0;
				tasten[3] = 0;
			}
		}
		
		if(tasten[4] == 1){
			if(BLAU_MINUS == 0){
				timertasten[4]++;
			}
			if(timertasten[4] == 10){
				timertasten[4] = 0;
				tasten[4] = 0;
			}
		}
		
		if(tasten[5] == 1){
			if(BLAU_PLUS == 0){
				timertasten[5]++;
			}
			if(timertasten[5] == 10){
				timertasten[5] = 0;
				tasten[5] = 0;
			}
		}
			
			
		}//ende clockteiler


	} //ende while
}//ende main




