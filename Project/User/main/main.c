#include "main.h"
#include <stdio.h>
#include "math.h"

uint8_t TEST_Data[20] = {0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA};
uint8_t TEST_Data1[1024];
uint8_t TEST_Data2=0x66;
/*
* app1 全屏动画演示
* app2 I2C设备扫描
*
*/
int app_int = 1;   //APP

/* 已知I2C设备名称查找 */
static const char* I2C_Get_Device_Name(uint8_t addr)
{
	switch(addr)
	{
		case 0x08: return "PCA9536";
		case 0x10: return "VEML6040";
		case 0x18: return "MCP9808";
		case 0x1D: return "ADXL345";
		case 0x20: return "PCA9555";
		case 0x29: return "VL53L0X";
		case 0x30: return "SW3518";
		case 0x38: return "AHT20";
		case 0x3C: return "SSD1306";
		case 0x40: return "INA219";
		case 0x44: return "SHT3x";
		case 0x48: return "TMP112";
		case 0x50: return "AT24Cxx";
		case 0x51: return "RV3028";
		case 0x57: return "AT24Cxx";
		case 0x5A: return "MLX90614";
		case 0x60: return "Si7021";
		case 0x68: return "DS1307/MPU6050";
		case 0x69: return "MPU6050";
		case 0x76: return "BMP280";
		case 0x77: return "BME280/BMP180";
		default:   return "";
	}
}

/*i2c扫描程序*/
void I2C_Scan_Display(void)
{
	char buf[32];
	uint8_t i, row;
	uint16_t addr_color;
	static uint8_t last_count = 0xFF;
	static uint8_t last_addr[10];

	/* 执行扫描 */
	Software_IIC_SCAN();

	/* 对比上次结果，没变化就跳过屏幕刷新 */
	if(IIC_ADDN == last_count)
	{
		uint8_t changed = 0;
		for(i = 0; i < IIC_ADDN; i++)
		{
			if(IIC_ADDT[i] != last_addr[i])
			{
				changed = 1;
				break;
			}
		}
		if(!changed)
		{
			delay_ms(3000);
			return;
		}
	}

	/* 记录本次结果 */
	last_count = IIC_ADDN;
	for(i = 0; i < IIC_ADDN; i++)
		last_addr[i] = IIC_ADDT[i];

	/* 只有结果变化时才清屏重绘 */
	LCD_Clear(BLACK);

	/* 标题 - 青色 */
	LCD_ShowString(24, 4, "I2C Scanner", CYAN, BLACK, 32);

	/* 分隔线 - 灰色 */
	LCD_ShowString(0, 36, "------------------------", GRAY, BLACK, 16);

	/* 扫描结果 - 绿色 */
	sprintf(buf, "Found: %d device(s)", IIC_ADDN);
	LCD_ShowString(0, 52, buf, GREEN, BLACK, 24);

	/* 分隔线 */
	LCD_ShowString(0, 78, "------------------------", GRAY, BLACK, 16);

	if(IIC_ADDN == 0)
	{
		LCD_ShowString(24, 110, "No Device", RED, BLACK, 32);
		LCD_ShowString(20, 150, "Check connection", YELLOW, BLACK, 16);
	}
	else
	{
		for(i = 0; i < IIC_ADDN && i < 10; i++)
		{
			row = i;

			if(IIC_ADDT[i] < 0x20)
				addr_color = YELLOW;
			else if(IIC_ADDT[i] < 0x50)
				addr_color = GREEN;
			else if(IIC_ADDT[i] < 0x78)
				addr_color = CYAN;
			else
				addr_color = RED;

			sprintf(buf, "0x%02X", IIC_ADDT[i]);
			LCD_ShowString(4, 94 + row * 22, buf, addr_color, BLACK, 16);

			const char *name = I2C_Get_Device_Name(IIC_ADDT[i]);
			if(name[0] != '\0')
				LCD_ShowString(52, 94 + row * 22, name, WHITE, BLACK, 16);
			else
				LCD_ShowString(52, 94 + row * 22, "Unknown", GRAY, BLACK, 16);
		}
	}
	/* 等待间隔，避免反复扫描 */
	delay_ms(1000);
}



/* 全屏动画演示入口 (定义在 animation_demo.c) */
extern void Animation_Demo(void);


int main(void)
{
	/*初始化*/
	RCU_Init();
	systick_config();
	Hardware_SPI0_Init();
	Hardware_SPI1_Init();
	Software_IIC_Init();
	NorFlash_25Q_Init();
	NorFlash_25Q_Read_Buffer(62255,TEST_Data1,1024);
	TFT_LCD_Init();
	/*初始化*/
	
	
	LCD_Display_Color(0,0,240,240,BLACK); //清屏
	while(1)
	{
			switch(app_int)
	{
		case 1: 
			Animation_Demo();
		break;
		case 2:	
			I2C_Scan_Display();
		break;
	}
	}

}

