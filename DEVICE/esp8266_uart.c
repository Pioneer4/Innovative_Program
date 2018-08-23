/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : esp8266_uart.c
*Auther         : ����
*Version        : V1.0
*Date           : 2017-04-19
*Description    : ���ļ��԰����������жϽ����˳�ʼ��
*Function List  : 
*Histoty		:
1. Date: 2017-04-15    Auther: ����
   Modifycation: ���uCOS-II���ݳ���
*************************************************************/

#include "myinclude.h"

#if SYSTEM_SUPPORT_OS
#include "includes.h"  //ucos ʹ��	  
#endif

extern WIFI_Data  Wifi_Data_Buf;
extern u8 startFlag;
extern OS_EVENT *semWifi;
extern OS_EVENT *semWifiData;

/*************************************************************
*Function Name  : UART3_Init
*Auther         : ����
*Version        : V2.0
*Date           : 2017-04-16
*Description    : ����uart3�ĳ�ʼ��
*Input          ��1. bound  ����ͨ�ŵĲ�����
*Output         ��
*Return         ��
*************************************************************/
void UART3_Init(u32 bound)
{
	GPIO_InitTypeDef GPIOA_InitStructure;
	USART_InitTypeDef UART3_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/*******��ʼ��GPIOB��USART3��ʱ��************/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	/*****����GPIOB10��GPIOB11ΪUSART3***********/
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART1);       //PB10:Tx
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART1);       //PB11:Rx
	
	GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIOA_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOA_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIOA_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	
    GPIO_Init(GPIOB, &GPIOA_InitStructure);
	
	UART3_InitStructure.USART_BaudRate = bound;                      //������
	UART3_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //��Ӳ��������
	UART3_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //���պͷ���ģʽ
	UART3_InitStructure.USART_Parity = USART_Parity_No;              //����żУ��
	UART3_InitStructure.USART_StopBits = USART_StopBits_1;           //1��ֹͣλ
	UART3_InitStructure.USART_WordLength = USART_WordLength_8b;      //8���ֳ�
	
	USART_Init(USART3, &UART3_InitStructure);                        //UART1��ʼ��
	
	/*******UART3�Ľ����ж�����************/
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	/******* ʹ�ܴ���3���߿����ж� *******/
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART3, ENABLE);  //ʹ��UART3
	
	USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	USART_ClearITPendingBit(USART3, USART_IT_IDLE);
}

#if 0
/*************************************************************
*Function Name  : itoa
*Auther         : ����
*Version        : V1.0
*Date           : 2017-04-19
*Description    : ����������ת�����ַ���
*Input          ��1. value   Ҫת����������
                  2. *string ת������ַ���ָ��
                  3. radix   ��ʾ10���ƣ��������Ϊ0
*Output         ��ת���ַ����洢�����׵�ַ
*Return         ��
*************************************************************/
static char *itoa( int value, char *string, int radix )
{
	int     i, d;
    int     flag = 0;
    char    *ptr = string;
	
	/* This implementation only works for decimal numbers. */
	if (radix != 10)
	{
		*ptr = 0;
		return string;
	}
	
	if (!value)
	{
		*ptr++ = 0x30;
		*ptr = 0;
		return string;
	}
	
	/* if this is a negative value insert the minus sign. */
	if (value < 0)
	{
		*ptr++ = '-';
		
		/* Make the value positive. */
        value *= -1;
	}
	
	for (i=10000; i>0; i/=10)
	{
		d = value / i;
		
		if (d || flag)
		{
			*(ptr++) = (char)(d + 0x30);  //0x30 = '0'
			value -= (d * i);
			flag = 1;
		}
	}
	
	*ptr = 0;
	
	return string;
}
#endif

