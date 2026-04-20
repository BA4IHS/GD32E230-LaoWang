#include "TFT_LCD.h"
#include "LCD_Font.h"
#include "math.h"

uint8_t X_Offset=0;
uint8_t Y_Offset=0;


#define TFT_LCD_LED(x)				gpio_bit_write(GPIOA,GPIO_PIN_11,(bit_status)x)	
#define TFT_LCD_IM12(x)				gpio_bit_write(GPIOF,GPIO_PIN_7,(bit_status)x)	
#define TFT_LCD_RST(x)				gpio_bit_write(GPIOA,GPIO_PIN_12,(bit_status)x)	
#define TFT_LCD_CS(x)					gpio_bit_write(GPIOA,GPIO_PIN_15,(bit_status)x)	
#define TFT_LCD_DC(x)					gpio_bit_write(GPIOB,GPIO_PIN_3,(bit_status)x)	
#define TFT_LCD_WR(x)					gpio_bit_write(GPIOB,GPIO_PIN_4,(bit_status)x)	
#define TFT_LCD_WD(x)					gpio_bit_write(GPIOF,GPIO_PIN_6,(bit_status)x)	


void TFT_LCD_Init(void)
{
//开启时钟
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOF);
//GPIOA
	gpio_mode_set(GPIOA,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
	gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
	gpio_bit_reset(GPIOA,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
//GPIOA
	gpio_mode_set(GPIOA,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15);
	gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15);
	gpio_bit_reset(GPIOA,GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15);
//GPIOB	
	gpio_mode_set(GPIOB,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_PIN_4);
	gpio_output_options_set(GPIOB,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_4);
	gpio_bit_reset(GPIOB,GPIO_PIN_4);
//GPIOF
	gpio_mode_set(GPIOF,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_PIN_6|GPIO_PIN_7);
	gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_6|GPIO_PIN_7);
	gpio_bit_reset(GPIOF,GPIO_PIN_7);	
//初始化
	TFT_LCD_IM12(1);			//SPI模式
	TFT_LCD_CS(1);
	TFT_LCD_DC(1);
	TFT_LCD_RST(0);	
	delay_ms(100);
	TFT_LCD_RST(1);	
	delay_ms(200);
	LCD_Chip_Init();
	TFT_LCD_LED(1);
}

void Backlight_PWM_Init(void)
{

}

void LCD_Send_CMD_1B(uint8_t CMD)
{
	TFT_LCD_WR(0);
	TFT_LCD_CS(0);   
	LCD_SPI_Send(CMD); 
	TFT_LCD_CS(1);
	TFT_LCD_WR(1);
}

void LCD_Send_Data_1B(uint8_t data)
{
	TFT_LCD_CS(0);
	LCD_SPI_Send(data);
	TFT_LCD_CS(1);
}

void LCD_Send_Data_2B(uint16_t Data)
{
	uint8_t Data1[2];
	TFT_LCD_CS(0);
	LCD_SPI_Send(Data>>8);
	LCD_SPI_Send(Data&0xFF);
	TFT_LCD_CS(1);
}

void LCD_Send_Data(const uint8_t *data, uint8_t len)
{
	uint8_t i;
	TFT_LCD_CS(0);
//	SPI_8b();
	for(i=0;i<len;i++) 
	{
		LCD_SPI_Send(data[i]);
	}  
	TFT_LCD_CS(1);
}

