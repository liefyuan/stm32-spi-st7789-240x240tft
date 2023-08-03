#ifndef __HAL_SPI1_DMA_H__
#define __HAL_SPI1_DMA_H__

#include "stdint.h"
#include "sys.h"
#include "st7789_hal_spi1.h"

#define SCREEN_SIZE      ST7789_LCD_WIDTH*ST7789_LCD_HEIGHT


#define DMA_SPI_BUF_SIZE (240*240)
extern uint16_t SpiDmaBuffer[DMA_SPI_BUF_SIZE];	//发送数据缓冲区


void DMA_Config(uint32_t TX_Buff,uint32_t SENDBUFF_SIZE);
void DMA_Write_buf(uint32_t SizeLen);
void dma_hal_spi1_clear(uint16_t color);

#endif // __HAL_SPI1_DMA_H__



