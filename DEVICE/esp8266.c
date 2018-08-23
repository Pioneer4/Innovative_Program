/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : esp8266.c
*Auther         : ����
*Version        : V1.0
*Date           : 2017-04-19
*Description    : ���esp8266����ز���
*Function List  : 
*Histoty		:
1. Date: 2017-04-15    Auther: ����
   Modifycation: ���uCOS-II���ݳ���
*************************************************************/

#include "myinclude.h"

#if SYSTEM_SUPPORT_OS
#include "includes.h"     //ucos ʹ��	  
#endif

#define SEND_CMD_WAIT_TIME  1000   //OS_TICKS_PER_SEC*5
#define MAX_RET_VALUE_LEN   50
WIFI_Data  Wifi_Data_Buf;

/*
���������־
��������������״̬��1
�����������      ��0
*/
u8	startFlag;        

extern OS_EVENT *semWifi;
extern OS_EVENT *semWifiData;
extern u8 wifiConnectState;  //��־��WIFI�����Ƿ�ɹ�
extern u8 TCP_ConnectState;  //��־��TCP�����Ƿ�ɹ�

/*************************************************************
*Function Name  : ESP8266_Init
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-20
*Description    : ���Esp8266ģ���ʼ��
				  Rx   : PB10
				  Tx   : PB11
				  CH_PD: PB12
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
u8 ESP8266_Init(void)
{
	u8 errESP8266Init;
	startFlag = 1;
	
	ESP8266_Power_Pin_Init();      //��ʼ�����ڵ���״̬
	UART3_Init(115200);
	ESP8266_Power_Ctrl(POWER_ON);  //ģ���ϵ�
	
	OSSemPend(semWifi, OS_TICKS_PER_SEC*10, &errESP8266Init);
	
	/*********�ж��Ƿ��Ѿ�����WIFI*********/
	if (errESP8266Init == OS_ERR_NONE)
	{
		startFlag = 0;
		
		Wifi_Data_Buf.Wifi_Data_Buffer[Wifi_Data_Buf.Wifi_Data_Len] = '\0';
		Wifi_Data_Buf.Wifi_Data_Rev_Finish = 0;
		//XFS_SendPkt("�������ӳɹ�");
		OSTimeDly(OS_TICKS_PER_SEC*2);       //2017-05-04
		return WIFI_CONNECT;
	}
	else
	{
		startFlag = 0;
		return WIFI_INIT_FAIL;
	}
}


/*************************************************************
*Function Name  : ESP8266_Power_Pin_Init
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-20
*Description    : ���ESP8266ģ��Ƭѡ���ƽ�CH_PD�ĳ�ʼ�� PC0
                  ��ʼ����Esp8266ģ�鴦�ڵ���״̬
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void ESP8266_Power_Pin_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOC,GPIO_Pin_0);
}


/*************************************************************
*Function Name  : ESP8266_Power_Ctrl
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-20
*Description    : ����Ƭѡ��CH_PD 
*Input          ��1. POWER_ON  �ϵ�
                  2. POWER_OFF ����
*Output         ��
*Return         ��
*************************************************************/
void ESP8266_Power_Ctrl(u8 powerCtrl)
{
	if(POWER_ON == powerCtrl)
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_0);
	}
	else
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	}
}


