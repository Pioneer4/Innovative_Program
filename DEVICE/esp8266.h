/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : esp826.h
*Auther         : ����
*Version        : v1.0
*Date           : 2017-04-19
*Description    : ���ļ�������esp8266.c�����еĺ���ԭ��
*Function List  : 
*************************************************************/

#ifndef __ESP8266_H
#define	__ESP8266_H


#include "myinclude.h"

//#define API_KEY     "EPUF6JFF7TPW1B4Zg8M69Axgk1Q= "		//��Ҫ����Ϊ�û��Լ��Ĳ���
//#define DEV_ID      "4602551"							//��Ҫ����Ϊ�û��Լ��Ĳ���

#define API_KEY     "H=04D5y3SoQ05yoGnQ5V27M2T7Y= "		//��Ҫ����Ϊ�û��Լ��Ĳ���
#define DEV_ID      "17723965"							//��Ҫ����Ϊ�û��Լ��Ĳ���

#define RX_BUF_MAX_LEN     2048 
#define WIFI_MAX_DATA_LEN  600

#define AT_SEND_OK 		   1
#define AT_SEND_ERROR 	   0


#define WIFI_INIT_FAIL	   0
#define WIFI_CONNECT 	   1
#define WIFI_NOT_CONNECT   2

#define TCP_CONNECT 	   1
#define TCP_NOT_CONNECT    2

#define POWER_ON           1
#define POWER_OFF          0

#define REV_AT 		       1
#define	REV_DATA	       0

//����WIFI���ݽṹ��
typedef struct Gprs_Data_St
{
	u8 	Wifi_Data_Buffer[WIFI_MAX_DATA_LEN];
	u16	Wifi_Data_Len;
	u8	Wifi_Data_Rev_Finish;
}WIFI_Data;

u8   ESP8266_Init(void);
void ESP8266_Power_Pin_Init(void);
void ESP8266_Power_Ctrl(u8 powerCtrl);

u8	 SendCmd(char *cmd, char *reply1, char *reply2, char *ret_value, u32 waittime);

void ESP8266_Con_Udp(u8 wifiConnectState);



#endif 
