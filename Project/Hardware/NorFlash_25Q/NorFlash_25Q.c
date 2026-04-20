#include "NorFlash_25Q.h"

uint8_t NorFlash_Status;
uint16_t NorFlash_ID=0x00;
uint32_t NorFlash_JID=0x00;
uint8_t NorFlash_UID[16];

/**************IO≥ı ºªØ**************/
void NorFlash_25Q_Init(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);
	gpio_mode_set(GPIOB,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12);
	gpio_output_options_set(GPIOB,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12);
	gpio_bit_set(GPIOB,GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12);
	NORFLASH_WP(1);
	NORFLASH_HOLD(1);
	NORFLASH_CS(1);
	NorFlash_ID=NorFlash_25Q_Read_ID();
	NorFlash_JID=NorFlash_25Q_Read_JEDEC_ID();
	NorFlash_25Q_Unique_ID(NorFlash_UID);	
}

/**************∂¡…Ë±∏ID**************/
uint16_t NorFlash_25Q_Read_ID(void)
{
	uint16_t ID;
	NORFLASH_CS(0);
	SPI1_Blocked_Transmit_Byte(SPIFlash_Read_Device_ID);
	SPI1_Blocked_Transmit_Byte(0x00);
	SPI1_Blocked_Transmit_Byte(0x00);	
	SPI1_Blocked_Transmit_Byte(0x00);	
	ID  = SPI1_Blocked_Transmit_Byte(0xFF)<< 8;
	ID |= SPI1_Blocked_Transmit_Byte(0xFF);
	NORFLASH_CS(1);
	return ID;	
}

/**************∂¡…Ë±∏ID**************/
uint32_t NorFlash_25Q_Read_JEDEC_ID(void)
{
	uint32_t ID;
	NORFLASH_CS(0);
	SPI1_Blocked_Transmit_Byte(SPIFlash_Read_JEDEC_ID);
	ID  = SPI1_Blocked_Transmit_Byte(0xFF)<< 16;
	ID |= SPI1_Blocked_Transmit_Byte(0xFF)<< 8;	
	ID |= SPI1_Blocked_Transmit_Byte(0xFF);	
	NORFLASH_CS(1);
	return ID;	
}

/**************∂¡Œ®“ªID**************/
void NorFlash_25Q_Unique_ID(uint8_t *Buffer)
{
	uint8_t Temporary[1]={0x00};
	NORFLASH_CS(0);
	SPI1_Blocked_Transmit_Byte(0x4B);
	SPI1_Blocked_Transmit_Byte(0x00);
	SPI1_Blocked_Transmit_Byte(0x00);
	SPI1_Blocked_Transmit_Byte(0x00);
	SPI1_DMA_Transmit(Temporary,Buffer,16,2);
	NORFLASH_CS(1);
}

/***************∂¡◊¥Ã¨***************/
uint16_t NorFlash_25Q_Read_Status(void)
{
	uint16_t Status;
	NORFLASH_CS(0);
	SPI1_Blocked_Transmit_Byte(SPIFlash_Read_Status1);
	Status  = SPI1_Blocked_Transmit_Byte(0x00);
	NORFLASH_CS(1);	
	delay_us(5);
	NORFLASH_CS(0);
	SPI1_Blocked_Transmit_Byte(SPIFlash_Read_Status2);
	Status |= SPI1_Blocked_Transmit_Byte(0x00)<< 8;
	NORFLASH_CS(1);	
	return Status;
}

/***************…Ë÷√–¥***************/
void NorFlash_25Q_Set_Write(void)
{
	NORFLASH_CS(0);
	SPI1_Blocked_Transmit_Byte(SPIFlash_Write_Enable);
	NORFLASH_CS(1);		
}

/**************µ»¥˝æÕ–˜**************/
void NorFlash_25Q_Wait_Busy(void)
{
	NORFLASH_CS(0);
	SPI1_Blocked_Transmit_Byte(SPIFlash_Read_Status1);
	while(SPI1_Blocked_Transmit_Byte(0x00) & SPIFlash_Status_BUSY);
	NORFLASH_CS(1);	
}

