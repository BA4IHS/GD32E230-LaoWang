#include "gd32e23x.h"
#include "systick.h"

volatile static float count_1us = 0;
volatile static float count_1ms = 0;

void systick_config(void)
{
	systick_clksource_set(SYSTICK_CLKSOURCE_HCLK_DIV8);
	count_1us = (float)SystemCoreClock/8000000;
	count_1ms = (float)count_1us * 1000;
}

void delay_1us(uint32_t count)
{
	uint32_t ctl;
	SysTick->LOAD = (uint32_t)(count * count_1us);
	SysTick->VAL = 0x0000U;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
	do{
			ctl = SysTick->CTRL;
	}while((ctl&SysTick_CTRL_ENABLE_Msk)&&!(ctl & SysTick_CTRL_COUNTFLAG_Msk));
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL = 0x0000U;
}

void delay_1ms(uint32_t count)
{
	uint32_t ctl;
	SysTick->LOAD = (uint32_t)(count * count_1ms);
	SysTick->VAL = 0x0000U;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
	do{
			ctl = SysTick->CTRL;
	}while((ctl&SysTick_CTRL_ENABLE_Msk)&&!(ctl & SysTick_CTRL_COUNTFLAG_Msk));
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL = 0x0000U;
}
