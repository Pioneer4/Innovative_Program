/************************************************************/
//     Open source but not allowed to modify
//File name: 
//Auther: ����
//Version: v1.0
//Date: 2016-08-25
//Description: ���ļ�������uart.c�е����к���ԭ��
//Function List: 
//
/************************************************************/
#ifndef  __UART_H
#define  __UART_H

#include "stm32f4xx.h"
#include "stdio.h"

#define UART_REC_LEN  200  //�����������ֽ��� 200

void UART1_Init(u32 bound);
void Clear_Serial(u8 USART_RX_BUF[]);  


#endif  //__UART_H


