/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : dht11.h
*Auther         : ����
*Version        : v1.0
*Date           : 2016-05-03
*Description    : ���ļ�������dht11.c�����еĺ���ԭ��
*Function List  : 
*************************************************************/
#ifndef __DHT11_H
#define __DHT11_H 

#include "myinclude.h"

//IO��������
#define DHT11_IO_IN()  {GPIOE->MODER &= ~(3<<(2*2)); GPIOE->MODER |= 0<<2*2;}//PE2����ģʽ
#define DHT11_IO_OUT() {GPIOE->MODER &= ~(3<<(2*2)); GPIOE->MODER |= 1<<2*2;}//PE2���ģʽ 
//IO��������											   
#define	DHT11_DQ_OUT PEout(2) //���ݶ˿�	PE2 
#define	DHT11_DQ_IN  PEin(2)  //���ݶ˿�	PE2 

void DHT11_Init(void);                  //��ʼ��DHT11
void DHT11_Rst(void);                 //��λDHT11
u8 DHT11_Check(void);                 //����Ƿ����DHT11
u8 DHT11_Read_Bit(void);              //����һ��λ
u8 DHT11_Read_Byte(void);             //����һ���ֽ�
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//��ȡ��ʪ��

#endif
