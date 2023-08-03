#include "st7789_hal_spi1.h"
#include "sys.h"

#include "stm32f4xx_spi.h"
#include "delay.h"

#if USE_DMA_SPI1
#include "hal_spi1_dma.h"
#endif // USE_DMA_SPI

SPI_InitTypeDef  SPI_InitStructure;

void hal_spi1_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    //SPI1的时钟来源是APB2 84MHz
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	//使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);	//使能SPI1时钟

    //PA5--SCK ,PA7--MOSI初始化设置,
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;//PA5,PA7复用功能输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1); //PA5复用为 SPI1

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1); //PA7复用为 SPI1

    //这里只针对SPI口初始化
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);//复位SPI1
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);//停止复位SPI1

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//串行同步时钟的空闲状态为低电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第一个跳变沿（上升或下降）数据被采样
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		//定义波特率预分频的值:波特率预分频值为
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
    SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

    SPI_Cmd(SPI1, ENABLE); //使能SPI1外设
    hal_spi1_rw_data(0xff);//启动传输
}

//液晶IO初始化配置
void hal_spi1_gpio_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_1 | GPIO_Pin_10  | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    hal_spi1_init();//初始化硬件SPI1
	
	HAL_SPI_RST_SET;
	HAL_SPI_BLK_SET;
	HAL_SPI_BLK_CLR;
	HAL_SPI_BLK_SET;
	
#if USE_DMA_SPI1
	DMA_Config((uint32_t)SpiDmaBuffer, DMA_SPI_BUF_SIZE);
#endif // USE_DMA_SPI
}

//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
uint8_t hal_spi1_rw_data(uint16_t TxData)
{
//    uint8_t retry=0;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
    {
//        retry++;
//        if(retry>200)return 0;
    }
    SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个数据
//    retry=0;

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)//检查指定的SPI标志位设置与否:接受缓存非空标志位
    {
//        retry++;
//        if(retry>200)return 0;
    }
    return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据
}
/******************************************************************************************/
//向屏幕写一个8位指令
void hal_spi1_write_cmd(uint8_t cmd)
{
	//SPI 写命令时序开始
    HAL_SPI_CS_CLR;
    HAL_SPI_DC_CLR;
	
	hal_spi1_rw_data(cmd);

    HAL_SPI_CS_SET;
}

//向SPI总线传输一个8位数据
void hal_spi1_write_8bit_data(uint8_t data)
{
	//SPI 写数据时序开始
    HAL_SPI_CS_CLR;
    HAL_SPI_DC_SET;
	
	hal_spi1_rw_data(data);
	
	HAL_SPI_CS_SET;
}

//向屏幕写一个16位数据
void hal_spi1_write_16bit_data(uint16_t data)
{	
    HAL_SPI_CS_CLR;
    HAL_SPI_DC_SET;

	//写入高8位数据
	//hal_spi1_rw_data(data>>8);
	//写入低8位数据
	hal_spi1_rw_data(data);

	
    HAL_SPI_CS_SET;
}

void hal_spi1_txbits(uint8_t bit)
{
	uint16_t spiBitSet = 0;
	if(bit == 16)
	{
		spiBitSet = SPI_DataSize_16b;
	}
	else
		spiBitSet = SPI_DataSize_8b;
    //这里只针对SPI口初始化
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);//复位SPI1
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);//停止复位SPI1

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
    SPI_InitStructure.SPI_DataSize = spiBitSet;		//设置SPI的数据大小:SPI发送接收8位帧结构
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//串行同步时钟的空闲状态为低电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第一个跳变沿（上升或下降）数据被采样
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		//定义波特率预分频的值:波特率预分频值为
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
    SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

    SPI_Cmd(SPI1, ENABLE); //使能SPI外设
}

//屏幕复位
void hal_spi1_hard_reset(void)
{
    HAL_SPI_RST_CLR;//RST引脚输出为低
    delay_ms(1000);
    HAL_SPI_RST_SET;//RST引脚输出为高
    delay_ms(100);
}

/**********************************************************************************/
/*************************************************
函数名：LCD_Set_Region
功能：设置lcd显示区域，在此区域写点数据自动换行
入口参数：xy起点和终点,Y_IncMode表示先自增y再自增x
返回值：无
*************************************************/
void hal_spi1_setregion(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
    hal_spi1_write_cmd(0x2a);
    hal_spi1_write_8bit_data(x_start>>8);
    hal_spi1_write_8bit_data(x_start);
    hal_spi1_write_8bit_data(x_end>>8);
    hal_spi1_write_8bit_data(x_end);

    hal_spi1_write_cmd(0x2b);
    hal_spi1_write_8bit_data(y_start>>8);
    hal_spi1_write_8bit_data(y_start);
    hal_spi1_write_8bit_data(y_end>>8);
    hal_spi1_write_8bit_data(y_end);

    hal_spi1_write_cmd(0x2c); //开始写入GRAM
}

// 屏幕清屏
void hal_spi1_clear(uint16_t color)
{
	//uint16_t row,column;
	int i = 0;

	hal_spi1_setregion(0, 0, LCD_WIDTH, LCD_HEIGHT);
	
    HAL_SPI_CS_CLR;
    HAL_SPI_DC_SET;
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);//PA1  设置低，灯亮

	hal_spi1_txbits(16);
    for(i = 0 ; i < 240*240; i++)
    {
        hal_spi1_write_16bit_data(color);
    }
	GPIO_SetBits(GPIOA,GPIO_Pin_1);//PA1  设置高，灯灭
	HAL_SPI_CS_SET;
	hal_spi1_txbits(8);
}

