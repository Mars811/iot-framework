/*********************************************************************************************
 * 洋桃电子 www.DoYoung.net
 * 本程序应用于洋桃IoT开发板，可到洋桃电子官网观看教学视频、购买、并下载资料
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
	printf("开始连接云端服务器\r\n");
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;//配置部分可变头部的值
	buflen = sizeof(buf);
	memset(buf,0,buflen);
	memset(ClientID,0,128);//客户端ID的缓冲区全部清零
	sprintf(ClientID,"%s|securemode=3,signmethod=hmacsha1|",DEVICENAME);//构建客户端ID，并存入缓冲区
	memset(Username,0,128);//用户名的缓冲区全部清零
	sprintf(Username,"%s&%s",DEVICENAME,PRODUCTKEY);//构建用户名，并存入缓冲区

	Username_len = strlen(Username);

	memset(temp,0,128);//临时缓冲区全部清零
	sprintf(temp,"clientId%sdeviceName%sproductKey%s",DEVICENAME,DEVICENAME,PRODUCTKEY);//构建加密时的明文

	//以DeviceSecret为秘钥对temp中的明文，进行hmacsha1加密，结果就是密码，并保存到缓冲区中
	//好像还真不是。。。
	//utils_hmac_sha1(temp,strlen(temp),Password,DEVICESECRE,DEVICESECRE_LEN);

	//MQTT配置
	strcpy(ClientID, MQTT_CLIENTID);
	strcpy(Username, MQTT_USERNAME);
	strcpy(Password, MQTT_PASSWORD);
	Password_len = strlen(Password);//计算用户名的长度

	printf("ClientId:%s\r\n",ClientID);
	printf("Username:%s\r\n",Username);
	printf("Password:%s\r\n",Password);

	//【重要参数设置】可修改版本号、保活时间
	data.MQTTVersion = MQTTVERSION; //MQTT协议版本号
	data.clientID.cstring = ClientID; //客户端标识，用于区分每个客户端xxx为自定义，后面为固定格式
	data.keepAliveInterval = KEEPALIVEINTERVAL; //保活计时器，定义了服务器收到客户端消息的最大时间间隔,单位是秒
	data.cleansession = 1; //该标志置1服务器必须丢弃之前保持的客户端的信息，将该连接视为“不存在”
	data.username.cstring = Username; //用户名 DeviceName&ProductKey
	data.password.cstring = Password; //密码，工具生成
	
	len = MQTTSerialize_connect(buf, buflen, &data);//构造连接的报文
	transport_sendPacketBuffer(0,buf, len);//发送连接请求

	unsigned char sessionPresent, connack_rc;
	a=0;
	while(MQTTPacket_read(buf, buflen, transport_getdata) != CONNACK || a>1000)//等待胳回复
	{
		HAL_Delay(10);//必要的延时等待
		a++;//超时计数加1
	}
	if(a>1000)NVIC_SystemReset();//当计数超时，则复位单片机

	while(MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0);
	if(connack_rc != 0)
	{
		printf("连接回复:%uc\r\n",connack_rc);
	}
	printf("连接成功!\r\n");
	return 0;//执行成功返回0
}

void IOT_ping(void)//发送心跳包PING（保持与云服务器的连接）
{
	uint32_t len;
	len = MQTTSerialize_pingreq(buf, buflen); //计算数据长度
	transport_sendPacketBuffer(0, buf, len); //发送数据
	HAL_Delay(200);//必要的延时等待
	printf("发送心跳包Ping... ");
}

uint8_t IOT_subscribe(void)//subscribe主题订阅（订阅成功后才能接收订阅消息）
{
	uint32_t len;
	int req_qos = TOPIC_QOS;
	MQTTString topicString = MQTTString_initializer;//定义Topic结构体并初始化
	topicString.cstring = TOPIC_SUBSCRIBE;
	len = MQTTSerialize_subscribe(buf, buflen, 0, MSGID, 1, &topicString, &req_qos);//订阅发送数据编码
	transport_sendPacketBuffer(0, buf, len);
	HAL_Delay(100);//必要的延时等待
	if(MQTTPacket_read(buf, buflen, transport_getdata) == SUBACK) //等待订阅回复
	{
		unsigned short submsgid;
		int subcount;
		int granted_qos;
		MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);//回复的订阅确认数据解码
		if(granted_qos != 0) //qos不为0表示订阅成功
		{
			printf("订阅成功 GrantedQoS=%d\r\n", granted_qos);
			return 0; //订阅成功
		}
	}
	printf("订阅失败\r\n");
	return 1; //订阅失败
}

uint8_t IOT_publish(char* payload)//publish主题发布（参数是发布信息内容，用双引号包含）
{
	uint32_t len;
	MQTTString topicString = MQTTString_initializer;//定义Topic结构体并初始化
	topicString.cstring = TOPIC_PUBLISH;
	int payloadlen = strlen(payload);//用函数计算发布信息内容的长度
	printf("发布信息：%.*s\r\n", payloadlen, payload);
	//将要发送的信息payload通过MQTTSerialize_publish编码后用transport_sendPacketBuffer发送给云服务器
	len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString,
								(unsigned char*)payload, payloadlen);//发布数据编码
	transport_sendPacketBuffer(0, buf, len); //发送编码好的最终数据
	HAL_Delay(100);//必要的延时等待
	return 1;
}

/*********************************************************************************************
 * 洋桃电子 www.DoYoung.net
 * 本程序应用于洋桃IoT开发板，可到洋桃电子官网购买并下载资料
*********************************************************************************************/
