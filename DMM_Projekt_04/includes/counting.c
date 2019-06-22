#include <stdlib.h>
#include <avr/io.h>	
#include <util/delay.h>

#include "counting.h"


void counting(volatile uint8_t *n)
{
	static uint8_t i = 0, b = 0;
	
	if ((i < 30) && (b == 0))
	{
		i++;
		
		if (i == 30)
		{
			b = 1;
		}
	}
	
	else if ((i > 0) && (b == 1))
	{
		i--;
		
		if (i == 0)
		{
			b = 0;
		}
	}
	
	*n = i;
	
}