void LCD_Display(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const uint8_t *pix_data)
{
	uint8_t End=1;
	uint32_t Send_Length,ALL_Length;
	dma_parameter_struct  dma_init_struct;
	uint8_t tx_data[4];
	x0+=X_Offset;
	x1+=X_Offset;	
	y0+=Y_Offset;
	y1+=Y_Offset;	
	ALL_Length=(x1-x0+1)*(y1-y0+1);
	if(ALL_Length==0)
		return;	
	tx_data[0] = x0>>8;
	tx_data[1] = x0&0xFF;
	tx_data[2] = x1>>8;
	tx_data[3] = x1&0xFF;
	LCD_Send_CMD_1B(0x2A);
	LCD_Send_Data(tx_data,4);
	tx_data[0] = y0>>8;
	tx_data[1] = y0&0xFF;
	tx_data[2] = y1>>8;
	tx_data[3] = y1&0xFF;
	LCD_Send_CMD_1B(0x2B);
	LCD_Send_Data(tx_data,4);
	LCD_Send_CMD_1B(0x2C);
	while(End)
	{
		if(ALL_Length>65534)
		{
			ALL_Length-=65534;
			Send_Length=65534;
		}
		else
		{
			End=0;
			Send_Length=ALL_Length;
		}
//开始传输	
		spi_disable(SPI0);
		TFT_LCD_CS(0);
		spi_i2s_data_frame_format_config(SPI0,SPI_FRAMESIZE_16BIT);
		dma_struct_para_init(&dma_init_struct);  
		rcu_periph_clock_enable(RCU_DMA);
		dma_deinit(DMA_CH2);
		dma_init_struct.periph_addr         = (uint32_t)(SPI0+0x0CU);
		dma_init_struct.memory_addr         = (uint32_t)pix_data;
		dma_init_struct.direction           = DMA_MEMORY_TO_PERIPHERAL;
		dma_init_struct.periph_width        = DMA_PERIPHERAL_WIDTH_16BIT;
		dma_init_struct.memory_width        = DMA_MEMORY_WIDTH_16BIT;
		dma_init_struct.priority            = DMA_PRIORITY_ULTRA_HIGH;
		dma_init_struct.number              = Send_Length;
		dma_init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
		dma_init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
		dma_init(DMA_CH2, &dma_init_struct);
		dma_circulation_disable(DMA_CH2);
		dma_memory_to_memory_disable(DMA_CH2);
		dma_channel_enable(DMA_CH2);
		spi_enable(SPI0);
		spi_dma_enable(SPI0, SPI_DMA_TRANSMIT);
		while(RESET == dma_flag_get(DMA_CH2,DMA_FLAG_FTF));
		spi_disable(SPI0);
		spi_i2s_data_frame_format_config(SPI0,SPI_FRAMESIZE_8BIT);
		spi_enable(SPI0);
	}
	TFT_LCD_CS(1);
}

void LCD_Display_Color(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint16_t Color)
{
	uint8_t End=1;
	uint32_t Send_Length,ALL_Length;
	dma_parameter_struct  dma_init_struct;
	uint8_t tx_data[4];
	uint8_t buff[2] = {0};
	buff[0] = (Color >> 8)&0xff;
	buff[1] = (Color)&0xff;
	x0+=X_Offset;
	x1+=X_Offset;	//-1
	y0+=Y_Offset;
	y1+=Y_Offset;	//-1
	tx_data[0] = x0>>8;
	tx_data[1] = x0&0xFF;
	tx_data[2] = x1>>8;
	tx_data[3] = x1&0xFF;
	LCD_Send_CMD_1B(0x2A);
	LCD_Send_Data(tx_data,4);
	tx_data[0] = y0>>8;
	tx_data[1] = y0&0xFF;
	tx_data[2] = y1>>8;
	tx_data[3] = y1&0xFF;
	LCD_Send_CMD_1B(0x2B);
	LCD_Send_Data(tx_data,4);
	LCD_Send_CMD_1B(0x2C);
	ALL_Length=((x1+1)-x0)*((y1+1)-y0);
	while(End)
	{
		if(ALL_Length>65534)
		{
			ALL_Length-=65534;
			Send_Length=65534;
		}
		else
		{
			End=0;
			Send_Length=ALL_Length;
		}
		if(!Send_Length)
			return;	
//开始传输	
		spi_disable(SPI0);
		TFT_LCD_CS(0);
		spi_i2s_data_frame_format_config(SPI0,SPI_FRAMESIZE_16BIT);
		dma_struct_para_init(&dma_init_struct);  
		rcu_periph_clock_enable(RCU_DMA);
		dma_deinit(DMA_CH2);
		dma_init_struct.periph_addr         = (uint32_t)(SPI0+0x0CU);
		dma_init_struct.memory_addr         = (uint32_t)&Color;
		dma_init_struct.direction           = DMA_MEMORY_TO_PERIPHERAL;
		dma_init_struct.periph_width        = DMA_PERIPHERAL_WIDTH_16BIT;
		dma_init_struct.memory_width        = DMA_MEMORY_WIDTH_16BIT;
		dma_init_struct.priority            = DMA_PRIORITY_ULTRA_HIGH;
		dma_init_struct.number              = Send_Length;
		dma_init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
		dma_init_struct.memory_inc          = DMA_MEMORY_INCREASE_DISABLE;
		dma_init(DMA_CH2, &dma_init_struct);
		dma_circulation_disable(DMA_CH2);
		dma_memory_to_memory_disable(DMA_CH2);
		dma_channel_enable(DMA_CH2);
		spi_enable(SPI0);
		spi_dma_enable(SPI0, SPI_DMA_TRANSMIT);
		while(RESET == dma_flag_get(DMA_CH2,DMA_FLAG_FTF));
		spi_disable(SPI0);
		spi_i2s_data_frame_format_config(SPI0,SPI_FRAMESIZE_8BIT);
		spi_enable(SPI0);
	}	
	TFT_LCD_CS(1);
}

