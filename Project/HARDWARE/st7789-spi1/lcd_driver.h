#ifndef __LCD_DRIVER_H__
#define __LCD_DRIVER_H__

#include "stdint.h"
#include "stm32f4xx.h"
#include "system_stm32f4xx.h"

#define RED  	0xf800
#define GREEN	0x07e0
#define BLUE 	0x001f
#define WHITE	0xffff
#define BLACK	0x0000
#define YELLOW  0xFFE0
#define GRAY0   0xEF7D   	    //»ÒÉ«0 3165 00110 001011 00101
#define GRAY1   0x8410      	//»ÒÉ«1      00000 000000 00000
#define GRAY2   0x4208      	//»ÒÉ«2  1111111111011111

void LCD_Init(void);
void Lcd_Full(uint16_t color);
void Lcd_Dma_Full(uint16_t color);

#endif // __LCD_DRIVER_H__




