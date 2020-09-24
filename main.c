#include <stdlib.h>
#include <avr/io.h>
#include <avr/eeprom.h>
/*  24.9.2020
 * 	Dübendorf
 * 	Simon Chatziparaskewas
 *	verstellbares RGB PWM Mit 5 speicherplätzen
 *
 * BENUTZUNG: 
 * - taste halten senkt den zugehörigen farbwert. 2x in einer sekunde drücken schaltet farbe aus
 * + taste halten erhöht den zugehörigen farbwert. 2x in einer sekunde schaltet farbe wider ein farbwert bei einschaltung beträgt 0
 * ist der maximale farbwert in eine richtung erreicht wird zurückgesetzt also 255-> 0 und 0->255
 * drücken aller + tasten gleichzeitig setzt den gewälten speicherplatz auf vorgegebene farbe (weiss)(kann geändert werden)
 * drücken aller - tasten gleichzeitig:
 * wird losgelassen wenn farbe weiss wird zum nächsten speicherplatz gewechselt
 * wird losgelassen wenn farbe rot wird die aktuell eingestellte farbe (nicht der indikator) im aktuellen speicher gespeichert
 * 
 * bei einem neustart wird der erste speicherplatz geladen
 * 
 * 
 * */

//define taster
#define GRUEN_MINUS !(PINB & (1<<PB6))
#define GRUEN_PLUS !(PIND & (1<<PD4))
#define ROT_MINUS !(PIND & (1<<PD6))
#define ROT_PLUS !(PIND & (1<<PD3))
#define BLAU_MINUS !(PINB & (1<<PB7))
#define BLAU_PLUS !(PIND & (1<<PD5))

//define zustände
//an
#define ROT PORTB |= (1<<PB2)
#define GRUEN PORTB |= (1<<PB1)
#define BLAU PORTB |= (1<<PB3)
//aus
#define NROT PORTB &= ~(1<<PB2)
#define NGRUEN PORTB &= ~(1<<PB1)
#define NBLAU PORTB &= ~(1<<PB3)



