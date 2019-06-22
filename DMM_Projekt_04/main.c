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

#define LENGTH_TABLE 250


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

//Variablen Niklas
uint32_t time_sys = 0; //number of milliseconds since µC started
uint32_t time_syslast = 0; //number of milliseconds since µC started
uint32_t time_sensorlastnegedge = 0; //Sensing wont work within first time_debounce ms
uint32_t time_lasthigh = 0;
uint16_t time_debounce = 50;
int8_t flag_pcint = 0; //set on bouncing cycle rotation impulse
int8_t flag_turn = 0; //set on debounced cycle rotation impulse
int8_t lock_debounce = 0; //additional locking to debounce until time_sensorlastnegedge is updated
int8_t flag_time = 0; //to count time_sys in main function
int8_t flag_store = 0; //1 while recording new curve
uint16_t size_wheel = 2215; //wheels circumference in mm
uint32_t time_measarray[5] = {0};
uint8_t flag_measarray[5] = {0};
uint8_t speed_threshold = 1; // in m/s. Lower speeds ain't used for calculations
uint8_t speed_table[LENGTH_TABLE] = {0}; //8-bit representation: 0 -> 0 km/h; 255 -> (255*0.2) = 51 km/h
uint8_t power_table[LENGTH_TABLE] = {0}; //8-bit representation: 0 -> 0 W; 255 -> (255*2) = 510 W
double speed_current = 0; //speed from time 2 samples before
double acc_current = 0; //acceleration from time 2 samples before
double power_current = 0; //power in W from time 2 samples before
double mass_eff = 80; //total effective mass in kg: bike + rider + transformed inertia torque
double power_calc = 0; //power calculated by interpolation of measured curve
uint32_t time_syssec = 0;
int8_t state_sensor = -1;
//Variablen Niklas ENDE

double calc_power (double speed, double acceleration);

ISR(TIMER1_OVF_vect)
{
	TimerOverflow = 1;
}

ISR (PCINT0_vect)
{
	JoySelect_Flag = 1;
}

//ISR Niklas
ISR (PCINT2_vect)
{
	flag_pcint = 1;
}

ISR (TIMER0_COMPA_vect)
{
	flag_time++;
} 
//ISR Niklas ENDE

