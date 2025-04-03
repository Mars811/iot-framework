#ifndef xfmc_h
#define xfmc_h

#include "stdint.h"
#include "stm32f10x_flash.h"

#define BOOTLOADER_SADDRESS     (uint32_t)0x08000000
#define TOTAL_PAGE_NUM          64
#define BOOT_PAGE_NUM           16
#define APP_PAGE_NUM            TOTAL_PAGE_NUM - BOOT_PAGE_NUM
#define APP_SADDRESS            BOOTLOADER_SADDRESS + BOOT_PAGE_NUM * 1024

void xfmc_erase_flash(uint16_t start_addr, uint16_t len);
void xfmc_write_flash(uint32_t start_addr, uint32_t *write_data, uint32_t len);
void xfmc_erase_app(void);

#endif