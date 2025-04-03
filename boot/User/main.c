#include "stm32f10x.h"
#include "xfmc.h"
#include "xuart.h"
#include "w25q64.h"
#include "Delay.h"
#include "boot_key.h"
#include "xmodem.h"

// ���庯��ָ�����ͣ��������û�в�����û�з���ֵ
typedef void (*pFunction)(void);
#define APPLICATION_ADDRESS     (uint32_t)0x08004000  

// ����һ������ָ�����
pFunction JumpToApplication;
uint32_t JumpAddress;

int main(void)
{
	// ��ʼ��
	xuart_init();
	w25q64_init();
	boot_key_init();	// ������û����
	xuart_printf("��ʼ�����\r\n");
	
	while (1)
	{
		if(GPIO_ReadInputDataBit(KEY_PORT, KEY_PIN) == KEY_ENABLE){
			Delay_s(1);
			if(GPIO_ReadInputDataBit(KEY_PORT, KEY_PIN) == KEY_ENABLE){
				xuart_printf("����OTA����ģʽ\r\n");
				
				xfmc_erase_app();
				xuart_printf("�Ѳ����û��ռ䣬�ȴ��������ݰ�\r\n");
				xm_init();
				xm_start();
			}
		}else{
			xuart_printf("��������ģʽ\r\n");

			USART_DeInit(USART1);
			USART_Cmd(USART1,DISABLE);

			// ��ת��Ӧ�ó���
			JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
			JumpToApplication = (pFunction) JumpAddress;
			__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
			JumpToApplication();
		}

	}
}