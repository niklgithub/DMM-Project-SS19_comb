#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "music.h"

/* Sine lookup table for the PWM generator (pp = 32) */
static const PROGMEM unsigned char sine_lookup[]={
	0x10,0x13,0x16,0x19,0x1b,0x1d,0x1f,0x20,
	0x20,0x20,0x1f,0x1d,0x1b,0x19,0x16,0x13,
	0x10,0x0d,0x0a,0x07,0x05,0x03,0x01,0x00,
	0x00,0x00,0x01,0x03,0x05,0x07,0x0a,0x0d
};

const MUSIC_Note MUSIC_Tetris[] = { 
	{ MUSIC_E5, MUSIC_4 }, 
	{ MUSIC_B4, MUSIC_8 },
	{ MUSIC_C5, MUSIC_8 },
	{ MUSIC_D5, MUSIC_4 },
	{ MUSIC_C5, MUSIC_8 },
	{ MUSIC_D5, MUSIC_8 },
		
	{ MUSIC_A4, MUSIC_4 },
	{ MUSIC_A4, MUSIC_8 },
	{ MUSIC_C5, MUSIC_8 },
	{ MUSIC_E5, MUSIC_4 },
	{ MUSIC_D5, MUSIC_8 },
	{ MUSIC_C5, MUSIC_8 },
		
	{ MUSIC_B4, MUSIC_4 + MUSIC_8 },
	{ MUSIC_C5, MUSIC_8 },
	{ MUSIC_D5, MUSIC_4 },
	{ MUSIC_E5, MUSIC_4 },
					
	{ MUSIC_C5, MUSIC_4 },
	{ MUSIC_A4, MUSIC_4 },
	{ MUSIC_A4, MUSIC_8 },
	
	 MUSIC_END };

static void delay_ms(uint16_t delay);
static void pwm_init (void);
static void pwm_deinit (void);
static void pwm_gen(uint16_t tone, uint16_t periods);

void Music_PlayTrack (MUSIC_Track track) 
{
	pwm_init();
	
	while (!MUSIC_IS_END(*track)) {
		
		if (track->tone == 0) {			
			delay_ms(track->duration);
		} else {
			uint16_t periods = (uint32_t)track->duration * (uint32_t)track->tone / 1000;
			pwm_gen(track->tone, periods);
			
			delay_ms(MUSIC_32 / 2);
		}
		
		++track;
	}
	
	pwm_deinit();
}

static void delay_ms(uint16_t delay)
{
	while (delay--) 
		_delay_ms(1);	
}

void pwm_init (void) 
{
	//setup PWM timer
	DDRD |= _BV(PIND5);	// speaker pin => output
	TCCR1A = _BV(COM1A1) | _BV(COM1A0) | _BV(WGM10); //set on match
	TCCR1B = _BV(CS10) | _BV(WGM12); //fast PWM (8 bit), clk/1  source
	
	//setup sample timer (CTC Mode)	
	TCCR3A = 0; 
	TCCR3B = _BV(CS30) | _BV(WGM32); // clk/1
}

void pwm_deinit (void)
{	
	TCCR1A = 0;
	TCCR1B = 0;	
	TCCR3A = 0;
	TCCR3B = 0; 
}

void pwm_gen(uint16_t tone, uint16_t periods)
{
	//calculate single sine sample duration
	OCR3A = F_CPU / tone / sizeof(sine_lookup) - 1;
		
	for (uint16_t period=0; period < periods; ++period)  {
		for (uint8_t spl=0; spl < sizeof(sine_lookup); ++spl) {
			//wait for timer ready for next spl
			loop_until_bit_is_set(TIFR3, OCF1A);	
			TIFR3 = _BV(OCF1A); //clear OCF
			
			OCR1A = pgm_read_byte(sine_lookup + spl) + 0x70;
		}	
	}	
}