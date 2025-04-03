#include "xfmc.h"

/**
 * ----------------------------------------------------------------------
 * @brief              flash����
 * @param  start_addr  ��ʼ��ַ
 * @param  len         ����ҳ����һ�β���Ϊ1kb��
 * ----------------------------------------------------------------------
**/
void xfmc_erase_flash(uint16_t start_addr, uint16_t len){
    uint16_t i;
    FLASH_Unlock();
    for(i = 0; i < len; i++){
       FLASH_ErasePage(start_addr + i*1024);
    }
}

/**
 * ----------------------------------------------------------------------
 * @brief              flashд��
 * @param  start_addr  ��ʼ��ַ
 * @param  write_data  д������
 * @param  len         д�볤��(��λ�ֽ�)
 * ----------------------------------------------------------------------
**/
void xfmc_write_flash(uint32_t start_addr, uint32_t *write_data, uint32_t len){
    FLASH_Unlock();
    while(len){
        FLASH_ProgramWord(start_addr, *write_data);
        len-=4;
        start_addr+=4;
        write_data++;
    }
    FLASH_Lock();
}

void xfmc_erase_app(void){
    xfmc_erase_flash(APP_SADDRESS, APP_PAGE_NUM);
}