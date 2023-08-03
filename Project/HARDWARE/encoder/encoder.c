#include "encoder.h"

#include "stdio.h"
#include "delay.h"

uint8_t ext_key_flg = 0;

//������ʼ������
void extix_gpio_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIOA0

}

//�ⲿ�ж�2�������
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
	EXTI_ClearITPendingBit(EXTI_Line2);//���LINE2�ϵ��жϱ�־λ 
}
//�ⲿ�ж�3�������
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
	EXTI_ClearITPendingBit(EXTI_Line3);  //���LINE3�ϵ��жϱ�־λ  
}

//�ⲿ�жϳ�ʼ������
//��ʼ��PE2~4,PA0Ϊ�ж�����.
void EXTIX_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;

	extix_gpio_init(); //������Ӧ��IO�ڳ�ʼ��

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);//PE2 ���ӵ��ж���2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);//PE3 ���ӵ��ж���3



	/* ����EXTI_Line2,3,4 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line2 | EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //�½��ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//�ж���ʹ��
	EXTI_Init(&EXTI_InitStructure);//����



	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;//�ⲿ�ж�2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);//����


	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;//�ⲿ�ж�3
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);//����
}


