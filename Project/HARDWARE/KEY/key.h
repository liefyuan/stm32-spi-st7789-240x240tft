#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 

//STM32F407开发板
//按键输入驱动
								  
////////////////////////////////////////////////////////////////////////////////// 	 

/*下面的方式是通过直接操作库函数方式读取IO*/
#define WK_UP 	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)	//PA0--对应K1按键


/*下面方式是通过位带操作方式读取IO*/
/*
#define WK_UP 	PAin(0)		//PA0
*/


#define WKUP_PRES   1

void KEY_Init(void);	//IO初始化
u8 KEY_Scan(u8);  		//按键扫描函数	

#endif
