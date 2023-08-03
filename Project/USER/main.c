#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd_driver.h"

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
    delay_init(168);      //初始化延时函数
    uart_init(115200);		//初始化串口波特率为115200

    LED_Init();					  //初始化LED
	
	printf("system init!\r\n");
	
	LCD_Init();
	
	Lcd_Full(RED);
	delay_ms(1000);
	
    while(1)
    {
		Lcd_Full(GREEN);
		//LED_ON;
		//delay_ms(1000);
		
		Lcd_Full(WHITE);
		//LED_OFF;
		//delay_ms(1000);
		
		Lcd_Full(BLUE);
		//LED_ON;
		//delay_ms(1000);
		
		Lcd_Full(RED);
		//LED_OFF;
		//delay_ms(1000);		
    }
}