uint8_t End_DMA_Interrupt;
uint32_t Send_Length_DMA_Interrupt,ALL_Length_DMA_Interrupt;
uint8_t *pix_data_DMA_Interrupt;

void LCD_Display_DMA_Interrupt(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint8_t *pix_data,uint8_t New)
{
//	dma_init_type DMA_Init;
//	if(New)
//	{
//		End_DMA_Interrupt=1;
//		uint8_t tx_data[4];
//		x0+=X_Offset;
//		x1+=X_Offset;	//-1
//		y0+=Y_Offset;
//		y1+=Y_Offset;	//-1
//		tx_data[0] = x0>>8;
//		tx_data[1] = x0&0xFF;
//		tx_data[2] = x1>>8;
//		tx_data[3] = x1&0xFF;
//		LCD_Send_CMD_1B(0x2A);
//		LCD_Send_Data(tx_data,4);
//		tx_data[0] = y0>>8;
//		tx_data[1] = y0&0xFF;
//		tx_data[2] = y1>>8;
//		tx_data[3] = y1&0xFF;
//		LCD_Send_CMD_1B(0x2B);
//		LCD_Send_Data(tx_data,4);
//		LCD_Send_CMD_1B(0x2C);
//		SPI_16b();
//		spi_enable(SPI1,TRUE);
//		ALL_Length_DMA_Interrupt=((x1+1)-x0)*((y1+1)-y0);
//		pix_data_DMA_Interrupt=pix_data;
//	}
//	else
//	{
//		dma_flag_clear(DMA1_FDT5_FLAG);																				//清除传输完成标志
//		dma_channel_enable(DMA1_CHANNEL5,FALSE);															//停用DMA通道
//		spi_i2s_dma_transmitter_enable(SPI2,FALSE);														//DMA传输结束
//		pix_data_DMA_Interrupt=pix_data_DMA_Interrupt+Send_Length_DMA_Interrupt;
//	}
//	while(End_DMA_Interrupt)
//	{
//		if(ALL_Length_DMA_Interrupt>65534)
//		{
//			ALL_Length_DMA_Interrupt-=65534;
//			Send_Length_DMA_Interrupt=65534;
//		}
//		else
//		{
//			End_DMA_Interrupt=0;
//			Send_Length_DMA_Interrupt=ALL_Length_DMA_Interrupt;
//		}
//		dmamux_enable(DMA1,TRUE);
//		dmamux_init(DMA1MUX_CHANNEL5,DMAMUX_DMAREQ_ID_SPI2_TX);
//		dma_default_para_init(&DMA_Init);																			//DMA默认配置
//		dma_reset(DMA1_CHANNEL5);																							//DMA通道恢复默认配置		
//		if(!Send_Length_DMA_Interrupt)
//			return;
////DMA1通道配置
//		DMA_Init.buffer_size						= Send_Length_DMA_Interrupt;				//缓冲区大小		= Residual_Length
//		DMA_Init.direction							= DMA_DIR_MEMORY_TO_PERIPHERAL;				//方向					= 存储器到外设
//		DMA_Init.memory_base_addr 			= (uint32_t)pix_data_DMA_Interrupt;		//内存基地址		= pbuffer
//		DMA_Init.memory_data_width 			= DMA_MEMORY_DATA_WIDTH_HALFWORD;			//内存数据宽度	= 半字(2B)
//		DMA_Init.memory_inc_enable 			= TRUE;																//内存地址递增	= 使能
//		DMA_Init.peripheral_base_addr 	= (uint32_t)(&SPI2->dt);							//外设基地址		= SPI->dt的地址
//		DMA_Init.peripheral_data_width 	= DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;	//外设数据宽度	= 半字(2B)
//		DMA_Init.peripheral_inc_enable 	= FALSE;															//外设地址递增	= 不使能
//		DMA_Init.priority 							= DMA_PRIORITY_VERY_HIGH;							//优先级				= 非常高
//		DMA_Init.loop_mode_enable 			= FALSE;															//环路模式使能	= 不使能
//		dma_init(DMA1_CHANNEL5,&DMA_Init);		
////开始传输	
//		SPI_LCD_CS(0);
//		spi_i2s_dma_transmitter_enable(SPI2,TRUE);														//DMA传输开始
//		dma_channel_enable(DMA1_CHANNEL5,TRUE);																//使能DMA通道
//		nvic_irq_enable(DMA1_Channel5_IRQn,0,0);	
//		dma_interrupt_enable(DMA1_CHANNEL5,DMA_FDT_INT,TRUE);		
//		return ;
//	}
//	while(spi_i2s_flag_get(SPI2,SPI_I2S_BF_FLAG)==SET);
//	SPI_LCD_CS(1);
//	SPI_8b();	
//	lv_display_flush_ready(DMA_disp_drv);				//通知图形库已准备好		
}

