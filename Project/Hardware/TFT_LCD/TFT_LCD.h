#ifndef __SPI_LCD_H
#define __SPI_LCD_H 			   
#include "main.h"

#define LCD_SPI_Send(x)				SPI0_Write(x)
#define TFT_LCD_LED(x)				gpio_bit_write(GPIOA,GPIO_PIN_11,(bit_status)x)	
#define TFT_LCD_IM12(x)				gpio_bit_write(GPIOF,GPIO_PIN_7,(bit_status)x)	
#define TFT_LCD_RST(x)				gpio_bit_write(GPIOA,GPIO_PIN_12,(bit_status)x)	
#define TFT_LCD_CS(x)					gpio_bit_write(GPIOA,GPIO_PIN_15,(bit_status)x)	
#define TFT_LCD_DC(x)					gpio_bit_write(GPIOB,GPIO_PIN_3,(bit_status)x)	
#define TFT_LCD_WR(x)					gpio_bit_write(GPIOB,GPIO_PIN_4,(bit_status)x)	
#define TFT_LCD_WD(x)					gpio_bit_write(GPIOF,GPIO_PIN_6,(bit_status)x)

void TFT_LCD_Init(void);
void Backlight_PWM_Init(void);
void LCD_Chip_Init(void);
void LCD_Clear(uint16_t Color);
void LCD_Display(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const uint8_t *pix_data);
void LCD_Display_Color(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint16_t Color);
void LCD_Display_DMA_Interrupt(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint8_t *pix_data,uint8_t New);
void LCD_Display_DrawPoint(uint16_t x, uint16_t y, uint16_t color);


#define BLACK         	 0x0000		//��ɫ
#define WHITE         	 0xFFFF		//��ɫ
#define RED           	 0xF800		//��ɫ
#define GREEN         	 0x07E0 	//��ɫ
#define BLUE          	 0x001F 	//��ɫ 
#define BRED             0XF81F
#define GRED 			       0XFFE0
#define GBLUE			       0X07FF
#define MAGENTA       	 0xF81F
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			     0XBC40 //��ɫ
#define BRRED 			     0XFC07 //�غ�ɫ
#define GRAY  			     0X8430 //��ɫ
#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
#define LIGHTGREEN     	 0X841F //ǳ��ɫ
#define LGRAY 			     0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ
#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)

void Rounded_Rectangle(uint16_t X0,uint16_t Y0,uint16_t X1,uint16_t Y1,uint16_t R,uint16_t Colour);
void LCD_Display(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const uint8_t *pix_data);
void LCD_ShowIntNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint16_t fc,uint16_t bc,uint8_t sizey);
void LCD_ShowString(uint16_t x,uint16_t y,const char *p,uint16_t fc,uint16_t bc,uint8_t sizey);
void LCD_Show_Float(uint16_t x,uint16_t y,float num,uint8_t len,uint8_t decimals,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t Mode,uint16_t SUM_width);

#endif

