/*********************************************************************************************
 * ���ҵ��� www.DoYoung.net
 * ������Ӧ��������IoT�����壬�ɵ����ҵ��ӹ����ۿ���ѧ��Ƶ�����򡢲���������
*********************************************************************************************/

#include "esp8266.h"

uint8_t esp8266_send_cmd(char *cmd, char *ack, uint16_t waittime) //ESP8266����ָ��ײ㺯����
{
	uint8_t res = 0;
	USART3_RX_STA = 0;
	memset(USART3_RX_BUF,0,USART3_REC_LEN); //������3�Ļ���ռ���0
	WIFI_printf("%s\r\n", cmd); //����WIFIģ��ר�õķ��ͺ���
	if(waittime) //��Ҫ�ȴ�Ӧ��
	{
		while(--waittime) //�ȴ�����ʱ
		{
			HAL_Delay(10);//HAL����ʱ����
			if(USART3_RX_STA&0x8000) //���յ��ڴ���Ӧ����
			{
				if(esp8266_check_cmd((uint8_t *)ack))
				{
					printf("�ظ���Ϣ:%s\r\n",(uint8_t *)ack);//����Ӧ����Ϣ
					break; //�õ���Ч����
				}
				USART3_RX_STA=0; //����3��־λ��0
			} 
		}
		if(waittime==0)res=1;
	}
	return res;
}

