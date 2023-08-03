#include "encoder.h"

#include "stdio.h"
#include "delay.h"

uint8_t ext_key_flg = 0;

//按键初始化函数
void extix_gpio_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOA0

}

//外部中断2服务程序
void EXTI2_IRQHandler(void)
{
	static uint8_t oldState = 0, newState = 0;
	
	newState = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2);
	
	if(newState != oldState)
	{
		printf("change\r\n");
		if(newState == 1)
			ext_key_flg = 1;
		else
			ext_key_flg = 2;
	}
	
//	if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2) == 1)
//	{
//		printf("PE2: High ");
//		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) == 1)
//		{
//			printf("PE3: High\r\n");
//		}
//		else if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) == 0)
//		{
//			printf("PE3: Low\r\n");
//		}	
//	}
//	else if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2) == 0)
//	{
//		printf("PE2: Low ");
//		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) == 1)
//		{
//			printf("PE3: High\r\n");
//		}
//		else if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) == 0)
//		{
//			printf("PE3: Low\r\n");
//		}
//	}
	
	oldState = newState;
//	ext_key_flg = 1;
//	printf("PE2\r\n");
	EXTI_ClearITPendingBit(EXTI_Line2);//清除LINE2上的中断标志位 
}
//外部中断3服务程序
void EXTI3_IRQHandler(void)
{
//	delay_ms(5);
//	if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) == 1)
//	{
//		printf("PE3: High\r\n");
//	}
//	else if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) == 0)
//	{
//		printf("PE3: Low\r\n");
//	}
//	ext_key_flg = 2;
	//printf("PE3\r\n");
	EXTI_ClearITPendingBit(EXTI_Line3);  //清除LINE3上的中断标志位  
}

//外部中断初始化程序
//初始化PE2~4,PA0为中断输入.
void EXTIX_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;

	extix_gpio_init(); //按键对应的IO口初始化

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);//PE2 连接到中断线2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);//PE3 连接到中断线3



	/* 配置EXTI_Line2,3,4 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line2 | EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//中断线使能
	EXTI_Init(&EXTI_InitStructure);//配置



	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;//外部中断2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置


	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;//外部中断3
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置
}


