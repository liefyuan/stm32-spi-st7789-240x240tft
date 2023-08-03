#include "st7789_gpio_spi.h"

#include "delay.h"

//屏幕IO初始化配置
void gpio_spi_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5| GPIO_Pin_7;
	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     //普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      //上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_11;
	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     //普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      //上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SPI_SDA_SET;
	GPIO_SPI_SCL_SET;
	
	GPIO_SPI_RST_SET;

	GPIO_SPI_BLK_SET;
	GPIO_SPI_BLK_CLR;
	GPIO_SPI_BLK_SET;
}

void gpio_spi_write(uint8_t data)
{
	uint8_t i;
	for(i = 0; i < 8; i++)    // 295 ms 1 frame
    {
        //if((data&0x80) != 0)
		if(data&0x80)
        {
            GPIO_SPI_SDA_SET;
        }
        else
		{
            GPIO_SPI_SDA_CLR;
		}

        GPIO_SPI_SCL_CLR;
        GPIO_SPI_SCL_SET;
        data <<= 1;
    }
}

//向屏幕写一个8位指令
void gpio_spi_write_cmd(uint8_t cmd)
{
	//SPI 写命令时序开始
    GPIO_SPI_CS_CLR;
    GPIO_SPI_DC_CLR;

	gpio_spi_write(cmd);

    GPIO_SPI_CS_SET;
}

//向SPI总线传输一个8位数据
void gpio_spi_write_8bit_data(uint8_t data)
{
	//SPI 写数据时序开始
    GPIO_SPI_CS_CLR;
    GPIO_SPI_DC_SET;
	
	gpio_spi_write(data);
	
	GPIO_SPI_CS_SET;
}

//向屏幕写一个16位数据
void gpio_spi_write_16bit_data(uint16_t data)
{	
    GPIO_SPI_CS_CLR;
    GPIO_SPI_DC_SET;

	gpio_spi_write(data>>8);
	gpio_spi_write(data);
	
    GPIO_SPI_CS_SET;
}

//屏幕复位
void gpio_spi_hard_reset(void)
{
    GPIO_SPI_RST_CLR;//RST引脚输出为低
    delay_ms(1000);
    GPIO_SPI_RST_SET;//RST引脚输出为高
    delay_ms(100);
}

/**********************************************************************************/
/*************************************************
函数名：LCD_Set_Region
功能：设置lcd显示区域，在此区域写点数据自动换行
入口参数：xy起点和终点,Y_IncMode表示先自增y再自增x
返回值：无
*************************************************/
static void Lcd_SetRegion(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
    gpio_spi_write_cmd(0x2a);
    gpio_spi_write_8bit_data(x_start>>8);
    gpio_spi_write_8bit_data(x_start);
    gpio_spi_write_8bit_data(x_end>>8);
    gpio_spi_write_8bit_data(x_end);

    gpio_spi_write_cmd(0x2b);
    gpio_spi_write_8bit_data(y_start>>8);
    gpio_spi_write_8bit_data(y_start);
    gpio_spi_write_8bit_data(y_end>>8);
    gpio_spi_write_8bit_data(y_end);

    gpio_spi_write_cmd(0x2c); //开始写入GRAM
}

// 屏幕清屏
void gpio_spi_clear(uint16_t color)
{
	uint16_t row,column;

	Lcd_SetRegion(0, 0, ST7789_LCD_WIDTH, ST7789_LCD_HEIGHT);
	
    GPIO_SPI_CS_CLR;
    GPIO_SPI_DC_SET;
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);//PA1  设置低，灯亮
	for(row = 0;row < ST7789_LCD_WIDTH; row++)             //ROW loop
	{
		for(column = 0;column < ST7789_LCD_HEIGHT; column++) //column loop
		{
			gpio_spi_write_16bit_data(color); // 180ms 1 frame
		}
	}
	GPIO_SetBits(GPIOA,GPIO_Pin_1);//PA1  设置高，灯灭
	GPIO_SPI_CS_SET;
}

void gpio_spi_lcd_init(void)
{
    gpio_spi_write_cmd(0x36);
    gpio_spi_write_8bit_data(0x00);

    gpio_spi_write_cmd(0x3A);
    gpio_spi_write_8bit_data(0x05);

    gpio_spi_write_cmd(0xB2);
    gpio_spi_write_8bit_data(0x0C);
    gpio_spi_write_8bit_data(0x0C);
    gpio_spi_write_8bit_data(0x00);
    gpio_spi_write_8bit_data(0x33);
    gpio_spi_write_8bit_data(0x33);

    gpio_spi_write_cmd(0xB7);
    gpio_spi_write_8bit_data(0x35);

    gpio_spi_write_cmd(0xBB);
    gpio_spi_write_8bit_data(0x19);

    gpio_spi_write_cmd(0xC0);
    gpio_spi_write_8bit_data(0x2C);

    gpio_spi_write_cmd(0xC2);
    gpio_spi_write_8bit_data(0x01);

    gpio_spi_write_cmd(0xC3);
    gpio_spi_write_8bit_data(0x12);

    gpio_spi_write_cmd(0xC4);
    gpio_spi_write_8bit_data(0x20);

    gpio_spi_write_cmd(0xC6);
    gpio_spi_write_8bit_data(0x0F);

    gpio_spi_write_cmd(0xD0);
    gpio_spi_write_8bit_data(0xA4);
    gpio_spi_write_8bit_data(0xA1);

    gpio_spi_write_cmd(0xE0);
    gpio_spi_write_8bit_data(0xD0);
    gpio_spi_write_8bit_data(0x04);
    gpio_spi_write_8bit_data(0x0D);
    gpio_spi_write_8bit_data(0x11);
    gpio_spi_write_8bit_data(0x13);
    gpio_spi_write_8bit_data(0x2B);
    gpio_spi_write_8bit_data(0x3F);
    gpio_spi_write_8bit_data(0x54);
    gpio_spi_write_8bit_data(0x4C);
    gpio_spi_write_8bit_data(0x18);
    gpio_spi_write_8bit_data(0x0D);
    gpio_spi_write_8bit_data(0x0B);
    gpio_spi_write_8bit_data(0x1F);
    gpio_spi_write_8bit_data(0x23);

    gpio_spi_write_cmd(0xE1);
    gpio_spi_write_8bit_data(0xD0);
    gpio_spi_write_8bit_data(0x04);
    gpio_spi_write_8bit_data(0x0C);
    gpio_spi_write_8bit_data(0x11);
    gpio_spi_write_8bit_data(0x13);
    gpio_spi_write_8bit_data(0x2C);
    gpio_spi_write_8bit_data(0x3F);
    gpio_spi_write_8bit_data(0x44);
    gpio_spi_write_8bit_data(0x51);
    gpio_spi_write_8bit_data(0x2F);
    gpio_spi_write_8bit_data(0x1F);
    gpio_spi_write_8bit_data(0x1F);
    gpio_spi_write_8bit_data(0x20);
    gpio_spi_write_8bit_data(0x23);

    gpio_spi_write_cmd(0x21);

    gpio_spi_write_cmd(0x11);

    gpio_spi_write_cmd(0x29);
}