uint8_t* esp8266_check_cmd(uint8_t *str) //ESP8266���ָ��ײ㺯����
{
	char *strx=0;
	if(USART3_RX_STA&0X8000) //���յ�һ��������
	{
		USART3_RX_BUF[USART3_RX_STA&0X7FFF] = 0; //��ӽ�����
		printf("%s\r\n",(char*)USART3_RX_BUF);
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (uint8_t*)strx;
}

uint8_t esp8266_quit_trans(void) //ESP8266�˳��жϣ��ײ㺯����
{
	while((USART3->SR&0X40)==0); //�ȴ����Ϳ�
	USART3->DR='+';      
	HAL_Delay(15); //���ڴ�����֡ʱ��(10ms)
	while((USART3->SR&0X40)==0); //�ȴ����Ϳ�
	USART3->DR='+';      
	HAL_Delay(15); //���ڴ�����֡ʱ��(10ms)
	while((USART3->SR&0X40)==0); //�ȴ����Ϳ�
	USART3->DR='+';      
	HAL_Delay(500); //�ȴ�500ms
	return esp8266_send_cmd("AT","OK",20); //�˳�͸���ж�
}

uint8_t esp8266_Connect_IOTServer(void) //ESP8266���ӵ�������ƽ̨������
{
//״̬���
	printf("׼������ģ��\r\n");
	HAL_Delay(100);
	esp8266_send_cmd("AT","OK",50);
	printf("׼���˳�͸��ģʽ\n");
	if(esp8266_quit_trans())
	{
		printf("�˳�͸��ģʽʧ�ܣ�׼������\r\n");
		return 6;
	}else printf("�˳�͸��ģʽ�ɹ�\r\n");
	
	printf("׼���رջ���\r\n");
	if(esp8266_send_cmd("ATE0","OK",50))
	{
		printf("�رջ���ʧ��׼������\r\n");
		return 1;
	}else printf("�رջ��Գɹ�\r\n");
	
	printf("��ѯģ���Ƿ�����\r\n");
	if(esp8266_send_cmd("AT","OK",50))
	{
		printf("ģ�鲻����׼������\r\n");
		return 1;
	}else printf("���ò�ѯ���߳ɹ�\r\n");
	
//����
	printf("׼������STAģʽ\r\n");
	if(esp8266_send_cmd("AT+CWMODE=1","OK",50))
	{
		printf("����STAģʽʧ��׼������\r\n");
		return 1;
	}else printf("����STAģʽ�ɹ�\r\n");
	
	printf("׼������\r\n");
	if(esp8266_send_cmd("AT+RST","OK",50))
	{
		printf("����ʧ�ܣ�׼������\r\n");
		return 2;
	}else printf("�����ɹ����ȴ�����\r\n");
	
	HAL_Delay(3000);//��ʱ�ȴ�WIFIģ�����

	printf("׼��ȡ���Զ�����\r\n");
	if(esp8266_send_cmd("AT+CWAUTOCONN=0","OK",50))
	{
		printf("ȡ���Զ�����ʧ�ܣ�׼������\r\n");
		return 3;
	}else printf("ȡ���Զ����ӳɹ�\r\n");
	
	printf("׼������·����\r\n");
	if(esp8266_Connect_AP())
	{
		printf("����·����ʧ�ܣ��ȴ�����\r\n");
		return 4;
	}else printf("����·�����ɹ�\r\n");
	
	HAL_Delay(4000);//��ʱ�ȴ�WIFIģ�����

	printf("׼������DHCP\r\n");
	if(esp8266_send_cmd("AT+CWDHCP=1,1","OK",100))
	{
		printf("����DHCPʧ�ܣ�׼������\r\n");
		return 7;
	}else printf("����DHCP�ɹ�\r\n");

//TCP����
	printf("����Ϊ�رն�·����\r\n");
	if(esp8266_send_cmd("AT+CIPMUX=0","OK",100))
	{
		printf("�رն�·����ʧ�ܣ�׼������\r\n");
		return 7;
	}else printf("���ùرն�·���ӳɹ�\r\n");

	printf("׼�����ӷ�����\r\n");
	if(esp8266_Connect_Server())
	{
		printf("���ӷ�����ʧ�ܣ��ȴ�����\r\n");
		return 8;
	}else printf("���ӷ������ɹ�\r\n");
	
	printf("׼���˳�͸��ģʽ\n");
	if(esp8266_quit_trans())
	{
		printf("�˳�͸��ģʽʧ�ܣ�׼������\r\n");
		return 6;
	}else printf("�˳�͸��ģʽ�ɹ�\r\n");
	
	printf("����Ϊ͸��ģʽ\r\n");
	if(esp8266_send_cmd("AT+CIPMODE=1","OK",50))
	{
		printf("����͸��ʧ�ܣ�׼������\r\n");
		return 6;
	}else printf("����͸���ɹ�\r\n");

//��������
	printf("���ÿ���͸��ģʽ\r\n");
	if(esp8266_send_cmd("AT+CIPSEND","OK",1000))
	{
		printf("����͸��ʧ�ܣ�׼������\r\n");
		return 9;
	}else printf("����͸���ɹ�\r\n");
	
	USART3_RX_STA = 0;
	memset(USART3_RX_BUF,0,USART3_REC_LEN);
	
	return 0; //һ��˳������0
}

uint8_t esp8266_Connect_AP() //ESP8266����AP�豸������·������
{
	uint8_t i=10;
	char *p = (char*)malloc(50);//����洢�ռ��ָ��

	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",SSID,PASS);//��������ATָ��
	while(esp8266_send_cmd(p,"WIFI GOT IP",1000) && i)//ѭ���жϵȴ�����AP�Ľ��
	{
		printf("����APʧ�ܣ�������������\r\n"); //����ʧ�ܵķ�����Ϣ
		i--;
	}
	free(p);//�ͷŷ���Ŀռ��ָ��
	if(i) return 0;//ִ�гɹ�����0
	else return 1;//ִ��ʧ�ܷ���1
}

uint8_t esp8266_Connect_Server() //ESP8266���ӵ�������
{
	uint8_t i=10;
	char *p = (char*)malloc(50);//����洢�ռ��ָ��
	sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",\%s",IOT_DOMAIN_NAME,IOT_PORTNUM);
	while(esp8266_send_cmd(p,"CONNECT",1000) && i)
	{
		printf("���ӷ�����ʧ�ܣ�������������\r\n");
		i--;
	}
	free(p);//�ͷŷ���Ŀռ��ָ��
	if(i)return 0;//ִ�гɹ�����0
	else return 1;//ִ��ʧ�ܷ���1
}

/*********************************************************************************************
 * ���ҵ��� www.DoYoung.net
 * ������Ӧ��������IoT�����壬�ɵ����ҵ��ӹ���������������
*********************************************************************************************/


