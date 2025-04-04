/**
 ****************************************************************************************************
 * @file        24cxx.h
 * @author      Mr.Mr.
 * @version     V1.0
 * @date        2023-04-23
 * @brief       24CXX驱动代码
 * @license     Copyright (c) 2020-2032,  
 ****************************************************************************************************
 * @attention
 * 
 * 实验平台:ST-1 STM32F407核心板
 *  
 *  
 * 公司网址:www.genbotter.com
 * 购买地址:makerbase.taobao.com
 * 
 ****************************************************************************************************
 */

#ifndef __24CXX_H
#define __24CXX_H

#include "./SYSTEM/sys/sys.h"

/* 24CXX型号定义 */
#define AT24C01     ((1 << 7) - 1)
#define AT24C02     ((1 << 8) - 1)
#define AT24C04     ((1 << 9) - 1)
#define AT24C08     ((1 << 10) - 1)
#define AT24C16     ((1 << 11) - 1)
#define AT24C32     ((1 << 12) - 1)
#define AT24C64     ((1 << 13) - 1)
#define AT24C128    ((1 << 14) - 1)
#define AT24C256    ((1 << 15) - 1)

/* 板载24CXX型号定义 */
#define EE_TYPE     AT24C02

/* 函数声明 */
void at24cxx_init(void);                                            /* 初始化AT24CXX */
uint8_t at24cxx_check(void);                                        /* 检测AT24CXX是否正常 */
uint8_t at24cxx_read_one_byte(uint16_t addr);                       /* 从AT24CXX指定地址读取一字节数据 */
void at24cxx_write_one_byte(uint16_t addr,uint8_t data);            /* 往AT24CXX指定地址写入一字节数据 */
void at24cxx_read(uint16_t addr, uint8_t *pbuf, uint16_t datalen);  /* 从AT24CXX指定地址开始读取指定长度的数据 */
void at24cxx_write(uint16_t addr, uint8_t *pbuf, uint16_t datalen); /* 往AT24CXX指定地址开始写入指定长度的数据 */

#endif
