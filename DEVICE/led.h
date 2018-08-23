/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : led.c
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-16
*Description    : ���ļ�������led.c�����еĺ���ԭ��
*Function List  : 
*************************************************************/
#ifndef __LED_H
#define __LED_H

#include "myinclude.h"

//LED�˿ڶ���
#define LED2 PAout(6)
#define LED3 PAout(7)	 

#define LED_STATUS_ON   0
#define LED_STATUS_OFF  1
#define GET_LED2_STATUS() PAin(6)

#define ON  Bit_RESET
#define OFF Bit_SET

void LedInit(void);  //��ʼ��		
void Led2Switch(BitAction State);
void Led3Switch(BitAction State);

#endif
