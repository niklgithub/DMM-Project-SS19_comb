#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>		//damit die spezifischen Befehle wie DDRB und PORTB erkannt werden
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "GUI.h"
#include "lcd.h"
#include "twi.h"
#include "dataflash.h"
#include "Logo.h"


void start_sequence (void)
{
	// Some counters
	uint16_t emg_offset = 0;
	uint8_t fb_x = 0;
	uint8_t fb_y = 0;


	Backlight_LED(BL_RED_ON | BL_GREEN_ON | BL_BLUE_ON);
	// Clear precautionally
	LCD_Clear();
	
	// Copy emglogo into framebuffer
	for (fb_y = 0; fb_y < 8; fb_y++)
	for (fb_x = 0; fb_x < 128; fb_x++)
	lcd_framebuffer[fb_y][fb_x] = pgm_read_byte(&Logo[emg_offset++]);
	
	// Mark pages 0-4 for update
	lcd_frameupdate = 0xff;
	
	LCD_Update();
	_delay_ms(3000);
	LCD_Clear();
	
	LCD_GotoXY(0,0);
	LCD_PutString_P(PSTR("DMM-Projekt Nr.4 \r\n"));
	LCD_PutString_P(PSTR("Geschwindigkeitsabhaeng.\r\n"));
	LCD_PutString_P(PSTR("Fahrwiderstand\r\n"));
	LCD_PutString_P(PSTR("Teilnehmer:\r\n"));
	LCD_PutString_P(PSTR("Niklas Kuhrmeyer\r\n"));
	LCD_PutString_P(PSTR("Philipp Leifer\r\n"));
	LCD_Update();
	_delay_ms(3000);
	LCD_Clear();	
	GUI_Logger();
}






void GUI_select(volatile uint8_t sel_gui,volatile uint8_t sel_dia,volatile uint8_t push_select)							   //sel_guide = selx
{
		
	switch(sel_gui)							//je nach selx wird Funktion ausgewaehlt die Menueband oben einfaerbt und darstellt
	{
		case 2:
		GUI_Logger();						//Auswahl Seite 1 "Logger"
		break;
		
		
		
		case 7:
		GUI_Kurve();						//Auswahl Seite 2 "Kurve"
		break;
		
		
		
		
		
		case 12:
		GUI_Masse();						//Auswahl Seite 3 "Masse"
		
		if ((PINA & 0x80) == 0)				//Gesamte if-Schleife: Positionierung der Pfeile in Abhaengigkeit vom Wert push_select
		{
			font_invert(1);
			LCD_GotoXY(push_select,3);
			LCD_PutChar(0x1E);
			LCD_Update();
			font_invert(0);
		}
		
		else if ((PINA & 0x40) == 0)
		{
			font_invert(1);
			LCD_GotoXY(push_select, 5);
			LCD_PutChar(0x1F);
			LCD_Update();
			font_invert(0);
		}
		
		else
		{
			font_invert(0);
			LCD_GotoXY(push_select,3);
			LCD_PutChar(0x1E);
			LCD_GotoXY(push_select, 5);
			LCD_PutChar(0x1F);
			LCD_Update();
		}
		break;
		
		default:
		break;
	}
}







void GUI_Logger(void)						//Ausgabe Menüband Seite 1 bzw. Logger
{
	PORTB &= ~(1<<PINB0);
	PORTB |= (1<<PINB1);
	
	LCD_Clear();
	LCD_GotoXY(0,0);
	font_invert(1);	
	LCD_PutString_P(PSTR("Logger"));	
	font_invert(0);
	LCD_GotoXY(8,0);
	LCD_PutString_P(PSTR("Kurve"));
	LCD_GotoXY(15,0);
	LCD_PutString_P(PSTR("Masse"));	
	
	LCD_Update();	
}


void GUI_Kurve(void)						//Ausgabe Menüband + Graphenstruktur Seite 2 bzw. Kurve
{
	PORTB &= ~(1<<PINB1);
	PORTB |= ((1<<PINB2)|(1<<PINB0));
	
	LCD_Clear();
	LCD_GotoXY(0,0);
	LCD_PutString_P(PSTR("Logger"));
	LCD_GotoXY(8,0);
	font_invert(1);
	LCD_PutString_P(PSTR("Kurve"));
	font_invert(0);
	LCD_GotoXY(15,0);
	LCD_PutString_P(PSTR("Masse"));	
	LCD_GotoXY(20,7);
	LCD_PutString_P(PSTR("v"));
	LCD_GotoXY(0,2);
	LCD_PutString_P(PSTR("P"));
	LCD_DrawLine(12,60,120,60,1);			//v-/X-Achse
	LCD_DrawLine(12,10,12,60,1);			//P-/Y-Achse
	LCD_DrawLine(10,13,12,10,1);			//Pfeillinie
	LCD_DrawLine(14,13,12,10,1);			//Pfeillinie
	LCD_DrawLine(117,58,120,60,1);			//Pfeillinie
	LCD_DrawLine(117,62,120,60,1);			//Pfeillinie
	LCD_Update();
}

void GUI_Masse(void)						//Ausgabe Menüband Seite 3 bzw. Masse
{
	PORTB &= ~(1<<PINB2);
	PORTB |= (1<<PINB1);
	
	LCD_Clear();
	LCD_GotoXY(0,0);
	LCD_PutString_P(PSTR("Logger"));
	LCD_GotoXY(8,0);
	LCD_PutString_P(PSTR("Kurve"));
	LCD_GotoXY(15,0);
	font_invert(1);
	LCD_PutString_P(PSTR("Masse"));
	font_invert(0);
	
	LCD_GotoXY(10,4);
	LCD_PutString_P(PSTR("in kg"));
}