/*************************************************************
*Function Name  : USART3_printf
*Auther         : ����
*Version        : V1.0
*Date           : 2017-04-19
*Description    : ��ʽ�������������C���е�printf��������û���õ�C��
                  �ܹ���� "\r" "\n"   "%d"  "%s" �Ľ���
*Input          ��1. *USARTx ����ͨ��
                  2. *Data   Ҫ���͵����ڵ����ݵ�ָ��
                  3. ...     ��������
*Output         ��
*Return         ��
*************************************************************/
void USART3_printf( USART_TypeDef* USARTx, char *Data, ... )
{
#if 0
	const char *s;
	int d;
#endif
	
	char buf[128];
	char *ptr =buf;

	va_list ap;
	va_start(ap, Data);
	
#if 1
	vsprintf(buf, Data, ap);
	va_end(ap);
	
	while (*ptr != '\0')
	{
		USART_SendData(USARTx, *ptr++);
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
	}
#endif
	
#if 0
	while (*Data != 0)  //�ж��Ƿ�ﵽ�ַ���������
	{
		if (*Data == 0x5c)                          // '\'
		{
			switch (*++Data)
			{
				case 'r':                              
					USART_SendData(USARTx, 0x0d);   //�س���
				    Data++;
				    break;
				
				case 'n':                           //���з�
					USART_SendData(USARTx, 0x0a);
				    Data++;
				    break;
				
				default:
					Data++;
				    break;
			}
		}
		
		else if (*Data == '%')
		{
			switch (*++Data)
			{
				case 's':                           //�ַ���   
					s = va_arg(ap, const char*);
                    for ( ; *s; s++)
                    {
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
					}				
				    Data++;
				    break;
				
				case 'd':                           //ʮ����
					d = va_arg(ap, int);
				    itoa(d, buf, 10);
			     	for (s = buf; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
					}
				    Data++;
				    break;
				
				default:
					Data++;
				    break;
			}
		}
		
		else
		{
			USART_SendData(USARTx, *Data++);
		}
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
	}
#endif
}

/*************************************************************
*Function Name  : USART3_Send_Data
*Auther         : ����
*Version        : V1.0
*Date           : 2017-04-19
*Description    : UART3���ݷ���
*Input          ��1. *data Ҫ���͵����ݵ�ָ��
                  2. len   ���ݳ���
*Output         ��
*Return         ��
*************************************************************/
void USART3_Send_Data(u8 *data, u8 len)
{
	u8 i=0;
	
	while( USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET );
	
	for(i = 0; i < len; i++)
	{
		USART_SendData(USART3,	data[i]);
		while(RESET == USART_GetFlagStatus(USART3, USART_FLAG_TC));
	}
}

/*************************************************************
*Function Name  : USART3_IRQHandler
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-15
*Description    : USART3���жϷ������̣������˴���Э�飨�Իس����з�������
*Input          ��
*Output         ��
*Return         ��
*Histoty        :
1. Date: 2017-04-15    Auther: ����
   Modifycation: ���uCOS-II���ݳ���
*************************************************************/
void USART3_IRQHandler(void)
{
	char ch;
	
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)     //������ռĴ�����Ϊ��
	{
		ch  = USART_ReceiveData( USART3 );
		
		if (Wifi_Data_Buf.Wifi_Data_Len < ( RX_BUF_MAX_LEN-1) )//Ԥ��һ���ֽ�д������
		{
			Wifi_Data_Buf.Wifi_Data_Buffer[Wifi_Data_Buf.Wifi_Data_Len++] = ch;
		}
	}
	
	if ( USART_GetITStatus(USART3, USART_IT_IDLE) == SET )     //����֡�������
	{
		if (Wifi_Data_Buf.Wifi_Data_Buffer[0] != 0xff)         //�����ף�0xff�����Ǻ��ֿ�������ַ�
		{
			/***����������״̬��***/
			if(startFlag == 0)
			{
				if (Wifi_Data_Buf.Wifi_Data_Buffer[0] == 'H')  //����Ƿ������ش����ݰ� 
				{
					OSSemPost(semWifiData);                    //���� ���ݰ��ź���
				}
				else
				{
					OSSemPost(semWifi);                        //���� ATָ���ź���
				} 
			}
			/***��������״̬��***/
			else
			{
				if (Wifi_Data_Buf.Wifi_Data_Buffer[Wifi_Data_Buf.Wifi_Data_Len-3]=='P')//���ӳɹ���WIFI GOT IP��
				{
					OSSemPost(semWifi);                        //�����ź���
				}
			}
		}
		
		Wifi_Data_Buf.Wifi_Data_Rev_Finish = 1;
		

		USART_ReceiveData(USART3); //��DR  �������������жϱ�־λ
		                           //(�ȶ�USART_SR��Ȼ���USART_DR)
	}
	
#if SYSTEM_SUPPORT_OS 	           //���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
}
