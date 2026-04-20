#include "TMP112.h"

#define TMP112_ADDR 0x49

uint8_t TMP112_13b=0;
int16_t TMP112_TEMP_Data;
float TMP112_TEMP;

void TMP112_Init(void)
{
	uint8_t Init_Data[2]={0x60,0xB0};
	I2C1_WriteBytes(TMP112_ADDR,TMP112_REG_CONF,Init_Data,2);
}

float TMP112_Read_Temp(void)
{
	uint8_t Data[2];
	I2C1_ReadBytes(TMP112_ADDR,TMP112_REG_TEMP,Data,2);
	if(TMP112_13b)
	{
		TMP112_TEMP_Data=(Data[0]<<5)|(Data[1]>>3);
		if(TMP112_TEMP_Data & 0x1000) 
			TMP112_TEMP=(float)((int16_t)(TMP112_TEMP_Data|0xE000))*0.0625f;
		else 
			TMP112_TEMP=(float)TMP112_TEMP_Data*0.0625f;
	}
	else
	{
		TMP112_TEMP_Data=(Data[0]<<4)|(Data[1]>>4);
		if(TMP112_TEMP_Data & 0x0800) 
			TMP112_TEMP=(float)((int16_t)(TMP112_TEMP_Data|0xF000))*0.0625f;
		else 
			TMP112_TEMP=(float)TMP112_TEMP_Data*0.0625f;
	}
	return TMP112_TEMP;
}

