#include "aliyun_app.h"

void aliyun_main(void){
	uint16_t a=0,b=0;
	int t,qos,payloadinlen; //Ϊ���漴����������Ϣ�����������
	unsigned char dup,retained;
	unsigned short msgid;
	unsigned char* payloadin;
	MQTTString receiveTopic;

    //�ȴ�����������
    while(esp8266_Connect_IOTServer());//ATָ������TCP�����Ʒ�������IP�Ͷ˿ڲ�����esp8266.h�ļ����޸����ã�
    while(IOT_connect());//��MQTTЭ��+��Ԫ����Ϣ���Ӱ�����������ƽ̨����Ԫ�������iot.h�ļ����޸����ã�
    printf("�����Ʒ�����\r\n");
    HAL_Delay(100);//�ȴ�
    IOT_subscribe();//���ⶩ�ģ����ĳɹ�����ܽ��ն�����Ϣ��
    a=0xFFF0; //ǿ�Ʒ����������ļ�������������ط�������

    while(1){
        //�����ϴ�
        /*
        char temp[30];
        snprintf(temp, sizeof(temp), "temperature:%d", para_temperature);
        IOT_publish(temp);
        HAL_Delay(100);

        snprintf(temp, sizeof(temp), "humidity:%d", para_humidity);
        IOT_publish(temp);
        HAL_Delay(100);

        snprintf(temp, sizeof(temp), "lightness:%d", para_lightness);
        IOT_publish(temp);
        HAL_Delay(100);

        snprintf(temp, sizeof(temp), "temperature_th:%d", para_temperature_th);
        IOT_publish(temp);
        */

        // ������
        HAL_Delay(10);		//��ѭ���ļ����ʱ����ֹˢ�¹��죩
        a++;					//�����1
        if(a>1000){ 			//ÿ1000*10������ʱ����һ��Ping�����������������Ʒ����������ӣ�
              a=0;			//�����־��0
              IOT_ping();		//����Ping������
              if(MQTTPacket_read(buf, buflen, transport_getdata)==PINGRESP){		//�ж��������ǲ��ظ�ȷ��
                  printf("�����ɹ�\r\n"); 											//�ظ�0xD0��0x00ʱ��ʾ�����ɹ��Ļظ�
              }else {
                  printf("����ʧ��\r\n");//�޻ظ���ʾʧ��
                  a=0xFFF0; //ǿ�Ʒ����������ļ�������������ط�������
                  b++;//����������1
                  if(b>20) //������ٷ���������20�κ��޻ظ�����λWIFIģ����������
                  {
                      while(esp8266_Connect_IOTServer());//ATָ������TCP�����Ʒ�������IP�Ͷ˿ڲ�����esp8266.h�ļ����޸����ã�
                      while(IOT_connect());//��MQTTЭ��+��Ԫ����Ϣ���Ӱ�����������ƽ̨����Ԫ�������iot.h�ļ����޸����ã�
                      a=0;b=0;//�����־��0
                  }
              }
              USART3_RX_STA = 0;//����3���ձ�־λ��0
        }
  
        //�����ƶ˵Ķ�����Ϣ
        if(USART3_RX_STA&0x8000) //�ж��Ʒ�������������Ϣ
        {
            switch (USART3_RX_BUF[0]/16){//�жϽ��յ��ı�������
                  case PUBLISH:
                      t = MQTTDeserialize_publish( 				//�Խ��յ�MQTTԭʼ���ݽ��н��루����1��ʾ�ɹ�������ֵ��ʾ����
                        &dup, 								//���ó����ط���־λ��0�׷���1��ǰ���ĵ��ط���
                        &qos, 								//���ó������������ȼ���0���ַ�һ�Σ�1���ٷַ�һ�Σ�2ֻ�ַ�һ�Σ�
                        &retained, 							//���ó�������λ����
                        &msgid, 								//���ó�����ϢID
                        &receiveTopic, 						//���ó�������������
                        &payloadin, 							//���ó�����Ϣ����
                        &payloadinlen, 						//���ó�����Ϣ����
                        USART3_RX_BUF, USART3_RX_STA&0x7FFF	//�����롿ԭʼ���ݻ��棨����+������
                    );
  
                      //��ȷ��������
                      if(t){
                        printf("���յ�����:%.*s  ", receiveTopic.lenstring.len, receiveTopic.lenstring.data);//��ʾ��������
                        printf("��Ϣ����:%.*s  ", payloadinlen, payloadin);
                        printf("QoS:%d\r\n", qos);
                        USART3_RX_STA = 0;
  
                        //��������
                        if(strncmp(payloadin, "para_lightness:", 15) == 0) {
                            IOT_publish("para_lightness has been changed.");
                            printf("para_lightness has been changed.");
  
                        }
                        if(strncmp(payloadin, "para_temperature_th:", 20) == 0) {
                            IOT_publish("para_temperature_th has been changed.");
                            printf("para_temperature_th has been changed.");
                        }
                      }else{
                          printf("���ն�����Ϣʱ����\r\n");//���մ���ʱ����ʾ
                      }
                      break;
                  case CONNACK: //���ӱ���ȷ��
                  //�������Ĵ������Ҳ�ɿ��ã�
                      break;
                  case SUBACK: //����������ȷ��
                  //�������Ĵ������Ҳ�ɿ��ã�
                      break;
                  case UNSUBACK: //ȡ�����ı���ȷ��
                  //�������Ĵ������Ҳ�ɿ��ã�
                      break;
                  default:
                  //�������
                      break;
            }
            USART3_RX_STA = 0;//����3���ձ�־λ��0
        }	
    }
}