void LCD_Chip_Init(void)
{
	LCD_Send_CMD_1B(0x36);
	LCD_Send_Data_1B(0x00);
	LCD_Send_CMD_1B(0x3A); 
	LCD_Send_Data_1B(0x05);
	LCD_Send_CMD_1B(0xB2);
	LCD_Send_Data_1B(0x0C);
	LCD_Send_Data_1B(0x0C);
	LCD_Send_Data_1B(0x00);
	LCD_Send_Data_1B(0x33);
	LCD_Send_Data_1B(0x33); 
	LCD_Send_CMD_1B(0xB7); 
	LCD_Send_Data_1B(0x35);  
	LCD_Send_CMD_1B(0xBB);
	LCD_Send_Data_1B(0x19);
	LCD_Send_CMD_1B(0xC0);
	LCD_Send_Data_1B(0x2C);
	LCD_Send_CMD_1B(0xC2);
	LCD_Send_Data_1B(0x01);
	LCD_Send_CMD_1B(0xC3);
	LCD_Send_Data_1B(0x12);   
	LCD_Send_CMD_1B(0xC4);
	LCD_Send_Data_1B(0x20);  
	LCD_Send_CMD_1B(0xC6); 
	LCD_Send_Data_1B(0x0F);    
	LCD_Send_CMD_1B(0xD0); 
	LCD_Send_Data_1B(0xA4);
	LCD_Send_Data_1B(0xA1);
	LCD_Send_CMD_1B(0xE0);
	LCD_Send_Data_1B(0xD0);
	LCD_Send_Data_1B(0x04);
	LCD_Send_Data_1B(0x0D);
	LCD_Send_Data_1B(0x11);
	LCD_Send_Data_1B(0x13);
	LCD_Send_Data_1B(0x2B);
	LCD_Send_Data_1B(0x3F);
	LCD_Send_Data_1B(0x54);
	LCD_Send_Data_1B(0x4C);
	LCD_Send_Data_1B(0x18);
	LCD_Send_Data_1B(0x0D);
	LCD_Send_Data_1B(0x0B);
	LCD_Send_Data_1B(0x1F);
	LCD_Send_Data_1B(0x23);
	LCD_Send_CMD_1B(0xE1);
	LCD_Send_Data_1B(0xD0);
	LCD_Send_Data_1B(0x04);
	LCD_Send_Data_1B(0x0C);
	LCD_Send_Data_1B(0x11);
	LCD_Send_Data_1B(0x13);
	LCD_Send_Data_1B(0x2C);
	LCD_Send_Data_1B(0x3F);
	LCD_Send_Data_1B(0x44);
	LCD_Send_Data_1B(0x51);
	LCD_Send_Data_1B(0x2F);
	LCD_Send_Data_1B(0x1F);
	LCD_Send_Data_1B(0x1F);
	LCD_Send_Data_1B(0x20);
	LCD_Send_Data_1B(0x23);
	LCD_Send_CMD_1B(0x21); 
	LCD_Send_CMD_1B(0x11); 
	LCD_Send_CMD_1B(0x29);
}


/*********************************************************************************************************************************************************************************/

uint16_t Display_Buff[512];