int main(void)
{
	
	uint8_t counterPWM = 0; //counter für PWM
	uint16_t clockteiler = 0;//counter zur verlangsamug der tasterabfragrate 
	uint16_t teilwert = 2000;//widerholungen des pwm`s pro abfrage tasten (kann geändert werden kleiner = sneller...)
	uint8_t blau = 0;//farbwert blau
	uint8_t rot = 0;//farbwert rot
	uint8_t gruen =0;//farbwert gruen
	uint8_t rotaus = 0;//rot de/aktiviert
	uint8_t blauaus = 0;//blau de/aktiviert
	uint8_t gruenaus = 0;//gruen de/aktiviert
	uint16_t timertasten [6] = {0, 0, 0, 0, 0, 0};//array für counter der einzelnen tasten zur zeiterkennung
	uint8_t tasten [6] = {0, 0, 0, 0, 0, 0};//taste innerhalb einer sekunde betätigt ja nein
	uint16_t mencounter1 = 0;//counter zur verlangsamung des mencounter2`s
	uint8_t mencounter2 = 0;//counter zur definierung von er zeit im "speicherwechsel/speichern menue
	uint8_t select = 0;//zustand des obigen menues bei loslassen der tasten
	uint8_t rotaddr = 0;//adresse rot im speicher mal8
	uint8_t gruenaddr = 1;//adresse gruen im speicher mal8
	uint8_t blauaddr = 2;//adresse blau im speicher mal8
	uint8_t safestate = 0;//zähler zur begrenzung der speicherplatzmenge
	

	
	
	DDRB |= (1<<PB1) | (1<<PB2) | (1<<PB3);//aktivierung ausgänge rgb
	DDRD &= ~((1<<PD3) | (1<<PD4) | (1<<PD5) | (1<<PD6));//aktivierung eingänge taster
	DDRB &= ~((1<<PB6) | (1<<PB7));//aktivierung taster
	
	rot = eeprom_read_byte((uint8_t*)(rotaddr * 8));
	gruen = eeprom_read_byte((uint8_t*)(gruenaddr * 8));//laden es 1. speicherplatzes
	blau = eeprom_read_byte((uint8_t*)(blauaddr * 8));
	
	while(1)
	{ 	
		
		
		if (clockteiler == teilwert){//reset counter zur verlangsamung der tasterabfragrate
			clockteiler = 0;
		}
		
		
		//bei ausschalten einer farbe farbwert auf null setzen
		if(rotaus == 1){
			rot = 0;
		}	
		if(blauaus == 1){
			blau = 0;
		}
		if(gruenaus == 1){
			gruen = 0;
		}	
			
			
			
			//einschalten der farben bei pwm wert null sovern farbe nicht ausgeschaltet
		if(counterPWM == 0){
			if(rot > 0){
				ROT;
			}//ende rot ein
			if(blau > 0){
				BLAU;
			}//ende blau ein
			if(gruen > 0){
				GRUEN;
			}//ende gruen ein
			
			
			
		}//ende counter pwm
		
		if(counterPWM > gruen){//gruen ausschalten bei ueberschreiten pwm wert
			NGRUEN;
		}
		
		if(counterPWM > blau){//blau ausschalten bei ueberschreiten pwm wert
			NBLAU;
		}
		
		if(counterPWM > rot){//rot ausschalten bei ueberschreiten pwm wert
			NROT;
		}
		
		
		//pwm und verlangsamung der taster +1
		counterPWM++;
		clockteiler++;
		
		
		
		
		
		

		
		if (clockteiler == teilwert){//reduzierung der abfragrate taster wird bei erreichtem definiertem wert ausgefuert
			
			if ((GRUEN_MINUS == 1) & (ROT_MINUS == 1) & (BLAU_MINUS == 1)){//alle -tasten gedrueckt
				select = 1;
				BLAU;
				ROT;
				GRUEN;
				
				
				while((GRUEN_MINUS == 1) & (ROT_MINUS == 1) & (BLAU_MINUS == 1)){// while zur auswählung des modus (speicher wechseln/speichern)
					mencounter1++;
					if(mencounter1 == 8000){
						mencounter1 = 0;
						mencounter2++;
					}
					if(mencounter2 > 10){
						NGRUEN;
						ROT;
						NBLAU;
						select = 2;
					}
				}//ende while modus select
				
				

				if(select == 2){//speichern der aktuellen farbe
					eeprom_write_byte((uint8_t*)(rotaddr * 8), rot);
					eeprom_write_byte((uint8_t*)(gruenaddr * 8), gruen);
					eeprom_write_byte((uint8_t*)(blauaddr * 8), blau);
					//erset der benutzten counter
					mencounter2 = 0;
					mencounter1 = 0;
					select = 0;
					rotaus = 0;
					blauaus = 0;
					gruenaus = 0;
				}//ende if speichern aktueller wert
				
				
				if(select == 1){//wechseln des speicherplatzes
					rotaddr = rotaddr + 3;
					gruenaddr = gruenaddr + 3;
					blauaddr = blauaddr + 3;
					safestate++;
					if(safestate == 5){//begrenzung der anzahl speicherplaetze
						safestate = 0;
						rotaddr = 0;
						gruenaddr = 1;
						blauaddr = 2;
					}
					//laden sed neuen speicherplatzwertes
					rot = eeprom_read_byte((uint8_t*)(rotaddr * 8));
					gruen = eeprom_read_byte((uint8_t*)(gruenaddr * 8));
					blau = eeprom_read_byte((uint8_t*)(blauaddr * 8));
					//reset der counter
					mencounter2 = 0;
					mencounter1 = 0;
					select = 0;
					//reset der deaktivierten farben
					rotaus = 0;
					blauaus = 0;
					gruenaus = 0;
				}
			}//ende save eeprom
			else if ((GRUEN_PLUS == 1) & (ROT_PLUS == 1) & (BLAU_PLUS == 1)){
				while((GRUEN_PLUS == 1) & (ROT_PLUS == 1) & (BLAU_PLUS == 1)){//alle + tasten gedrueckt
					ROT;
					NBLAU;
					NGRUEN;
				}
				//speicherplatz mit vorgegebenem wert ueberschreiben
				eeprom_write_byte((uint8_t*)(rotaddr * 8), 255);
				eeprom_write_byte((uint8_t*)(gruenaddr * 8), 255);
				eeprom_write_byte((uint8_t*)(blauaddr * 8), 255);
				//reset der deaktivierten farben
				rotaus = 0;
				blauaus = 0;
				gruenaus = 0;
			}//ende reset eeprom zu befiniertem fert
			
			else if(1 == 1){//abfrage der tasten nur  wenn nicht alle +/-
			if (GRUEN_MINUS == 1){//taste gruen minus
				if((timertasten[0] < 650) & (timertasten[0] != 0)){//erkennung ob taste innerhalb 1sek zum 2. mal gedrückt
					gruenaus = 1;
					timertasten[0] = 0;
				}
				else{//wenn nicht zum 2. mal in 1 sek gedrueckt vert ferrinngern
					gruen--;
					tasten[0] = 1;
				}
			}//ende gruen-
			
			//aufbau der restlichen tasten wie be gruen-
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
			//ende der tasten
		
		
		//zaeler fuer die erkennung der sekunde jede taste einzeln 
		if(tasten[0] == 1){//wird ausgeführet wenn taste gedrückt wurde
			if(GRUEN_MINUS == 0){
				timertasten[0]++;
			}
			if(timertasten[0] == 10){//wenn zählwert erreicht wird wider zurückgesetzt wird forher gedrückt gilt taste als doppeldruck
				timertasten[0] = 0;
				tasten[0] = 0;
			}
		}//ende taste gruenminus
		
		//nachfolgende tasten gleiche funktion wie bei gruen minus
		
		if(tasten[1] == 1){
			if(GRUEN_PLUS == 0){
				timertasten[1]++;
			}
			if(timertasten[1] == 10){
				timertasten[1] = 0;
				tasten[1] = 0;
			}
		}//ende gruenplus
		
		if(tasten[2] == 1){
			if(ROT_MINUS == 0){
				timertasten[2]++;
			}
			if(timertasten[2] == 10){
				timertasten[2] = 0;
				tasten[2] = 0;
			}
		}//ende rotminus
		
		if(tasten[3] == 1){
			if(ROT_PLUS == 0){
				timertasten[3]++;
			}
			if(timertasten[3] == 10){
				timertasten[3] = 0;
				tasten[3] = 0;
			}
		}//ende rotplus
		
		if(tasten[4] == 1){
			if(BLAU_MINUS == 0){
				timertasten[4]++;
			}
			if(timertasten[4] == 10){
				timertasten[4] = 0;
				tasten[4] = 0;
			}
		}//ende blauminus
		
		if(tasten[5] == 1){
			if(BLAU_PLUS == 0){
				timertasten[5]++;
			}
			if(timertasten[5] == 10){
				timertasten[5] = 0;
				tasten[5] = 0;
			}
		}//ende blaupluss
			
			}//end if wenn nicht alle +/-
		}//ende clockteiler


	} //ende while
}//ende main




