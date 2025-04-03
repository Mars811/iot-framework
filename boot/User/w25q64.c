#include "w25q64.h"

void w25q64_init(void){
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    // ����ʱ��
    RCC_APB2PeriphClockCmd(W25Q64_GPIO_RCC_CLOCK | W25Q64_SPI_RCC_CLOCK, ENABLE);

    // ����SPI����
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    // SCK/MOSI �����������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = W25Q64_PIN_SCK | W25Q64_PIN_MOSI;
    GPIO_Init(W25Q64_GPIO_PORT, &GPIO_InitStructure);

    // ʹ��Ƭѡ����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = W25Q64_PIN_CS;
    GPIO_Init(W25Q64_GPIO_PORT, &GPIO_InitStructure);
    
    // MISO �������루��һ�£�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = W25Q64_PIN_MISO;
    GPIO_Init(W25Q64_GPIO_PORT, &GPIO_InitStructure);

    // SPI ��������
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
    // �ȴ����ͻ���������
    while(SPI_I2S_GetFlagStatus(W25Q64_SPI, SPI_I2S_FLAG_TXE) != SET);

    // �������ݴ���ʱ��
    SPI_I2S_SendData(W25Q64_SPI, txd);
    
    // �ȴ��������
    while(SPI_I2S_GetFlagStatus(W25Q64_SPI, SPI_I2S_FLAG_RXNE) != SET);
    
    // ���ؽ�������
    return SPI_I2S_ReceiveData(W25Q64_SPI);
}

/**
 * ----------------------------------------------------------------------
 * @brief              д����
 * @param  write_data  ��������
 * @param  len         �ֽ���
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

        // ��������Ƭѡ������ָ�������
        W25Q64_DISABLE();

    // �ȴ���æ״̬
    }while((res & 0x01) == 0x01);
}

/**
 * ----------------------------------------------------------------------
 * @brief дʹ��
 * ----------------------------------------------------------------------
 * �ڶ� W25Q64 ����д��������ҳ��̡��������������������Ƭ�����ȣ�֮ǰ��������
 * ����дʹ��ָ�������ΪоƬ�ڲ���һ��д�������ƣ�Ĭ������²��������д������
 * ͨ������ 0x06 ָ����Խ�оƬ�ڲ���״̬�Ĵ����е�дʹ������λ��WEL����Ϊ 1��
 * �Ӷ����������д������
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
 * @brief             64k��������
 * @param  block_num  ������
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
 * @brief            дҳ
 * @param  wbuff     
 * @param  page_num  
 * ----------------------------------------------------------------------
 * һ��д������ݳ��ȱ�����256�ֽڡ���ΪW25Q64��һҳ��256�ֽڡ�����ͨ����ַ����
 * ��ҳ�ţ�������Ӧ��������Ȼ��д�����ݡ�
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
 * @brief         ��ҳ
 * @param  rbuff  ������������
 * @param  addr   �����ַ��ȡ
 * @param  len    Ҫ��ȡ�ĳ���
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
 * @brief ��ȡ W25Q64 �ĳ��� ID ���豸 ID
 * @param manufacturer_id ���ڴ洢���� ID ��ָ��
 * @param device_id ���ڴ洢�豸 ID ��ָ��
 */
void w25q64_get_id(uint8_t *manufacturer_id, uint8_t *device_id) {
    // �ȴ�оƬ��æ
    w25q64_wait_not_busy();

    // ʹ��Ƭѡ�ź�
    W25Q64_ENABLE();

    // ���ͻ�ȡ ID ���� 0x90
    w25q64_read_write_byte(0x90);

    // ���� 24 λ��ַ������ͨ���� 0x000000
    w25q64_read_write_byte(0x00);
    w25q64_read_write_byte(0x00);
    w25q64_read_write_byte(0x00);

    // ��ȡ���� ID
    *manufacturer_id = w25q64_read_write_byte(0xFF);

    // ��ȡ�豸 ID
    *device_id = w25q64_read_write_byte(0xFF);

    // ����Ƭѡ�ź�
    W25Q64_DISABLE();
}