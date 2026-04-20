#ifndef _Software_IIC_H 
#define _Software_IIC_H

#include "main.h"

#define Software_IIC_Delay_H()	delay_us(15);
#define Software_IIC_Delay_L()	delay_us(5);
#define Software_IIC_SCL(x)			gpio_bit_write(GPIOB,GPIO_PIN_8,(bit_status)x)	
#define Software_IIC_SDA(x)			gpio_bit_write(GPIOB,GPIO_PIN_9,(bit_status)x)	
#define Software_IIC_SDA_Gat()	gpio_input_bit_get(GPIOB,GPIO_PIN_9)

void Software_IIC_Init(void);																																				//IIC初始化
void Software_IIC_Start(void);																																			//起始信号
void Software_IIC_Stop(void);																																				//终止信号
uint8_t Software_IIC_Wait_Ack(void);																																//读ACK应答（用于写数据）
void Software_IIC_Ack(void);																																				//发ACK应答（用于读数据）
void Software_IIC_NAck(void);																																				//发NACK应答（用于读数据）
void Software_IIC_Send_Byte(uint8_t txd);																														//发送1字节
uint8_t Software_IIC_Read_Byte(unsigned char ack);																									//读取1字节并应答
void Software_IIC_Send(uint16_t Addr,uint8_t *Buffer,uint16_t Length,uint8_t Stop);									//发送指定长度（IIC地址,数据指针,长度,是否终止）
void Software_IIC_Read(uint16_t Addr,uint8_t *Buffer,uint16_t Length);															//读取指定长度（IIC地址,数据指针,长度）
uint8_t Software_IIC_Register_Write(uint8_t Addr,uint8_t Register,uint8_t *Buffer,uint16_t Length);	//向指定地址发送指定长度
uint8_t Software_IIC_Register_Read(uint8_t Addr,uint8_t Register,uint8_t *Buffer,uint16_t Length);	//从指定地址读取指定长度
void Software_IIC_SCAN(void);

extern uint8_t IIC_ADDN;
extern uint8_t IIC_ADDT[10];

/************************** 底层I2C封装 **************************/
#define I2C1_WriteBytes(addr,reg,buf,len)    Software_IIC_Register_Write(addr,reg,buf,len)
#define I2C1_ReadBytes(addr,reg,buf,len)     Software_IIC_Register_Read(addr,reg,buf,len)


#endif 