int main (void)
{
	//++++++++++++++++++++++++++++++++++++ SETUP ++++++++++++++++++++++++++++++++++++
	cli();
	// set PA3-PA7 as input and activated internal Pull-Up
	DDRA  &= ~((1<<PINA3)|(1<<PINA4)|(1<<PINA5)|(1<<PINA6)|(1<<PINA7));				// PINA3-PINA7 als Eingänge definiert
	PORTA |=  ((1<<PINA3)|(1<<PINA4)|(1<<PINA5)|(1<<PINA6)|(1<<PINA7));				// PINA3-PINA7 werden auf HIGH gezogen
	
	DDRC &= ~(1<<PINC0); // PB3 als Reedsensor-Eingang
	PORTC |= (1<<PINC0); // Sensor zieht Pin auf Masse
	
	
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
	
	init_pcint();
	init_sysclk();
												
	

	//++++++++++++++++++++++++++++++++++++ LOOP +++++++++++++++++++++++++++++++++++++
	while(1)																				// Endlosschleife	
	{	
		//Darstellungsbereich im Graphen
		const graph_x_bound = 103;											
		const graph_y_bound = 42;			
		
		int offset_x = 0, offset_x_clear = 0;
		int value_y = 0, value_y_clear = 0;	
		int c = 0;
		
		//main Niklas
		if((time_sys - (1000*time_syssec))>= 1000)
		{
			time_syssec++;
			PORTB ^= (1<<PINB0);
		}
		
		
		state_sensor = !(PINC & (1<<PINC0)); //inverted because of pull-up on PA3. state_sensor == 1 means magnet is near reed contact
		if(flag_pcint || (state_sensor == 1))
		{
			if(time_sys > time_lasthigh + time_debounce)
			{
				flag_turn = 1;
			}
			time_lasthigh = time_sys;
			flag_pcint = 0;
		}
		
		
		if (flag_turn) //to do: add switch to activate storing a new curve
		{
			PORTB ^= (1<<PINB3);
			
			
			time_measarray[0] = time_measarray[1];
			time_measarray[1] = time_measarray[2];
			time_measarray[2] = time_measarray[3];
			time_measarray[3] = time_measarray[4];
			time_measarray[4] = time_sys;
			
			flag_measarray[0] = 1;
			flag_measarray[1] = flag_measarray[2];
			flag_measarray[2] = flag_measarray[3];
			flag_measarray[3] = flag_measarray[4];
			if ( (time_measarray[4]-time_measarray[3]) > 0 ) //prevent divide by 0 errors
			{
				flag_measarray[4] = ((((double) size_wheel/((double) time_measarray[4]-(double) time_measarray[3]))) >= (double) speed_threshold);
			}
			else
			{
				flag_measarray[4] = 0;
			}
			
			if (flag_measarray[0] && flag_measarray[1] && flag_measarray[2] && flag_measarray[3] && flag_measarray[4])
			{
				//differentiation based on method of central difference
				speed_current = 2*size_wheel/(time_measarray[3]-time_measarray[1]); //speed in mm/ms = m/s
				acc_current = 2*1000*size_wheel*(((1/(time_measarray[4]-time_measarray[2]))-(1/(time_measarray[2]-time_measarray[0])))/(time_measarray[3]-time_measarray[1])); //acceleration in 1000mm/(ms)²=m/s²
				power_current = (-1) * mass_eff * speed_current * acc_current; //power in W
				
				if (flag_store)
				{
					store_table (speed_current , power_current);
				}
				
				else
				{
					power_calc = calc_power (speed_current , acc_current);
				}
			}
			
			
			flag_turn = 0;
		}
		
		/*Dont write main-code after following block!*/
		do //to realise 1 ms system period
		{
		} while (flag_time==0);
		time_sys += flag_time; // checking if flag_time > 1 would mean main takes too much time
		flag_time = 0;
		//main Niklas ENDE
			
		
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

	
//Funktionen Niklas
void init_pcint (void)
{
	cli();
	PCICR |= (1<<PCIE2); //enable interrupt on pinchange
	PCMSK2 = (1<<PCINT16); //mask for port pin PB5
	sei();
}
void init_sysclk (void) //sysclk uses Timer0 with output comparision
{
	cli();
	TCCR0B |= ((1<<CS01)|(1<<CS00)); //prescaler 64 (for 16 MHz quarz)
	OCR0A = 250; //compare to this. (250 for 1 ms)
	TIMSK0 |= (1<<OCIE0A); //enable interrupt
	sei();
}

void store_table (double speed, double power)
{
	static uint8_t speed_last = 255;
	static uint8_t i = 0;
	uint8_t speed_int = 0;
	uint8_t power_int = 0;
	
	speed = speed * (3.6 * 5); //convert speed from m/s to 0.5-km/h, 255 => 51 km/h
	if(speed > 255) speed = 255;
	if(speed < 0) speed = 0;
	speed_int = (int) speed;
	power = 0.5 * power; //convert power from W to 2-W; 255 => 510 W
	if(power < 0) power = 0;
	if(power > 255) power = 255;
	power_int = (int) power;
	
	if (speed_int < speed_last) //store new speed-power-point only if speed is lower than before
	{
		speed_table[i] = speed_int;
		power_table[i] = power_int;
		speed_last = speed_int;
		
		if (i < LENGTH_TABLE) //prevent overflow of i
		{
			i++;
		}
	}
}

double calc_power (double speed, double acceleration)
{
	double power_tot = 0;
	double power_frict = 0;
	double power_acc = 0;
	uint8_t speed_int = 0;
	uint8_t i = 0;
	
	speed_int = (int) (speed * 3.6 * 5);
	
	while (speed_int < speed_table[i])
	{
		i++;
	}
	
	if (speed_int == speed_table[i])
	{
		power_frict = power_table[i];
	}
	
	else
	{
		power_frict = (((double) power_table[i-1] - (double) power_table[i])/((double) speed_table[i-1] - (double) speed_table[i])) * ((double) speed_int - (double) speed_table[i]) + (double) power_table[i];
		power_frict = power_frict * 2; //convert so that 255 => 510 W
	}
	
	power_acc = mass_eff * speed * acceleration;
	
	power_tot = power_frict + power_acc;
	
	return power_tot;
}
//Funktionen Niklas ENDE	







			
			
		


