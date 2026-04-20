#ifndef __TMP112_H
#define __TMP112_H
#include "main.h"

#define TMP112_REG_TEMP    0x00    // 温度寄存器（只读）
#define TMP112_REG_CONF    0x01    // 配置寄存器（读写）
#define TMP112_REG_TLOW    0x02    // 温度下限寄存器（读写）
#define TMP112_REG_THIGH   0x03    // 温度上限寄存器（读写）

void TMP112_Init(void);
float TMP112_Read_Temp(void);

extern int16_t TMP112_TEMP_Data;
extern float TMP112_TEMP;

#endif
