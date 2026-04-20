#ifndef __CLOCK_H
#define __CLOCK_H

#include "main.h"
#include "air32f10x_rcc.h"

extern RCC_ClocksTypeDef clocks;

void RCC_ClkConfiguration(void);


#endif