void SH7789_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
	LCD_Send_CMD_1B(0x2A);														//列地址设置
	LCD_Send_Data_2B(x1+X_Offset);
	LCD_Send_Data_2B(x2+X_Offset);
	LCD_Send_CMD_1B(0x2B);														//行地址设置
	LCD_Send_Data_2B(y1+Y_Offset);
	LCD_Send_Data_2B(y2+Y_Offset);
	LCD_Send_CMD_1B(0x2C);	 													//储存
}

void LCD_Display_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
	SH7789_Address_Set(x,y,x,y);											//设置光标位置 
	LCD_Send_Data_2B(color);
} 

uint32_t mypow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;
	return result;
}

void LCD_Clear(uint16_t Color)
{
	uint16_t i,j;  	
	SH7789_Address_Set(0,0,240-1,240-1);
	for(i=0;i<240;i++)
	{
		for (j=0;j<240;j++)
		{
			LCD_Send_Data_2B(Color); 			 
		}
	}
}

void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint16_t fc,uint16_t bc,uint8_t sizey)
{
	uint8_t temp,sizex,t,m=0;
	uint16_t i,TypefaceNum,Location;												//字符所占字节
	uint16_t x0=x;
	sizex=sizey/2;
	TypefaceNum=(sizex/8+((sizex%8)?1:0))*sizey;
	num=num-' ';																			//得到偏移后的值
//	SH7789_Address_Set(x,y,x+sizex-1,y+sizey-1);			//设置光标位置 
	Location=0;
	for(i=0;i<TypefaceNum;i++)
	{ 
		if(sizey==12)temp=ascii_1206[num][i];						//调用6x12字体
		else if(sizey==16)temp=ascii_1608[num][i];			//调用8x16字体
		else if(sizey==24)temp=ascii_2412[num][i];			//调用12x24字体
		else if(sizey==32)temp=ascii_3216[num][i];			//调用16x32字体
		else return;
		for(t=0;t<8;t++)
		{
//			if(temp&(0x01<<t))
//				LCD_Send_Data_2B(fc);
//			else 
//				LCD_Send_Data_2B(bc);
			if(temp&(0x01<<t))
				Display_Buff[Location]=fc;
			else 
				Display_Buff[Location]=bc;
			Location++;			
			m++;
			if(m%sizex==0)
			{
				m=0;
				break;
			}
		}
	} 
	LCD_Display(x,y,x+sizex-1,y+sizey-1,(uint8_t*)Display_Buff);	
}

void LCD_ShowIntNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint16_t fc,uint16_t bc,uint8_t sizey)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;
	uint8_t sizex=sizey/2;
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+t*sizex,y,'0',fc,bc,sizey);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+t*sizex,y,temp+48,fc,bc,sizey);
	}
} 

void LCD_ShowString(uint16_t x,uint16_t y,const char *p,uint16_t fc,uint16_t bc,uint8_t sizey)
{         
	while(*p!='\0')
	{       
		LCD_ShowChar(x,y,*p,fc,bc,sizey);
		x+=sizey/2;
		p++;
	}  
}

void Rounded_Rectangle(uint16_t X0,uint16_t Y0,uint16_t X1,uint16_t Y1,uint16_t R,uint16_t Colour)
{
	int X,Y,D,X_Start,Y_Start,X_End,Y_End,i;
	if(X0<X1){X_Start=X0;X_End=X1;}else{X_Start=X1;X_End=X0;}
	if(Y0<Y1){Y_Start=Y0;Y_End=Y1;}else{Y_Start=Y1;Y_End=Y0;}
	X=0;
	Y=R;
	D=3-2*R;
	while(X<=Y)
	{
		LCD_Display_Color(X_Start +R -X,	Y_Start +R -Y,	X_End	-R	+X,	Y_Start	+R -Y,	Colour);
		LCD_Display_Color(X_Start +R -Y,	Y_Start +R -X,	X_End	-R	+Y,	Y_Start	+R -X,	Colour);		
		LCD_Display_Color(X_Start +R -X,	Y_End   -R +Y,	X_End	-R	+X,	Y_End		-R +Y,	Colour);	
		LCD_Display_Color(X_Start +R -Y,	Y_End   -R +X,	X_End	-R	+Y,	Y_End		-R +X,	Colour);	
		if(D<0)
			D+=4*X+6;
		else 
		{
			D+=4*(X-Y)+10;
			Y--;
		}
		X++;
	}
	for(i=Y_Start+R+1;i<=Y_End-R-1;i++)
	{
		LCD_Display_Color(X_Start,i,X_End,i,Colour);
	}
}