void hal_spi1_lcd_init(void)
{
	hal_spi1_txbits(8);
	/********************************************************/
    delay_ms(1000);
    hal_spi1_write_cmd(ST7789_CMD_SLPOUT); 	//Sleep Out
    delay_ms(120);
    //-----------------------ST7789V Frame rate setting-----------------//
    //************************************************
    hal_spi1_write_cmd(ST7789_CMD_COLMOD);       //65k mode
    hal_spi1_write_8bit_data(0x05);

    hal_spi1_write_cmd(ST7789_CMD_VCMOFSET); 	//VCOM
    hal_spi1_write_8bit_data(0x1A);

    hal_spi1_write_cmd(ST7789_CMD_MADCTL);       // 屏幕显示方向设置
    hal_spi1_write_8bit_data(0x00);
    //-------------ST7789V Frame rate setting-----------//
    hal_spi1_write_cmd(ST7789_CMD_PORCTRL);		//Porch Setting
    hal_spi1_write_8bit_data(0x05);
    hal_spi1_write_8bit_data(0x05);
    hal_spi1_write_8bit_data(0x00);
    hal_spi1_write_8bit_data(0x33);
    hal_spi1_write_8bit_data(0x33);

    hal_spi1_write_cmd(ST7789_CMD_GCTRL);		//Gate Control
    hal_spi1_write_8bit_data(0x05);			//12.2v   -10.43v
    //--------------ST7789V Power setting---------------//
    hal_spi1_write_cmd(ST7789_CMD_VCOMS);//VCOM
    hal_spi1_write_8bit_data(0x3F);

    hal_spi1_write_cmd(ST7789_CMD_LCMCTRL); //Power control
    hal_spi1_write_8bit_data(0x2c);

    hal_spi1_write_cmd(ST7789_CMD_VDVVRHEN);	//VDV and VRH Command Enable
    hal_spi1_write_8bit_data(0x01);

    hal_spi1_write_cmd(ST7789_CMD_VRHS);		//VRH Set
    hal_spi1_write_8bit_data(0x0F);		//4.3+( vcom+vcom offset+vdv)

    hal_spi1_write_cmd(ST7789_CMD_VDVSET);		//VDV Set
    hal_spi1_write_8bit_data(0x20);				//0v

    hal_spi1_write_cmd(ST7789_CMD_FRCTR2);		//Frame Rate Control in Normal Mode
    hal_spi1_write_8bit_data(0X01);			     //111Hz

    hal_spi1_write_cmd(ST7789_CMD_PWCTRL1);		//Power Control 1
    hal_spi1_write_8bit_data(0xa4);
    hal_spi1_write_8bit_data(0xa1);

    hal_spi1_write_cmd(ST7789_CMD_PWCTRL2);		//Power Control 1
    hal_spi1_write_8bit_data(0x03);

    hal_spi1_write_cmd(ST7789_CMD_EQCTRL);		//Equalize time control
    hal_spi1_write_8bit_data(0x09);
    hal_spi1_write_8bit_data(0x09);
    hal_spi1_write_8bit_data(0x08);
    //---------------ST7789V gamma setting-------------//
    hal_spi1_write_cmd(ST7789_CMD_PVGAMCTRL); //Set Gamma
    hal_spi1_write_8bit_data(0xD0);
    hal_spi1_write_8bit_data(0x05);
    hal_spi1_write_8bit_data(0x09);
    hal_spi1_write_8bit_data(0x09);
    hal_spi1_write_8bit_data(0x08);
    hal_spi1_write_8bit_data(0x14);
    hal_spi1_write_8bit_data(0x28);
    hal_spi1_write_8bit_data(0x33);
    hal_spi1_write_8bit_data(0x3F);
    hal_spi1_write_8bit_data(0x07);
    hal_spi1_write_8bit_data(0x13);
    hal_spi1_write_8bit_data(0x14);
    hal_spi1_write_8bit_data(0x28);
    hal_spi1_write_8bit_data(0x30);

    hal_spi1_write_cmd(ST7789_CMD_NVGAMCTRL); //Set Gamma
    hal_spi1_write_8bit_data(0xD0);
    hal_spi1_write_8bit_data(0x05);
    hal_spi1_write_8bit_data(0x09);
    hal_spi1_write_8bit_data(0x09);
    hal_spi1_write_8bit_data(0x08);
    hal_spi1_write_8bit_data(0x03);
    hal_spi1_write_8bit_data(0x24);
    hal_spi1_write_8bit_data(0x32);
    hal_spi1_write_8bit_data(0x32);
    hal_spi1_write_8bit_data(0x3B);
    hal_spi1_write_8bit_data(0x14);
    hal_spi1_write_8bit_data(0x13);
    hal_spi1_write_8bit_data(0x28);
    hal_spi1_write_8bit_data(0x2F);

    hal_spi1_write_cmd(ST7789_CMD_INVON);		//反显

    hal_spi1_write_cmd(ST7789_CMD_DISPON);		//开启显示
}
/*********************************************END OF FILE********************************************/

