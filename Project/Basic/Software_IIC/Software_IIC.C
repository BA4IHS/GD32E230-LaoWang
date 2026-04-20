#include "Software_IIC.h" 

void Software_IIC_Init(void)
{		
	rcu_periph_clock_enable(RCU_GPIOB);
	gpio_mode_set(GPIOB,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_PIN_8|GPIO_PIN_9);
	gpio_output_options_set(GPIOB,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_8|GPIO_PIN_9);
	gpio_bit_reset(GPIOB,GPIO_PIN_8|GPIO_PIN_9);
	Software_IIC_SDA(1);
	Software_IIC_SCL(1);	
	Software_IIC_SCAN();	
}

void Software_SDA_Direction(uint8_t Direction)
{
	if(Direction)
	{
		gpio_mode_set(GPIOB,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_PIN_9);			
	}
	else
	{
		gpio_mode_set(GPIOB,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,GPIO_PIN_9);
	}	
}

void Software_IIC_Start(void)
{
	Software_IIC_Delay_H();
	Software_SDA_Direction(1);
	Software_IIC_SDA(1);  
	Software_IIC_SCL(1);
	Software_IIC_Delay_H();
 	Software_IIC_SDA(0);
	Software_IIC_Delay_H();
	Software_IIC_SCL(0);
	Software_IIC_Delay_L();
}	  

void Software_IIC_Stop(void)
{
	Software_SDA_Direction(1);
	Software_IIC_SDA(0);
	Software_IIC_SCL(0);
	Software_IIC_Delay_H();
	Software_IIC_SCL(1);
	Software_IIC_Delay_H();
 	Software_IIC_SDA(1);
}

uint8_t Software_IIC_Wait_Ack(void)
{
	Software_SDA_Direction(0);
//Software_IIC_SDA(1);
	Software_IIC_Delay_L();
	Software_IIC_SCL(1);
	Software_IIC_Delay_H();
	if(Software_IIC_SDA_Gat())
	{
		Software_IIC_Delay_L();
		Software_IIC_SCL(0);
		Software_IIC_Delay_L();
		return 1;
	}
	else
	{
		Software_IIC_Delay_L();
		Software_IIC_SCL(0);
		Software_IIC_Delay_L();
		return 0;
	}
} 

void Software_IIC_Ack(void)
{
	Software_IIC_SCL(0);
	Software_SDA_Direction(1);
	Software_IIC_SDA(0);
	Software_IIC_Delay_H();
	Software_IIC_SCL(1);
	Software_IIC_Delay_H();
	Software_IIC_SCL(0);
}

void Software_IIC_NAck(void)
{
	Software_IIC_SCL(0);
	Software_SDA_Direction(1);
	Software_IIC_SDA(1);
	Software_IIC_Delay_H();
	Software_IIC_SCL(1);
	Software_IIC_Delay_H();
	Software_IIC_SCL(0);
}					 				     		  

void Software_IIC_Send_Byte(uint8_t txd)
{                        
  uint8_t t;
	Software_SDA_Direction(1);
  Software_IIC_SCL(0);
	for(t=0;t<8;t++)
	{              
		if((txd&0x80)>>7)
		{
			Software_IIC_SDA(1);
		}
		else	
		{
			Software_IIC_SDA(0);	
		}
		txd<<=1;
		Software_IIC_Delay_L();
		Software_IIC_SCL(1);
		Software_IIC_Delay_H();
		Software_IIC_SCL(0);
		Software_IIC_Delay_H();
	}	 
} 	    

uint8_t Software_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	Software_SDA_Direction(0);	
  for(i=0;i<8;i++)
	{
    Software_IIC_SCL(0);
    Software_IIC_Delay_H();
		Software_IIC_SCL(1);
    receive<<=1;
		if(Software_IIC_SDA_Gat())
			receive++;
		Software_IIC_Delay_H();
    Software_IIC_SCL(0);
   }					 
	if(!ack)
		Software_IIC_NAck();
   else
		Software_IIC_Ack();
   return receive;
}

