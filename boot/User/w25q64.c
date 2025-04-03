#include "w25q64.h"

void w25q64_init(void){
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    // 启用时钟
    RCC_APB2PeriphClockCmd(W25Q64_GPIO_RCC_CLOCK | W25Q64_SPI_RCC_CLOCK, ENABLE);

    // 配置SPI引脚
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    // SCK/MOSI 复用推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = W25Q64_PIN_SCK | W25Q64_PIN_MOSI;
    GPIO_Init(W25Q64_GPIO_PORT, &GPIO_InitStructure);

    // 使能片选引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = W25Q64_PIN_CS;
    GPIO_Init(W25Q64_GPIO_PORT, &GPIO_InitStructure);
    
    // MISO 浮空输入（改一下）
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = W25Q64_PIN_MISO;
    GPIO_Init(W25Q64_GPIO_PORT, &GPIO_InitStructure);

    // SPI 参数配置
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = W25Q64_SPI_MODE;
    SPI_InitStructure.SPI_DataSize = W25Q64_SPI_DATA_SIZE;
    SPI_InitStructure.SPI_CPOL = W25Q64_SPI_CPOL;
    SPI_InitStructure.SPI_CPHA = W25Q64_SPI_CPHA;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = W25Q64_SPI_BAUDRATE;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(W25Q64_SPI, &SPI_InitStructure);

    SPI_Cmd(W25Q64_SPI, ENABLE);

    W25Q64_DISABLE();
}

uint8_t w25q64_read_write_byte(uint8_t txd){
    // 等待发送缓冲区就绪
    while(SPI_I2S_GetFlagStatus(W25Q64_SPI, SPI_I2S_FLAG_TXE) != SET);

    // 发送数据触发时钟
    SPI_I2S_SendData(W25Q64_SPI, txd);
    
    // 等待接收完成
    while(SPI_I2S_GetFlagStatus(W25Q64_SPI, SPI_I2S_FLAG_RXNE) != SET);
    
    // 返回接收数据
    return SPI_I2S_ReceiveData(W25Q64_SPI);
}

/**
 * ----------------------------------------------------------------------
 * @brief              写数据
 * @param  write_data  数据数组
 * @param  len         字节数
 * ----------------------------------------------------------------------
**/
void w25q64_write_byte(uint8_t *write_data, uint16_t len){
    uint16_t i;
    for(i = 0; i < len; i++){
        w25q64_read_write_byte(write_data[i]);
    }
}

void w25q64_read_byte(uint8_t *read_data, uint16_t len){
   uint16_t i;
   for(i = 0; i < len; i++){
       read_data[i] = w25q64_read_write_byte(0xff);
   } 
}

void w25q64_wait_not_busy(void){
    uint8_t res;
    do{
        W25Q64_ENABLE();
        w25q64_read_write_byte(0x05); 
        res = w25q64_read_write_byte(0xff);

        // 马上拉低片选，告诉指令读完了
        W25Q64_DISABLE();

    // 等待不忙状态
    }while((res & 0x01) == 0x01);
}

/**
 * ----------------------------------------------------------------------
 * @brief 写使能
 * ----------------------------------------------------------------------
 * 在对 W25Q64 进行写操作（如页编程、扇区擦除、块擦除、整片擦除等）之前，必须先
 * 发送写使能指令。这是因为芯片内部有一个写保护机制，默认情况下不允许进行写操作，
 * 通过发送 0x06 指令，可以将芯片内部的状态寄存器中的写使能锁存位（WEL）置为 1，
 * 从而允许后续的写操作。
 * ----------------------------------------------------------------------
**/
void w25q64_write_enable(void){
    w25q64_wait_not_busy();
    W25Q64_ENABLE();
    w25q64_read_write_byte(0x06);
    W25Q64_DISABLE(); 
}

/**
 * ----------------------------------------------------------------------
 * @brief             64k扇区擦除
 * @param  block_num  扇区号
 * ----------------------------------------------------------------------
**/
void w25q64_erase_64k(uint8_t block_num){
   uint8_t cmd[4];
   cmd[0] = 0xD8;
   cmd[1] = (block_num * 64 * 1024) >> 16;
   cmd[2] = (block_num * 64 * 1024) >> 8;
   cmd[3] = (block_num * 64 * 1024) >> 0;

   w25q64_wait_not_busy();
   w25q64_write_enable();
   W25Q64_ENABLE();
   w25q64_write_byte(cmd, 4);
   W25Q64_DISABLE();
   w25q64_wait_not_busy();
}

/**
 * ----------------------------------------------------------------------
 * @brief            写页
 * @param  wbuff     
 * @param  page_num  
 * ----------------------------------------------------------------------
 * 一次写入的数据长度必须是256字节。因为W25Q64的一页是256字节。首先通过地址计算
 * 出页号，擦除对应的扇区，然后写入数据。
 * ----------------------------------------------------------------------
**/
void w25q64_write_page(uint8_t *wbuff, uint16_t page_num){
    uint8_t cmd[4];
    cmd[0] = 0x02;
    cmd[1] = (page_num * 256) >> 16;
    cmd[2] = (page_num * 256) >> 8;
    cmd[3] = (page_num * 256) >> 0;

    w25q64_wait_not_busy();
    w25q64_write_enable();
    W25Q64_ENABLE();
    w25q64_write_byte(cmd, 4);
    w25q64_write_byte(wbuff, 256);
    W25Q64_DISABLE();
}

/**
 * ----------------------------------------------------------------------
 * @brief         读页
 * @param  rbuff  接收数据数组
 * @param  addr   任意地址读取
 * @param  len    要读取的长度
 * ----------------------------------------------------------------------
**/
void w25q64_read_page(uint8_t *rbuff, uint32_t addr, uint32_t len){
    uint8_t cmd[4];
    cmd[0] = 0x03;
    cmd[1] = (addr) >> 16;
    cmd[2] = (addr) >> 8;
    cmd[3] = (addr) >> 0;

    w25q64_wait_not_busy();
    W25Q64_ENABLE();
    w25q64_write_byte(cmd, 4);
    w25q64_read_byte(rbuff, len);
    W25Q64_DISABLE();
}


/**
 * @brief 获取 W25Q64 的厂家 ID 和设备 ID
 * @param manufacturer_id 用于存储厂家 ID 的指针
 * @param device_id 用于存储设备 ID 的指针
 */
void w25q64_get_id(uint8_t *manufacturer_id, uint8_t *device_id) {
    // 等待芯片不忙
    w25q64_wait_not_busy();

    // 使能片选信号
    W25Q64_ENABLE();

    // 发送获取 ID 命令 0x90
    w25q64_read_write_byte(0x90);

    // 发送 24 位地址，这里通常填 0x000000
    w25q64_read_write_byte(0x00);
    w25q64_read_write_byte(0x00);
    w25q64_read_write_byte(0x00);

    // 读取厂家 ID
    *manufacturer_id = w25q64_read_write_byte(0xFF);

    // 读取设备 ID
    *device_id = w25q64_read_write_byte(0xFF);

    // 禁用片选信号
    W25Q64_DISABLE();
}