/*************************************************************
*Function Name  : ESP8266_Con_Udp
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-22
*Description    : ESP8266��������� 
*Input          ��wifiConnectState��
			      @arg: WIFI_CONNECT      ���ӳɹ��󣬽�����������Ӳ���
				  @arg: WIFI_NOT_CONNECT  ����ʧ�ܺ����Ƚ���wifi���ӣ�Ȼ��������������
*Output         ��
*Return         ��
*************************************************************/
void ESP8266_Con_Udp(u8 wifiNowState)
{
	u8 errESP8266_Con_Udp;
	char retValue[MAX_RET_VALUE_LEN] = {0};
	
	/***���ڳ�ʼ��ʱ����WIFI(�ȵ�)ʧ�ܣ����������·���ATָ�Ϊ����WIFI(�ȵ�)��׼��***/
	if (wifiNowState == WIFI_INIT_FAIL)
	{
		SendCmd("AT+RST", "OK", NULL, retValue, SEND_CMD_WAIT_TIME); //��λ
		OSTimeDly(OS_TICKS_PER_SEC*10);                              //�ȴ�10s,�����������ݰ�
		OSSemSet (semWifi, 0, &errESP8266_Con_Udp);                  //����������ݰ����ź���
	}
	
	SendCmd("ATE0", "OK", NULL, retValue, SEND_CMD_WAIT_TIME);
	SendCmd("AT", "OK", NULL, retValue, SEND_CMD_WAIT_TIME);
	
	/***���ڳ�ʼ��ʱ����WIFI(�ȵ�)ʧ�ܣ���������������WIFI(�ȵ�)��Ϊ���ӷ�������׼��***/
	if (wifiNowState == WIFI_INIT_FAIL)
	{
		SendCmd("AT+CWMODE=1", "OK", NULL,retValue, SEND_CMD_WAIT_TIME);
		if (AT_SEND_OK == SendCmd("AT+CWJAP=\"Pioneer's iPhone\",\"zhangqin\"", "CONNECTED", NULL, retValue, SEND_CMD_WAIT_TIME*2))  //10s
		{
			wifiConnectState = WIFI_CONNECT;
		}
		else
		{
			wifiConnectState = WIFI_NOT_CONNECT;
		}
		OSTimeDly(OS_TICKS_PER_SEC*10);                              //�ȴ�10s,�����������ݰ�
		OSSemSet (semWifi, 0, &errESP8266_Con_Udp);                  //����������ݰ����ź���
	}
	
	SendCmd("AT+CIPMUX=0", "OK", NULL,retValue, SEND_CMD_WAIT_TIME);
	SendCmd("AT+CIPMODE=1", "OK", NULL,retValue, SEND_CMD_WAIT_TIME);
	
	//if(AT_SEND_ERROR == SendCmd("AT+CIPSTART=\"TCP\",\"192.168.0.109\",8080", "CONNECT", NULL, retValue, SEND_CMD_WAIT_TIME))
	if(AT_SEND_ERROR == SendCmd("AT+CIPSTART=\"TCP\",\"183.230.40.33\",80", "CONNECT", NULL, retValue, SEND_CMD_WAIT_TIME))
	{
		TCP_ConnectState = TCP_NOT_CONNECT;
		LCD_ShowString(20,102,160,16,16,(u8*)"TCP NOT CONNECT!");
		//XFS_SendPkt("����������ʧ��");
	}
	else
	{
		TCP_ConnectState = TCP_CONNECT;
		XFS_SendPkt("���������ӳɹ�");
	}
	SendCmd("AT+CIPSEND", "OK", NULL,retValue, SEND_CMD_WAIT_TIME);
	OSTimeDly(OS_TICKS_PER_SEC);
	memset(&Wifi_Data_Buf, 0, sizeof(Wifi_Data_Buf));  //��սṹ��Wifi_Data_Buf
	OSSemPost(semWifiData);                            //�����ź��� ��ʼ�ϴ�����
}


/*************************************************************
*Function Name  : SendCmd
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-22
*Description    : ����һ��ATָ��
*Input          ��1. *cmd        ATָ��
                  2. *reply1     ����ֵ1
                  3. *reply2     ����ֵ2
                  4. *ret_value  ���ݽ��տ�����
                  5. waittime    ���ȴ�Ӧ��ʱ��
*Output         ��
*Return         ��
*************************************************************/
u8	SendCmd(char *cmd, char *reply1, char *reply2, char *ret_value, u32 waittime)
{
	u8 errSendCmd;
	memset(&Wifi_Data_Buf, 0, sizeof(Wifi_Data_Buf));  //��սṹ��Wifi_Data_Buf
	
	USART3_printf( USART3, "%s\r\n", cmd );            //����ATָ��
	
	/***����ҪACK����***/
	if ( (reply1==NULL) && (reply2==NULL) )            
    {
		return AT_SEND_OK;
    }
	
	/***��ҪACK����***/
	if(reply1 != NULL)
	{
		OSSemPend(semWifi, waittime, &errSendCmd);     //�ȴ������ź� ������ִ������
		if (errSendCmd == OS_ERR_NONE)
		{
			Wifi_Data_Buf.Wifi_Data_Rev_Finish = 0;
		}
		else
		{
			return AT_SEND_ERROR;
		}
	}
	/******************��ʼ�ж�ACK�Ƿ���ȷ******************/
	
	/***��Ҫ2��ACK����***/
	if ( (reply1 != NULL) && (reply2 != NULL) )
	{
		if (strstr( (char *)Wifi_Data_Buf.Wifi_Data_Buffer, reply1 )!=NULL && 
			strstr( (char *)Wifi_Data_Buf.Wifi_Data_Buffer, reply2 )!=NULL ) 
		{
			return AT_SEND_OK;
		}
		else
		{
			return AT_SEND_ERROR;
		}
	}
	
	/***��Ҫ��1��ACK����***/
	else if ( reply1 != NULL )
	{
		if (strstr( (char *)Wifi_Data_Buf.Wifi_Data_Buffer, reply1)!=NULL) 
		{
			return AT_SEND_OK;
		}
		else
		{
			return AT_SEND_ERROR;
		}
	}
	
	/***��Ҫ�ڶ�2��ACK����***/
	else
	{
		if (strstr( (char *)Wifi_Data_Buf.Wifi_Data_Buffer, reply2)!=NULL) 
		{
			return AT_SEND_OK;
		}
		else
		{
			return AT_SEND_ERROR;
		}
	}
}