/***************–¥◊¥Ã¨***************/
void NorFlash_25Q_Write_Status(uint8_t Status)
{
	NORFLASH_CS(0);
	SPI1_Blocked_Transmit_Byte(SPIFlash_Status_Write_Enable);
	NORFLASH_CS(1);	
	delay_us(5);
	NORFLASH_CS(0);
	SPI1_Blocked_Transmit_Byte(SPIFlash_Write_Status1);
	SPI1_Blocked_Transmit_Byte(Status&0xFF);
	SPI1_Blocked_Transmit_Byte((Status>>8)&0xFF);
	NORFLASH_CS(1);	
}

/***************ºÏ≤ÈQE***************/
void NorFlash_25Q_Check_QE(void)
{
	uint16_t res;
	NorFlash_25Q_Read_ID();
	res = NorFlash_25Q_Read_ID();
	if ((res & SPIFlash_Status_QE) != SPIFlash_Status_QE)
	{
		res |= SPIFlash_Status_QE;
		NorFlash_25Q_Write_Status(res);
		NorFlash_25Q_Wait_Busy();
	}	
}

/***************∆¨Èﬂ≥˝***************/
void NorFlash_25Q_Chip_Erase(void)
{
	NORFLASH_CS(0);
	SPI1_Blocked_Transmit_Byte(SPIFlash_Chip_Erase);
	NORFLASH_CS(1);	
	delay_us(5);
	NorFlash_25Q_Wait_Busy();
}

/***************“≥Èﬂ≥˝***************/
void NorFlash_25Q_4K_Section_Erase(uint32_t addr)
{
	NORFLASH_CS(0);
	SPI1_Blocked_Transmit_Byte(0x20);
	SPI1_Blocked_Transmit_Byte((addr>>16)&0xFF);
	SPI1_Blocked_Transmit_Byte((addr>>8)&0xFF);
	SPI1_Blocked_Transmit_Byte(addr&0xFF);
	NORFLASH_CS(1);
}

/*************32KBøÈÈﬂ≥˝*************/
void NorFlash_25Q_32K_Block_Erase(uint32_t addr)
{
	NORFLASH_CS(0);
	SPI1_Blocked_Transmit_Byte(0x52);
	SPI1_Blocked_Transmit_Byte((addr>>16)&0xFF);
	SPI1_Blocked_Transmit_Byte((addr>>8)&0xFF);
	SPI1_Blocked_Transmit_Byte(addr&0xFF);
	NORFLASH_CS(1);
}

/*************64KBøÈÈﬂ≥˝*************/
void NorFlash_25Q_64K_Block_Erase(uint32_t addr)
{
	NORFLASH_CS(0);
	SPI1_Blocked_Transmit_Byte(0xD8);
	SPI1_Blocked_Transmit_Byte((addr>>16)&0xFF);
	SPI1_Blocked_Transmit_Byte((addr>>8)&0xFF);
	SPI1_Blocked_Transmit_Byte(addr&0xFF);
	NORFLASH_CS(1);
}

/**************∂¡»° ˝æ›**************/
void NorFlash_25Q_Read_Buffer(uint32_t addr,uint8_t *Buffer,uint32_t Length)
{
	uint8_t Temporary[1]={0x00};
	NORFLASH_CS(0);
	SPI1_Blocked_Transmit_Byte(0x03);
	SPI1_Blocked_Transmit_Byte((addr>>16)&0xFF);
	SPI1_Blocked_Transmit_Byte((addr>>8)&0xFF);
	SPI1_Blocked_Transmit_Byte(addr&0xFF);
	SPI1_DMA_Transmit(Temporary,Buffer,Length,2);
	NORFLASH_CS(1);
}

/**************–¥»Î ˝æ›**************/
void NorFlash_25Q_Page_Write_Buffer(uint32_t addr,uint8_t *Buffer,uint32_t Length)
{
	uint8_t Temporary[1]={0x00};
	NORFLASH_CS(0);
	SPI1_Blocked_Transmit_Byte(0x02);
	SPI1_Blocked_Transmit_Byte((addr>>16)&0xFF);
	SPI1_Blocked_Transmit_Byte((addr>>8)&0xFF);
	SPI1_Blocked_Transmit_Byte(addr&0xFF);
	SPI1_DMA_Transmit(Buffer,Temporary,Length,1);
	NORFLASH_CS(1);
}


