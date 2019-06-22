#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>		//damit die spezifischen Befehle wie DDRB und PORTB erkannt werden
#include <util/delay.h> // um delays einfach zu erzeugen
#include <stdlib.h>

#include "Joystick.h"


void JoySelect(volatile uint8_t *selx, volatile uint8_t *sely, volatile uint8_t *push, volatile uint8_t *mass_first, volatile uint8_t *mass_second, volatile uint8_t *mass_third)
{	
	const uint8_t leftBound  =  2;
	const uint8_t rightBound = 12;
	const uint8_t highBound = 9;
	const uint8_t lowBound = 0;
	static uint8_t selectX = 2;
	
	static uint8_t selectY = 6;
	static uint8_t push_select = 11;
	static uint8_t select_flag = 0;
	
	static uint8_t mass_f = 0;
	static uint8_t mass_s = 0;
	static uint8_t mass_t = 0;

	 //Push 
	if ((PINA & 0x08) == 0 && select_flag == 0)
	{
		select_flag = 1;
		
		if (push_select > 9)
		{
			push_select = push_select - 1;
		}
		
		else if (push_select <= 9)
		{
			push_select = 11;
		}
	}

//x nach links	
	if ((PINA & 0x20)==0 && selectX>leftBound && select_flag == 0) //prüfe ob Bit 5 gesetzt ist und linker Anschlag nicht erreicht ist [PA5 (0b00100000)]
	{		
		selectX = selectX - 5;
		select_flag = 1;											// unterbindet mehrfachwahl bei gedrücktem Taster
	}
	
	
	
	//x nach rechts
	else if ((PINA & 0x10) == 0 && selectX < rightBound && select_flag == 0) //rechts PA4 (0b00010000)
	{		
		selectX = selectX + 5;		
		select_flag = 1;
	}
	
	
	//y nach oben 
	else if ((PINA & 0x80)==0 && selectY < highBound && select_flag == 0) //oben
	{
		switch(push_select)					   //abhaenig davon wie oft push select gewaehlt wird mit dem Fall die zu aendernde Stelle ausgewaehlt und um 1 inkrementiert 
		{
			case 11:
			if (mass_f < 9)
			{
				mass_f = mass_f + 1;					
			}			
			break;
			
			case 10:
			if (mass_s < 9)
			{
				mass_s = mass_s + 1;
			}			
			break;
			
			case 9:
			if (mass_t < 9)
			{
				mass_t = mass_t + 1;
			}			
			break;			
		}
		select_flag = 1;
	}
	
	
	//y nach unten 
	else if ((PINA & 0x40)==0 && selectY > lowBound && select_flag == 0)  //abhaenig davon wie oft push select gewaehlt wird mit dem Fall die zu aendernde Stelle ausgewaehlt und um 1 dekrementiert 
	{
		switch(push_select)
		{
			case 11:
			if (mass_f > 0)
			{
				mass_f = mass_f - 1;
			}			
			break;
			
			case 10:
			if (mass_s > 0)
			{
				mass_s = mass_s - 1;
			}
			break;
			
			case 9:
			if (mass_t > 0)
			{
				mass_t = mass_t - 1;
			}
			break;
		}
		select_flag = 1;
	}
	
	
	
	
	else if ((PINA & 0xF8) == 0xF8 && select_flag == 1)				// Wenn Taster losgelassen, dann erneute Betätigung möglich
	{
		select_flag = 0;
	}
	
	
		
	_delay_ms(10);																			// delay um das Prellen der Taster abzuwarten
	
	
	
	//Rueckschreiben der Daten auf uebergebene Adressen 
	*selx = selectX;
	*sely = selectY;
	*push = push_select;
	
	*mass_first = mass_f;
	*mass_second = mass_s;
	*mass_third = mass_t;
}