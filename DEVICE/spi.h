/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : spi.h
*Auther         : ����
*Version        : v1.0
*Date           : 2016-09-22
*Description    : ���ļ��������й�spi.c�е����к�����ԭ��
*Function List  : 
*************************************************************/
#ifndef __SPI_H
#define __SPI_H

#include "stm32f4xx.h"

 	    													  
void SPI1_Init(void);			              //��ʼ��SPI1��
u8 SPI1_ReadWriteByte(u8 TxData);             //SPI1���߶�дһ���ֽ�
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler); //����SPI1�ٶ�   

#endif

