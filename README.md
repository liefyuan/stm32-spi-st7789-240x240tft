# stm32-spi-st7789-240x240tft
# 区别
另一个仓库：https://github.com/liefyuan/stm32-spi-st7789-tft 与这个仓库的区别：
- 本仓库代码工程使用240*240的屏幕，因为我想在单片机内部直接开辟一个240x240x2的一个DMA缓存区，而320x240分辨率的屏幕由于我的单片机stm32f407vgt6的内存没有那么大所以无法做到。
- SPI这里我使用了16位传输模式，可以完美适配DMA传输快速刷图的计划。
- 工程里面留下了对PA1引脚的操作代码（外接了一颗LED灯），目的是使用示波器测刷一张图需要的时间。

# 刷图测试工程

# 基本介绍
- 主控芯片：STM32F407VGT6
- 屏幕驱动芯片：ST7789
- 屏幕分辨率：240x240
- 材质：ips
- 屏幕驱动方式：三线SPI方式（SDA，SCL，DC）

屏幕引脚：
- VCC
- GND
- SDA
- SCL
- CS
- RES
- DC
- BLK

# 刷图测试
自测满屏刷一种颜色
| 方式 | 一帧耗时 | 帧率 |
|--|--|--|
| GPIO模拟SPI | 232ms | 4.3fps |
| 硬件SPI外设（8bit） | 141ms | 7fps |
| 硬件SPI外设（16bit） | 107ms | 9.3fps |
| 硬件SPI外设（16bit）+DMA发送 | 43.6ms | 23fps |


# 刷图方式选择
- 打开GPIO模拟SPI：`lcd_driver.c`文件里面宏`#define USE_HAL_SPI`置为 0
- 打开硬件SPI外设：`lcd_driver.c`文件里面宏`#define USE_HAL_SPI`置为 1

如何打开spi和DMA呢？
- 先打开硬件SPI外设：`lcd_driver.c`文件里面宏`#define USE_HAL_SPI`置为 1
- 再打开DMA：`st7789_hal_spi.h`文件里面宏`#define USE_DMA_SPI`置为 1
