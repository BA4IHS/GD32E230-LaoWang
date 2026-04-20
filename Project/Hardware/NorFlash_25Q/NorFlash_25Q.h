#ifndef _NORFLASH_25Q_H
#define _NORFLASH_25Q_H

#include "main.h"

#define NORFLASH_CS(x)			gpio_bit_write(GPIOB,GPIO_PIN_12,(bit_status)x)
#define NORFLASH_WP(x)			gpio_bit_write(GPIOB,GPIO_PIN_10,(bit_status)x)	
#define NORFLASH_HOLD(x)		gpio_bit_write(GPIOB,GPIO_PIN_11,(bit_status)x)	


#define SPIFlash_Read_Device_ID					(0x90)
#define SPIFlash_Read_Device_ID_Dual		(0x92)
#define SPIFlash_Read_Device_ID_Quad		(0x94)
#define SPIFlash_Read_JEDEC_ID					(0x9F)
#define SPIFlash_Write_Enable						(0x06)
#define SPIFlash_Status_Write_Enable		(0x50)
#define SPIFlash_Write_Disable					(0x04)
#define SPIFlash_Read_Status1						(0x05)
#define SPIFlash_Read_Status2						(0x35)
#define SPIFlash_Read_Status3						(0x15)
#define SPIFlash_Write_Status1					(0x01)
#define SPIFlash_Write_Status2					(0x31)
#define SPIFlash_Write_Status3					(0x11)
#define SPIFlash_Chip_Erase							(0xC7)
#define SPIFlash_Read										(0x03)
#define SPIFlash_Fast_Read							(0x0B)
#define SPIFlash_Fast_Read_Dual_Output	(0x3B)
#define SPIFlash_Fast_Read_Dual_IO			(0xBB)
#define SPIFlash_Fast_Read_Quad_Output	(0x6B)
#define SPIFlash_Fast_Read_Quad_IO			(0xEB)
#define SPIFlash_Status_BUSY_Pos				(0U)
#define SPIFlash_Status_BUSY						(0x01U<<SPIFlash_Status_BUSY_Pos)
#define SPIFlash_Status_WEL_Pos					(1U)
#define SPIFlash_Status_WEL							(0x01U<<SPIFlash_Status_WEL_Pos)
#define SPIFlash_Status_QE_Pos					(9U)
#define SPIFlash_Status_QE							(0x01U<<SPIFlash_Status_QE_Pos)


void NorFlash_25Q_Init(void);																													//IO³õÊ¼»¯	
uint16_t  NorFlash_25Q_Read_ID(void);																									//¶ÁÉè±¸ID
uint32_t NorFlash_25Q_Read_JEDEC_ID(void);																						//¶ÁÉè±¸ID
void NorFlash_25Q_Unique_ID(uint8_t *Buffer);																					//¶ÁÎ¨Ò»ID
uint16_t  NorFlash_25Q_Read_Status(void);																							//¶Á×´Ì¬
void NorFlash_25Q_Set_Write(void);																										//ÉèÖÃÐ´
void NorFlash_25Q_Wait_Busy(void);																										//µÈ´ý¾ÍÐ÷
void NorFlash_25Q_Write_Status(uint8_t Status);																				//Ð´×´Ì¬
void NorFlash_25Q_Check_QE(void);																											//¼ì²éQE
void NorFlash_25Q_Chip_Erase(void);																										//Æ¬éß³ý
void NorFlash_25Q_4K_Section_Erase(uint32_t addr);																		//ÉÈÇøéß³ý
void NorFlash_25Q_32K_Block_Erase(uint32_t addr);																			//32KB¿ééß³ý
void NorFlash_25Q_64K_Block_Erase(uint32_t addr);																			//64KB¿ééß³ý
void NorFlash_25Q_Read_Buffer(uint32_t addr,uint8_t *Buffer,uint32_t Length);					//¶ÁÈ¡Êý¾Ý
void NorFlash_25Q_Page_Write_Buffer(uint32_t addr,uint8_t *Buffer,uint32_t Length);		//Ð´ÈëÊý¾Ý



#endif
