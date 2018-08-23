/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : key.h
*Auther         : ����
*Version        : V2.0
*Date           : 2017-04-16
*Description    : ���ļ������� key.c�����еĺ���ԭ��
*Function List  : 
*Histoty		:
1. Date: 2017-04-15    Auther: ����
   Modifycation: ���uCOS-II���ݳ���
*************************************************************/

#ifndef __KEY_H
#define __KEY_H	 
#include "myinclude.h" 

#define GET_KEY0_STATUS() PEin(4)
#define KEY0_ON   0
#define KEYO_OFF  1

/*����ķ�ʽ��ͨ��ֱ�Ӳ����⺯����ʽ��ȡIO*/
#define KEY0 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) //PE4
#define KEY1 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)	//PE3 
#define KEY2 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2) //PE2
#define WK_UP 	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)	//PA0

void KeyInit(void);

#endif
