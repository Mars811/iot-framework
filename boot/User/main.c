#include "stm32f10x.h"
#include "xfmc.h"
#include "xuart.h"
#include "w25q64.h"
#include "Delay.h"
#include "boot_key.h"
#include "xmodem.h"

// 定义函数指针类型，这个函数没有参数，没有返回值
typedef void (*pFunction)(void);
#define APPLICATION_ADDRESS     (uint32_t)0x08004000  

// 创建一个函数指针变量
pFunction JumpToApplication;
uint32_t JumpAddress;

int main(void)
{
	// 初始化
	xuart_init();
	w25q64_init();
	boot_key_init();	// 这两个没问题
	xuart_printf("初始化完成\r\n");
	
	while (1)
	{
		if(GPIO_ReadInputDataBit(KEY_PORT, KEY_PIN) == KEY_ENABLE){
			Delay_s(1);
			if(GPIO_ReadInputDataBit(KEY_PORT, KEY_PIN) == KEY_ENABLE){
				xuart_printf("进入OTA升级模式\r\n");
				
				xfmc_erase_app();
				xuart_printf("已擦除用户空间，等待接收数据包\r\n");
				xm_init();
				xm_start();
			}
		}else{
			xuart_printf("正常运行模式\r\n");

			USART_DeInit(USART1);
			USART_Cmd(USART1,DISABLE);

			// 跳转到应用程序
			JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
			JumpToApplication = (pFunction) JumpAddress;
			__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
			JumpToApplication();
		}

	}
}