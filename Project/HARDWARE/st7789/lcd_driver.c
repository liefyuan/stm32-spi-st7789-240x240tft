#include "lcd_driver.h"

#include "stm32f4xx.h"
#include "stm32f4xx_spi.h"
#include "delay.h"

#include "st7789_gpio_spi.h"
#include "st7789_hal_spi.h"

#if USE_DMA_SPI
#include "hal_spi_dma.h"
#endif // USE_DMA_SPI

#define USE_HAL_SPI  1


void LCD_Init(void)
{
#if USE_HAL_SPI
	hal_spi_gpio_init();
		
	hal_spi_hard_reset();
	
	hal_spi_lcd_init();
#else
	gpio_spi_init();
	
	gpio_spi_hard_reset();
	
	gpio_spi_lcd_init();
#endif
}

//向液晶屏写一个8位指令
void Lcd_WriteIndex(uint8_t Index)
{
#if USE_HAL_SPI
	hal_spi_write_cmd(Index);
#else
	gpio_spi_write_cmd(Index);
#endif
}

//向液晶屏写一个8位数据
void Lcd_WriteData(uint8_t Data)
{
#if USE_HAL_SPI
	hal_spi_write_8bit_data(Data);
#else
	gpio_spi_write_8bit_data(Data);
#endif
}
//向液晶屏写一个16位数据
void LCD_WriteData_16Bit(uint16_t Data)
{
#if USE_HAL_SPI
	hal_spi_write_16bit_data(Data);
#else
	gpio_spi_write_16bit_data(Data);
#endif
}

void Lcd_Reset(void)
{
#if USE_HAL_SPI
	hal_spi_hard_reset();
#else
	gpio_spi_hard_reset();
#endif
}

/***********************************************************************/

void Lcd_Full(uint16_t color)
{
#if USE_HAL_SPI
	#if USE_DMA_SPI
		dma_hal_spi_clear(color);
	#else
		hal_spi_clear(color);
	#endif
#else
	gpio_spi_clear(color);
#endif
}

void Lcd_Test(void)
{

}