float Digit_Magnify[]={1,10,100,1000,10000,100000,1000000,10000000};
long long Display_Max_Value[]={0,9,99,999,9999,99999,999999,9999999,999999};

/******************************************************************************
x,y				= 显示的其实地址
num				= 显示的变量
len				= 显示长度
decimals	= 小数位数
fc				= 字的颜色
bc				= 背景的颜色
sizey			= 字号
Mode			= 显示模式(1=隐藏无效位  0=保留无效位)
SUM_width	= 显示的总宽度(用于自动居中)
******************************************************************************/
void LCD_Show_Float(uint16_t x,uint16_t y,float num,uint8_t len,uint8_t decimals,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t Mode,uint16_t SUM_width)
{    

	long long Integer_Value;																										//整数值
	uint8_t Site_Value=0;																														//刷新位置的值
	uint8_t Offset;																																	//偏置值
	uint8_t Display_Length;																													//显示长度	
	uint8_t Site=0;																																	//刷新位置
	uint8_t Font_width=sizey/2;																											//字体宽度	
	if(Mode==1)																																	//如果,模式=1计算有效显示长度
	{

	}
	else																																				//如果,模式≠1计算有效显示长度
		Display_Length=len;																												//有效显示长度=显示长度
	if(Display_Length>len)																											//如果,有效显示长度>显示长度,使用显示长度
		Display_Length=len;
	if((num<0)||(Mode==2))																											//如果,为负数显示或模式为1,,长度+1
		Display_Length++;	
	if((decimals)||(Mode==2))																										//如果,有小数显示长度+1
		Display_Length++;	
	Offset=(SUM_width-(Display_Length*Font_width))/2;														//计算居中显示的起始坐标
	LCD_Display_Color(x,y,x+Offset,y+sizey,bc);																	//檫除显示部分	
	LCD_Display_Color((x+SUM_width)-Offset,y,x+SUM_width,y+sizey,bc);						//檫除上次尾部部分
	x+=Offset;																																	//x坐标+居中偏置
	if(num<0)																																		//如果,为负数显示
	{																																			
		LCD_ShowChar(x,y,'-',fc,bc,sizey);																				//显示字符-
		x+=Font_width;																														//x坐标+字宽度
		Display_Length--;																													//剩余显示长度-1
		Integer_Value=-num*Digit_Magnify[decimals];																//整形数值=-数值*小数点放大倍率
	}
	else 
	{
		if(Mode==2)																																//如果,模式为2
		{																																		
			LCD_ShowChar(x,y,'+',fc,bc,sizey);																			//显示字符-
			x+=Font_width;																													//x坐标+字宽度
			Display_Length--;																												//剩余显示长度-1
		}		
		Integer_Value=round((num)*Digit_Magnify[decimals]*10)/10;									//整形数值=-数值*小数点放大倍率
	} 
	if((decimals)||(Mode==2))																										//如果,有小数显示长度-1
		Display_Length--;																													//剩余显示长度-1
	if(Integer_Value>Display_Max_Value[Display_Length])													//如果,整形数值>指定长度下的最大值
		Integer_Value=Display_Max_Value[Display_Length];													//整形数值=指定长度下的最大值
	for(;Site<Display_Length;Site++)																						//循环显示至显示位≮显示长度
	{
		Site_Value=(Integer_Value/mypow(10,Display_Length-Site-1))%10;						//位显示值=(整型值/10^(剩余位数-1))除余10
		if(Site==(Display_Length-decimals))																				//显示位=有效显示长度-小数位=小数点位置
		{																																		
			LCD_ShowChar(x+(Site*Font_width),y,'.',fc,bc,sizey);										//显示字符.
			Site++;																																	//显示位+1
			Display_Length++;																												//有效显示长度+1（循环长度未包括小数点）
		}
		LCD_ShowChar(x+(Site*Font_width),y,Site_Value+48,fc,bc,sizey);						//使用显示字符函数显示，位显示值
	}
	if((!decimals)&&(Mode==2))
	{																																			
		LCD_ShowChar(x+(Site*Font_width),y,'.',fc,bc,sizey);											//显示字符.	
	}
}


