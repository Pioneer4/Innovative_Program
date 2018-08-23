/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : uast.c
*Auther         : ����
*Version        : v2.0  ����uCOS-II
*Date           : 2017-04-15
*Description    : ���ļ��������й�uart��ʼ�������Լ��ж�ʹ�ܵĺ�����
�����յ������ݱ��浽һ�����飬ʹ�ô��ڽ��յ�����ֽ���Ϊ2��14�η���1��
ͬʱͨ��Э������Ҫ����������Իس����з���\r\n������
*Function List  : 
*Histoty        :
1. Date: 2017-04-15    Auther: ����
   Modifycation: ���uCOS-II���ݳ���
*************************************************************/
#include "myinclude.h"

#if SYSTEM_SUPPORT_OS
#include "includes.h"  //ucos ʹ��	  
#endif

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif

u8 UART_RX_BUF[UART_REC_LEN];  //���ջ���

/*
����״̬
bit15 �������
bit14 ���յ�0x0d '\r'
bit13~0 ���յ�����Ч�ַ���Ŀ
*/
u16 USART_RX_STA = 0;  //����״̬���Ϊ0

/*************************************************************
*Function Name  : UART1_Init
*Auther         : ����
*Vertion        : v1.0
*Date           : 2016-08-25 
*Description    : ����uart1�ĳ�ʼ��
*Input          ��1. bound  ����ͨ�ŵĲ�����
*Output         ��
*Return         ��
*************************************************************/
void UART1_Init(u32 bound)
{
	GPIO_InitTypeDef GPIOA_InitStructure;
	USART_InitTypeDef UART1_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/*******��ʼ��GPIOA��USART1��ʱ��************/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	/*****����GPIOA9��GPIO10ΪUSART1***********/
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIOA_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOA_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIOA_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	
    GPIO_Init(GPIOA, &GPIOA_InitStructure);
	
	UART1_InitStructure.USART_BaudRate = bound;  //������
	UART1_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //��Ӳ��������
	UART1_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //���պͷ���ģʽ
	UART1_InitStructure.USART_Parity = USART_Parity_No;  //����żУ��
	UART1_InitStructure.USART_StopBits = USART_StopBits_1;  //1��ֹͣλ
	UART1_InitStructure.USART_WordLength = USART_WordLength_8b;  //8���ֳ�
	
	USART_Init(USART1, &UART1_InitStructure);  //UART1��ʼ��
	
	/*******UART1�Ľ����ж�����************/
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);  //ʹ��UART1
}

/*************************************************************
*Function Name  : USART1_IRQHandler
*Auther         : ����
*Vertion        : v2.0
*Date           : 2017-04-15
*Description    : USART1���жϷ������̣������˴���Э�飨�Իس����з�������
*Input          ��
*Output         ��
*Return         ��
*Histoty        :
1. Date: 2017-04-15    Auther: ����
   Modifycation: ���uCOS-II���ݳ���
*************************************************************/
void USART1_IRQHandler(void)
{
	u8 res;  //���ڽ�������
	
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif

	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //������ռĴ�����Ϊ��
	{
		res = USART_ReceiveData(USART1);  //��������
		
		if((USART_RX_STA & 0x8000) == 0)  //����δ���
		{
			/****����ѽ��յ�0x0d����һ�����жϱ��ξ��յ��Ƿ���0x0a******/            //  ('\r' 0x0d)  ('\n' 0x0a)
			if(USART_RX_STA & 0x4000)  //���յ�0x0d
			{
				if(res != 0x0a)  //�������0x0a����ʾ���մ������¿�ʼ
				{
					USART_RX_STA = 0;
				}
				else  //�����0x0a���������
				{
					USART_RX_STA |= 0x8000;
				}
			}
			
			/***���û���յ�0x0d����һ���жϽ��յ��Ƿ���0x0d***/
			else
			{
				if(res == 0x0d)
				{
					USART_RX_STA |= 0x4000;
				}
				else
				{
					UART_RX_BUF[USART_RX_STA & 0x3FFF] = res; //���մ���ڻ�����
					USART_RX_STA++;
					
					if((USART_RX_STA & 0x3FFF) > (UART_REC_LEN - 1))  //�������ݳ���������������������մ���  
					{
						USART_RX_STA = 0;
					}
				}
			}
		}
		
	}
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
}

/************************************************************/
//Function Name: Clear_Serial
//Auther: ����
//Vertion: v1.0
//Date: 2016-09-01
//Description: �ַ������
//Input��1.USART_RX_BUF[] Ҫ��յ��ַ���
//Output��
//Return��
/************************************************************/
void Clear_Serial(u8 USART_RX_BUF[])
{	
	u8 index = 0;
	
	while (USART_RX_BUF[index] != '\0')
	{
		USART_RX_BUF[index] = 0;
		index++;
	}
}

