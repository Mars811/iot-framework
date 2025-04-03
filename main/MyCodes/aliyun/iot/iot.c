/*********************************************************************************************
 * ���ҵ��� www.DoYoung.net
 * ������Ӧ��������IoT�����壬�ɵ����ҵ��ӹ����ۿ���ѧ��Ƶ�����򡢲���������
*********************************************************************************************/

#include "iot.h"

uint16_t buflen=200;
unsigned char buf[200];

char ClientID[128];
uint8_t ClientID_len;

char Username[128];
uint8_t Username_len;

char Password[128];
uint8_t Password_len;

uint8_t IOT_connect()
{
	uint16_t a;
	uint32_t len;
	char temp[128];
	printf("��ʼ�����ƶ˷�����\r\n");
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;//���ò��ֿɱ�ͷ����ֵ
	buflen = sizeof(buf);
	memset(buf,0,buflen);
	memset(ClientID,0,128);//�ͻ���ID�Ļ�����ȫ������
	sprintf(ClientID,"%s|securemode=3,signmethod=hmacsha1|",DEVICENAME);//�����ͻ���ID�������뻺����
	memset(Username,0,128);//�û����Ļ�����ȫ������
	sprintf(Username,"%s&%s",DEVICENAME,PRODUCTKEY);//�����û����������뻺����

	Username_len = strlen(Username);

	memset(temp,0,128);//��ʱ������ȫ������
	sprintf(temp,"clientId%sdeviceName%sproductKey%s",DEVICENAME,DEVICENAME,PRODUCTKEY);//��������ʱ������

	//��DeviceSecretΪ��Կ��temp�е����ģ�����hmacsha1���ܣ�����������룬�����浽��������
	//�����治�ǡ�����
	//utils_hmac_sha1(temp,strlen(temp),Password,DEVICESECRE,DEVICESECRE_LEN);

	//MQTT����
	strcpy(ClientID, MQTT_CLIENTID);
	strcpy(Username, MQTT_USERNAME);
	strcpy(Password, MQTT_PASSWORD);
	Password_len = strlen(Password);//�����û����ĳ���

	printf("ClientId:%s\r\n",ClientID);
	printf("Username:%s\r\n",Username);
	printf("Password:%s\r\n",Password);

	//����Ҫ�������á����޸İ汾�š�����ʱ��
	data.MQTTVersion = MQTTVERSION; //MQTTЭ��汾��
	data.clientID.cstring = ClientID; //�ͻ��˱�ʶ����������ÿ���ͻ���xxxΪ�Զ��壬����Ϊ�̶���ʽ
	data.keepAliveInterval = KEEPALIVEINTERVAL; //�����ʱ���������˷������յ��ͻ�����Ϣ�����ʱ����,��λ����
	data.cleansession = 1; //�ñ�־��1���������붪��֮ǰ���ֵĿͻ��˵���Ϣ������������Ϊ�������ڡ�
	data.username.cstring = Username; //�û��� DeviceName&ProductKey
	data.password.cstring = Password; //���룬��������
	
	len = MQTTSerialize_connect(buf, buflen, &data);//�������ӵı���
	transport_sendPacketBuffer(0,buf, len);//������������

	unsigned char sessionPresent, connack_rc;
	a=0;
	while(MQTTPacket_read(buf, buflen, transport_getdata) != CONNACK || a>1000)//�ȴ���ظ�
	{
		HAL_Delay(10);//��Ҫ����ʱ�ȴ�
		a++;//��ʱ������1
	}
	if(a>1000)NVIC_SystemReset();//��������ʱ����λ��Ƭ��

	while(MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0);
	if(connack_rc != 0)
	{
		printf("���ӻظ�:%uc\r\n",connack_rc);
	}
	printf("���ӳɹ�!\r\n");
	return 0;//ִ�гɹ�����0
}

void IOT_ping(void)//����������PING���������Ʒ����������ӣ�
{
	uint32_t len;
	len = MQTTSerialize_pingreq(buf, buflen); //�������ݳ���
	transport_sendPacketBuffer(0, buf, len); //��������
	HAL_Delay(200);//��Ҫ����ʱ�ȴ�
	printf("����������Ping... ");
}

uint8_t IOT_subscribe(void)//subscribe���ⶩ�ģ����ĳɹ�����ܽ��ն�����Ϣ��
{
	uint32_t len;
	int req_qos = TOPIC_QOS;
	MQTTString topicString = MQTTString_initializer;//����Topic�ṹ�岢��ʼ��
	topicString.cstring = TOPIC_SUBSCRIBE;
	len = MQTTSerialize_subscribe(buf, buflen, 0, MSGID, 1, &topicString, &req_qos);//���ķ������ݱ���
	transport_sendPacketBuffer(0, buf, len);
	HAL_Delay(100);//��Ҫ����ʱ�ȴ�
	if(MQTTPacket_read(buf, buflen, transport_getdata) == SUBACK) //�ȴ����Ļظ�
	{
		unsigned short submsgid;
		int subcount;
		int granted_qos;
		MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);//�ظ��Ķ���ȷ�����ݽ���
		if(granted_qos != 0) //qos��Ϊ0��ʾ���ĳɹ�
		{
			printf("���ĳɹ� GrantedQoS=%d\r\n", granted_qos);
			return 0; //���ĳɹ�
		}
	}
	printf("����ʧ��\r\n");
	return 1; //����ʧ��
}

uint8_t IOT_publish(char* payload)//publish���ⷢ���������Ƿ�����Ϣ���ݣ���˫���Ű�����
{
	uint32_t len;
	MQTTString topicString = MQTTString_initializer;//����Topic�ṹ�岢��ʼ��
	topicString.cstring = TOPIC_PUBLISH;
	int payloadlen = strlen(payload);//�ú������㷢����Ϣ���ݵĳ���
	printf("������Ϣ��%.*s\r\n", payloadlen, payload);
	//��Ҫ���͵���Ϣpayloadͨ��MQTTSerialize_publish�������transport_sendPacketBuffer���͸��Ʒ�����
	len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString,
								(unsigned char*)payload, payloadlen);//�������ݱ���
	transport_sendPacketBuffer(0, buf, len); //���ͱ���õ���������
	HAL_Delay(100);//��Ҫ����ʱ�ȴ�
	return 1;
}

/*********************************************************************************************
 * ���ҵ��� www.DoYoung.net
 * ������Ӧ��������IoT�����壬�ɵ����ҵ��ӹ���������������
*********************************************************************************************/
