#include "Hardware_SPI.h"

void Hardware_SPI0_Init(void)
{
	rcu_periph_clock_enable(RCU_SPI0);
	rcu_periph_clock_enable(RCU_GPIOB);
	gpio_af_set(GPIOB,GPIO_AF_0,GPIO_PIN_3|GPIO_PIN_5);
	gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3|GPIO_PIN_5);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_3|GPIO_PIN_5);
	spi_parameter_struct spi_init_struct;
	spi_i2s_deinit(SPI0);
	spi_struct_para_init(&spi_init_struct);
	spi_init_struct.trans_mode           = SPI_BIDIRECTIONAL_TRANSMIT;
	spi_init_struct.device_mode          = SPI_MASTER;
	spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
	spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
	spi_init_struct.nss                  = SPI_NSS_SOFT;
	spi_init_struct.prescale             = SPI_PSC_2;
	spi_init_struct.endian               = SPI_ENDIAN_MSB;
	spi_init(SPI0, &spi_init_struct);
	spi_enable(SPI0);
}

void SPI0_Write(uint8_t data)
{
	while(spi_i2s_flag_get(SPI0,SPI_FLAG_TBE)==RESET);
	spi_i2s_data_transmit(SPI0,data); 
}

/*********************************************** SPI1 ***********************************************/

void Hardware_SPI1_Init(void)
{// SPI1_SCK=PB13  SPI1_MISO=PB14  SPI1_MISO=PB15
	rcu_periph_clock_enable(RCU_SPI1);
	rcu_periph_clock_enable(RCU_GPIOB);
	gpio_af_set(GPIOB,GPIO_AF_0,GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);		
	gpio_mode_set(GPIOB, GPIO_MODE_AF,GPIO_PUPD_PULLDOWN,GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
	gpio_output_options_set(GPIOB,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
	spi_parameter_struct spi_init_struct;
	spi_i2s_deinit(SPI1);
	spi_struct_para_init(&spi_init_struct);
	spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
	spi_init_struct.device_mode          = SPI_MASTER;
	spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
	spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
	spi_init_struct.nss                  = SPI_NSS_SOFT;
	spi_init_struct.prescale             = SPI_PSC_16;
	spi_init_struct.endian               = SPI_ENDIAN_MSB;
	spi_init(SPI1, &spi_init_struct);
	spi_i2s_data_frame_format_config(SPI1,SPI_FRAMESIZE_8BIT);
	spi_enable(SPI1);
	spi_fifo_access_size_config(SPI1, SPI_BYTE_ACCESS);
}

uint8_t SPI1_Blocked_Transmit_Byte(uint8_t Data)
{
	while(spi_i2s_flag_get(SPI1,SPI_FLAG_TBE)==RESET);
	spi_i2s_data_transmit(SPI1,Data); 
	while(RESET == spi_i2s_flag_get(SPI1,SPI_FLAG_RBNE));
	return spi_i2s_data_receive(SPI1);
}

void SPI1_Blocked_Transmit(uint8_t *TX_Data,uint8_t *RX_Data,uint32_t Length,uint8_t Mode)
{
	uint32_t i;
	for(i=0;Length>0;Length--)
	{
		switch(Mode)
		{
			case 0 : RX_Data[i]=SPI1_Blocked_Transmit_Byte(TX_Data[i]);break;		//收发
			case 1 : SPI1_Blocked_Transmit_Byte(TX_Data[i]);break;							//只发送
			case 2 : RX_Data[i]=SPI1_Blocked_Transmit_Byte(TX_Data[0]);break;		//只接收
		}
		i++;
	}	
}

void SPI1_DMA_Transmit(uint8_t *TX_Data,uint8_t *RX_Data,uint32_t Length,uint8_t Mode)
{
	dma_parameter_struct  dma_init_struct;
	dma_struct_para_init(&dma_init_struct);  
	rcu_periph_clock_enable(RCU_DMA);
	dma_deinit(DMA_CH4);
	dma_init_struct.periph_addr         = (uint32_t)(SPI1+0x0CU);
	dma_init_struct.memory_addr         = (uint32_t)TX_Data;
	dma_init_struct.direction           = DMA_MEMORY_TO_PERIPHERAL;
	dma_init_struct.periph_width        = DMA_PERIPHERAL_WIDTH_8BIT;
	dma_init_struct.memory_width        = DMA_MEMORY_WIDTH_8BIT;
	dma_init_struct.priority            = DMA_PRIORITY_ULTRA_HIGH;
	dma_init_struct.number              = Length;
	dma_init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
	if(Mode==2)
		dma_init_struct.memory_inc        = DMA_MEMORY_INCREASE_DISABLE;
	else
		dma_init_struct.memory_inc        = DMA_MEMORY_INCREASE_ENABLE;
	dma_init(DMA_CH4,&dma_init_struct);
	dma_circulation_disable(DMA_CH4);
	dma_memory_to_memory_disable(DMA_CH4);
	dma_channel_enable(DMA_CH4);
	dma_deinit(DMA_CH3);
	dma_init_struct.periph_addr         = (uint32_t)(SPI1+0x0CU);
	dma_init_struct.memory_addr         = (uint32_t)RX_Data;
	dma_init_struct.direction           = DMA_PERIPHERAL_TO_MEMORY;
	dma_init_struct.periph_width        = DMA_PERIPHERAL_WIDTH_8BIT;
	dma_init_struct.memory_width        = DMA_MEMORY_WIDTH_8BIT;
	dma_init_struct.priority            = DMA_PRIORITY_ULTRA_HIGH;
	dma_init_struct.number              = Length;
	dma_init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
	if(Mode==1)
		dma_init_struct.memory_inc        = DMA_MEMORY_INCREASE_DISABLE;
	else
		dma_init_struct.memory_inc        = DMA_MEMORY_INCREASE_ENABLE;
	dma_init(DMA_CH3,&dma_init_struct);
	dma_circulation_disable(DMA_CH3);
	dma_memory_to_memory_disable(DMA_CH3);
	dma_channel_enable(DMA_CH3);
	spi_dma_enable(SPI1,SPI_DMA_RECEIVE);
	spi_dma_enable(SPI1,SPI_DMA_TRANSMIT);
	while(RESET == dma_flag_get(DMA_CH3,DMA_FLAG_FTF));
	while(RESET == dma_flag_get(DMA_CH4,DMA_FLAG_FTF));
}