void Software_IIC_Send(uint16_t Addr,uint8_t *Buffer,uint16_t Length,uint8_t Stop)
{
	uint8_t i;
	Software_IIC_Start();
	Software_IIC_Send_Byte(Addr<<1);
	Software_IIC_Wait_Ack();
	for(i=0;Length>0;Length--)
	{
		Software_IIC_Send_Byte(Buffer[i]);
		Software_IIC_Wait_Ack();
		i++;
	}
	if(Stop)
		Software_IIC_Stop();		
}

void Software_IIC_Read(uint16_t Addr,uint8_t *Buffer,uint16_t Length)
{
	uint8_t i;
	Software_IIC_Start();
	Software_IIC_Send_Byte((Addr<<1)+1);
	Software_IIC_Wait_Ack();
	for(i=0;Length>0;Length--)
	{
		if(Length==1)
			Buffer[i]=Software_IIC_Read_Byte(0);
		else
			Buffer[i]=Software_IIC_Read_Byte(1);
		i++;
	}
	Software_IIC_Stop();		
}

uint8_t Software_IIC_Register_Write(uint8_t Addr,uint8_t Register,uint8_t *Buffer,uint16_t Length)
{
	uint8_t i,OnLine;
	Software_IIC_Start();
	Software_IIC_Send_Byte(Addr<<1);
	OnLine=!Software_IIC_Wait_Ack();
	Software_IIC_Send_Byte(Register);
	Software_IIC_Wait_Ack();	
	for(i=0;Length>0;Length--)
	{
		Software_IIC_Send_Byte(Buffer[i]);
		Software_IIC_Wait_Ack();
		i++;
	}
	Software_IIC_Stop();	
	return OnLine;	
}

uint8_t Software_IIC_Register_Read(uint8_t Addr,uint8_t Register,uint8_t *Buffer,uint16_t Length)
{
	uint8_t i,OnLine;
	Software_IIC_Start();
	Software_IIC_Send_Byte(Addr<<1);
	OnLine=!Software_IIC_Wait_Ack();
	Software_IIC_Send_Byte(Register);
	Software_IIC_Wait_Ack();	
	Software_IIC_Start();
	Software_IIC_Send_Byte((Addr<<1)+1);
	Software_IIC_Wait_Ack();
	for(i=0;Length>0;Length--)
	{
		if(Length==1)
			Buffer[i]=Software_IIC_Read_Byte(0);
		else
			Buffer[i]=Software_IIC_Read_Byte(1);
		i++;
	}
	Software_IIC_Stop();	
	return OnLine;
}

#define IIC_MAX_DEVICES  10

uint8_t IIC_ADDT[IIC_MAX_DEVICES];
uint8_t IIC_ADDN = 0;

/* I2C总线恢复：当SDA被从机拉低时，发送时钟脉冲释放总线 */
static void Software_IIC_Bus_Recovery(void)
{
	uint8_t i;
	Software_SDA_Direction(1);
	Software_IIC_SDA(1);
	Software_IIC_SCL(1);

	/* 如果SDA被拉低，发送最多9个时钟脉冲释放 */
	for(i = 0; i < 9; i++)
	{
		if(Software_IIC_SDA_Gat())
			break;
		Software_IIC_SCL(0);
		Software_IIC_Delay_H();
		Software_IIC_SCL(1);
		Software_IIC_Delay_H();
	}

	/* 发送STOP条件 */
	Software_IIC_SDA(0);
	Software_IIC_Delay_H();
	Software_IIC_SCL(1);
	Software_IIC_Delay_H();
	Software_IIC_SDA(1);
}

void Software_IIC_SCAN(void)
{
	uint8_t addr;
	uint8_t retry;

	Software_IIC_Bus_Recovery();
	IIC_ADDN = 0;

	for(addr = 0x08; addr <= 0x77; addr++)
	{
		/* 跳过已知的广播地址 */
		if(addr == 0x00 || addr == 0x04 || addr == 0x06 || addr == 0x78)
			continue;

		retry = 0;
		while(retry < 3)
		{
			Software_IIC_Start();
			Software_IIC_Send_Byte(addr << 1);
			if(!Software_IIC_Wait_Ack())
			{
				Software_IIC_Stop();
				if(IIC_ADDN < IIC_MAX_DEVICES)
				{
					IIC_ADDT[IIC_ADDN] = addr;
					IIC_ADDN++;
				}
				break;
			}
			Software_IIC_Stop();
			retry++;
		}
	}
}

