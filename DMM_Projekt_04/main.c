#ifndef F_CPU
#define F_CPU 16000000UL
#endif


#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "includes/lcd.h"
#include "includes/twi.h"
#include "includes/dataflash.h"
#include "includes/Joystick.h"
#include "includes//GUI.h"
#include "includes/counting.h"


#define wait_joy_button()       {LCD_GotoXY(20,7);  \
	LCD_PutChar(0x10); \
	LCD_Update();      \
while(((PINA)&0x08));while(!((PINA)&0x08));_delay_ms(20);while(((PINA)&0x08)); },

volatile int TimerOverflow = 0;

//variablen für Joystick
volatile static uint8_t selx = 2;
volatile static uint8_t sely = 0;
volatile static uint8_t push = 11;

volatile static uint8_t count = 0;

volatile static uint8_t mass_first;
volatile static uint8_t mass_second;
volatile static uint8_t mass_third;
static uint16_t mass = 0;

uint8_t input_loop = 0;

static unsigned int graph[125];
static unsigned int graph_copy[125];

uint8_t JoySelect_Flag = 0;

ISR(TIMER1_OVF_vect)
{
	TimerOverflow = 1;
}

ISR (PCINT0_vect)
{
	JoySelect_Flag = 1;
}



int main (void)
{
	cli();
	// set PA3-PA7 as input and activated internal Pull-Up
	DDRA  &= ~((1<<PINA3)|(1<<PINA4)|(1<<PINA5)|(1<<PINA6)|(1<<PINA7));				// PINA3-PINA7 als Eingänge definiert
	PORTA |=  ((1<<PINA3)|(1<<PINA4)|(1<<PINA5)|(1<<PINA6)|(1<<PINA7));				// PINA3-PINA7 werden auf HIGH gezogen
	
	
	DDRB = 0x0F;	// DDRB als Ausgang setzen
	PORTB |= 0x0F;	// Pins an PORTB auf High (beschaltung)
	
	PCICR |= (1 << PCIE0);						// Einschalten des Interrupt für PortA
	PCMSK0 |= ((1 << PCINT3)|(1<<PCINT4)|(1<<PCINT5)|(1<<PCINT6)|(1<<PCINT7));				// Interrupterkennung für PB4 und PB5
	
	TCCR1B = (1 << CS10);		// Prescaler
	TCCR1B = ~(1 << CS11);		// auf
	TCCR1B = (1 << CS12);		// 1024
	TIMSK1 = (1 << ICIE1);		// Timer1 Interrupt aktiviert
	
	LCD_Init();																				// Initialisierungsroutine des LCD
	LCD_Clear();																			// Löscht den Framebuffer und setzt den Cursor auf (0,0)
		
	start_sequence ();																		//Startbildschirm (Logo und Projektbeschreibung)	
	
	sei();																					// Einschalten des Interrupt
	
												
	

	
	while(1)																				// Endlosschleife	
	{	
		//Darstellungsbereich im Graphen
		const graph_x_bound = 103;											
		const graph_y_bound = 42;			
		
		int offset_x = 0, offset_x_clear = 0;
		int value_y = 0, value_y_clear = 0;	
		int c = 0;
	
	
	
		
		if (JoySelect_Flag == 1)															  //abfrage ob änderung am joystick erfolgt ist. Schleife nur mit Breakpoint ueberspringen da Zeitfunktion, immer einen pausedurchlauf debuggen sonst wird aenderung nicht erkannt
		{
			JoySelect(&selx, &sely, &push, &mass_first, &mass_second, &mass_third);			//Auslesen Joystick Rueckschreiben der gewünschten Werte auf übergebene Adressen 
			
			mass = mass_first + mass_second * 10 + mass_third * 100;						// Rueckspeichern des unter Masse eingestellten Wertes
			GUI_select(selx, sely, push);													 // Seitenausahl bzw. Menuepunkt
			
			JoySelect_Flag = 0;																 //Flag zur anzeige der Aenderung zuruecksetzen
		}
		
		
		
		//if (TimerOverflow == 1)
		//{
			//TimerOverflow = 0;
			counting(&count);																//Zaehlfunktion  => Speichert anzahl der While durchlaufe in count
		
		
		
//////////////////////////////////////////////////////DUMMY/////////////////////////////////////////////////////////////////////////////////		

//input loop waehlt anzuzeigendes element aus graph array aus 
//count fuellt array nur mit dummydaten 		
//wenn graph array 103 erreicht hat wird die anzeige verschoben 
													
													
//graph und graph copy arrays werden unten in switch case 7 an die Zeichnungsfunktion uebergeben 

		if (input_loop > graph_x_bound)										 
		{	
			
			//Werte au
			for (int i = 0; i < 103; i++)				
			{
				graph_copy[i] = graph[i];
			}
			
									
			for (int i = 1; i <= 104; i++)
			{				
				c = graph[i];
				graph[i-1] = c;
			}
			
			input_loop = input_loop - 1;
			graph[input_loop] = count;
						
		}			
		//}
		else
		{
			graph[input_loop] = count;												//input_loop   auch nur schleifenzaheler waehlt elemnt von Array aus 	dieser bekommtt dann den count wert 
		}
		
		input_loop = input_loop + 1;												//später mittels Timer-Interrupt
		
		
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////							
		
		
		
				
		switch(selx)																//Auswahl der einzelnen seiten selx wird von Joyselect geandert
		{
			
			
			case 2:																	//Ausgabe Seite 1 ohne Menüband
					LCD_GotoXY(1,3);
					LCD_PutString_P(PSTR("Geschwindigkeit:"));
					LCD_GotoXY(1,4);
					
					LCD_PutNumber(1, 2);
					
					LCD_GotoXY(4,4);
					LCD_PutString_P(PSTR("m/s"));
					LCD_GotoXY(1,5);
					LCD_PutString_P(PSTR("Leistung:"));
					LCD_GotoXY(1,6);
					
					LCD_PutNumber(2, 10);
					
					LCD_GotoXY(4,6);
					LCD_PutString_P(PSTR("W"));
					LCD_Update();			
					
			break;
		
		
			
			case 7:																	//Ausgabe Seite 2 ohne Menüband (Leistungskurve)
				
	
				
				//LCD_Clear(0);
				//if (TimerOverflow == 1)
				//{
					
					//Zeichnung des graphen 
					for (int a = 0; a < 103; a++)
					{
						value_y = 60 - graph[a];									//value_y bezieht wert aus array graph 
						offset_x = 13 + a;
						
						value_y_clear = 60 - graph_copy[a];							//value_y_clear  bezieht wert aus array graph copy
					
						
						LCD_DrawLine(offset_x,60,offset_x,value_y_clear,2);			// löschen des alten graphen	   mithilfe graph_copy array 	 graph_copy wird benoetigt um die gezeichneten Linien im Mode 2 von Draw line zu löschen damit nicht der ganze bildschirm gecleart wird
						
						
						LCD_DrawLine(offset_x,60,offset_x,value_y,1);				// zeichnen des neuen graphen		  mithilfe graph array 
					}
							
					LCD_Update();													//erst hier wird bildschirm aktualisiert und neu angezeigt
					TimerOverflow = 0;
			break;			
			
			
			
			case 12:																//Ausgabe Seite 3 ohne Menüband
				LCD_GotoXY(11, 4);
				LCD_PutNumber(mass_first, 10);				
				LCD_GotoXY(10, 4);
				LCD_PutNumber(mass_second, 10);			
				LCD_GotoXY(9, 4);
				LCD_PutNumber(mass_third, 10);
				LCD_Update();
				
			break;
			
						
			default:
			break;
		}
	}
		
	return 0;
}

	
	







			
			
		


