#ifndef HARDWARE_SPI_H
#define HARDWARE_SPI_H

#include <main.h>

void Hardware_SPI0_Init(void);
void SPI0_Write(uint8_t data);

void Hardware_SPI1_Init(void);
uint8_t SPI1_Blocked_Transmit_Byte(uint8_t Data);
void SPI1_Blocked_Transmit(uint8_t *TX_Data,uint8_t *RX_Data,uint32_t Length,uint8_t Mode);
void SPI1_DMA_Send(uint8_t *TX_Data,uint32_t Length);
void SPI1_DMA_Receive(uint8_t *TX_Data,uint32_t Length);
void SPI1_DMA_Transmit(uint8_t *TX_Data,uint8_t *RX_Data,uint32_t Length,uint8_t Mode);

#endif
