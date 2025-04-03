#ifndef W25Q64_H
#define W25Q64_H

#include "stm32f10x.h"
#include <stdint.h>

// 硬件接口宏定义
#define W25Q64_SPI               SPI1
#define W25Q64_SPI_RCC_CLOCK     RCC_APB2Periph_SPI1

// GPIO 硬件定义
#define W25Q64_GPIO_PORT         GPIOA
#define W25Q64_GPIO_RCC_CLOCK    RCC_APB2Periph_GPIOA
#define W25Q64_PIN_SCK           GPIO_Pin_5
#define W25Q64_PIN_MISO          GPIO_Pin_6
#define W25Q64_PIN_MOSI          GPIO_Pin_7
#define W25Q64_PIN_CS            GPIO_Pin_4  // 软件片选

// SPI 配置参数
#define W25Q64_SPI_MODE          SPI_Mode_Master
#define W25Q64_SPI_DATA_SIZE     SPI_DataSize_8b
#define W25Q64_SPI_CPOL          SPI_CPOL_Low
#define W25Q64_SPI_CPHA          SPI_CPHA_1Edge
#define W25Q64_SPI_BAUDRATE      SPI_BaudRatePrescaler_4

// 宏控制
#define W25Q64_ENABLE()            GPIO_WriteBit(W25Q64_GPIO_PORT, W25Q64_PIN_CS, 0)
#define W25Q64_DISABLE()           GPIO_WriteBit(W25Q64_GPIO_PORT, W25Q64_PIN_CS, 1)

void w25q64_init(void);
uint8_t w25q64_read_write_byte(uint8_t txd);
void w25q64_write_byte(uint8_t *write_data, uint16_t len);
void w25q64_read_byte(uint8_t *read_data, uint16_t len);
void w25q64_wait_not_busy(void);
void w25q64_write_enable(void);
void w25q64_erase_64k(uint8_t block_num);
void w25q64_write_page(uint8_t *wbuff, uint16_t page_num);
void w25q64_read_page(uint8_t *rbuff, uint32_t addr, uint32_t len);
void w25q64_get_id(uint8_t *manufacturer_id, uint8_t *device_id);

#endif
