
#ifndef HANDLE_INT_H_
#define HANDLE_INT_H_

#include <stdint.h>		// ermöglicht die Verwendung von int-Datentypen mit definierter Bitzahl
#include <math.h>

uint8_t int_length(int16_t number);
void int16_to_string(char* str, int16_t num);

#endif