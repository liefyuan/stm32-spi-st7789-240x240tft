#include "stm32f4xx_dma.h"
#include "hal_spi_dma.h"
#include "delay.h"

#if USE_DMA_SPI

static uint8_t sendFlg = 0;

uint16_t Spi2DmaBuffer[DMA_SPI_BUF_SIZE];	//发送数据缓冲区
	
void DMA_Config(uint32_t TX_Buff,uint32_t SENDBUFF_SIZE)
{
    // 中断结构体
    NVIC_InitTypeDef NVIC_InitStructure;		
    // DMA结构体
    DMA_InitTypeDef DMA_InitStructure;  		
    /* 使能DMA时钟 */  		
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);	
    /* 复位初始化DMA数据流 */  
    DMA_DeInit(DMA1_Stream4);								
    /* 确保DMA数据流复位完成 */  
    while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE);	

    /* 配置 DMA Stream */
    /* 通道0，数据流4 */	  
    DMA_InitStructure.DMA_Channel = DMA_Channel_0;
    /* 外设地址 */  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI2->DR;	
    /* 内存地址(要传输的变量的指针) ,DMA存储器0地址*/  	
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)TX_Buff;	
    /* 方向：存储器到外设 */			
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    /* 数据传输量 ,可设置为0， 实际发送时会重新设置*/	    
    DMA_InitStructure.DMA_BufferSize = (uint32_t)SENDBUFF_SIZE;		
    /* 外设非增量模式 */		
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    /* 存储器增量模式 */  	
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    /* 外设数据长度:16位 */	 
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    /* 内存数据长度:16位 */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    /* DMA模式：正常模式 */  		
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    /* 优先级：高 */	 		
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    /* 禁用FIFO */
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;        		
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;   
    /* 外设突发单次传输 */  
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;    		
    /* 存储器突发单次传输 */  
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; 

    /* 初始化DMA Stream */		
    DMA_Init(DMA1_Stream4, &DMA_InitStructure);
    /* 开启传输完成中断  */		
    DMA_ITConfig(DMA1_Stream4,DMA_IT_TC,ENABLE);

    // 中断初始化 
    /* DMA发送中断源 */  
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn;	
    /* 抢断优先级 */  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    /* 响应优先级 */  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;				
    /* 使能外部中断通道 */ 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						 
    /* 配置NVIC */		
    NVIC_Init(&NVIC_InitStructure);
}

void DMA1_Stream4_IRQHandler(void)
{
    // DMA 发送完成
    if(DMA_GetITStatus(DMA1_Stream4, DMA_IT_TCIF4))	
    {
        // 清除DMA发送完成标志
        DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF4);	
        // 片选拉高，数据发送完毕	
        HAL_SPI_CS_SET;	
		sendFlg = 1;
    }
}

void DMA_Write_buf(uint32_t SizeLen)
{   
	// 关闭发送 DMA		
	DMA_Cmd(DMA1_Stream4, DISABLE);	
	
	//DMA_Config((uint32_t)Spi2DmaBuffer, DMA_SPI_BUF_SIZE);
	
	// 设置发送的数据量    
	DMA_SetCurrDataCounter(DMA1_Stream4, DMA_SPI_BUF_SIZE);
	// 清空数据
	SPI2->DR;		
	// 擦除DMA标志位	
	DMA_ClearFlag(DMA1_Stream4, DMA_IT_TCIF4);
	// 片选拉低,接收数据
	HAL_SPI_CS_CLR;
	HAL_SPI_DC_SET;
	// 开启发送 DMA
	DMA_Cmd(DMA1_Stream4, ENABLE);
	
	while(!sendFlg); 
	sendFlg = 0;
}

void dma_hal_spi_clear(uint16_t color)
{
    int i = 0;
    for(i = 0 ; i < DMA_SPI_BUF_SIZE; i++)
    {
        Spi2DmaBuffer[i] = color;
    }
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);//PA1  设置低，灯亮
	
	hal_spi_setregion(0, 0, LCD_WIDTH, LCD_HEIGHT);
	hal_spi_txbits(16);

	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
	
	DMA_Write_buf(DMA_SPI_BUF_SIZE);
	
	hal_spi_txbits(8);
	GPIO_SetBits(GPIOA,GPIO_Pin_1);//PA1  设置高，灯灭
}

#endif // USE_DMA_SPI
/*********************************************END OF FILE********************************************/
