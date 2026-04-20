#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>


void systick_config(void);
void delay_1ms(uint32_t count);
void delay_1us(uint32_t count);

#define delay_us(x)			delay_1us(x)
#define delay_ms(x)			delay_1ms(x)

#endif
