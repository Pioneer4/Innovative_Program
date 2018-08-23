/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : XFS5152CE.c
*Auther         : ����
*Version        : v1.0
*Date           : 2017-05-03
*Description    : ��ʼ��UART4 �Լ�����ģ�����ݰ��Ĵ������
*Function List  : 
*************************************************************/

#include "myinclude.h"


/*************************************************************
*Function Name  : UART4_Init
*Auther         : ����
*Vertion        : v1.0
*Date           : 2016-08-25 
*Description    : ����uart4�ĳ�ʼ��
                  Tx: PC10    Rx: PC11
*Input          ��1. bound  ����ͨ�ŵĲ�����
*Output         ��
*Return         ��
*************************************************************/
void UART4_Init(u32 bound)
{
	GPIO_InitTypeDef GPIOC_InitStructure;
	USART_InitTypeDef UART4_InitStructure;
	
	/*******��ʼ��GPIOC��UART4��ʱ��************/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	
	/*****����GPIOC10��GPIOC11ΪUART4***********/
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);
	
	GPIOC_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIOC_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOC_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIOC_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIOC_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	
    GPIO_Init(GPIOC, &GPIOC_InitStructure);
	
	UART4_InitStructure.USART_BaudRate = bound;  //������
	UART4_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //��Ӳ��������
	UART4_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //���պͷ���ģʽ
	UART4_InitStructure.USART_Parity = USART_Parity_No;  //����żУ��
	UART4_InitStructure.USART_StopBits = USART_StopBits_1;  //1��ֹͣλ
	UART4_InitStructure.USART_WordLength = USART_WordLength_8b;  //8���ֳ�
	
	USART_Init(UART4, &UART4_InitStructure);  //UART1��ʼ��
	
	USART_Cmd(UART4, ENABLE);  //ʹ��UART4
}


/*************************************************************
*Function Name  : UART4_Send_Data
*Auther         : ����
*Version        : V1.0
*Date           : 2017-04-19
*Description    : UART3���ݷ���
*Input          ��1. *data Ҫ���͵����ݵ�ָ��
                  2. len   ���ݳ���
*Output         ��
*Return         ��
*************************************************************/
void UART4_Send_Data(u8 *data, u8 len)
{
	u8 i=0;
	
	while( USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET );
	
	for(i = 0; i < len; i++)
	{
		USART_SendData(UART4,	data[i]);
		while(RESET == USART_GetFlagStatus(UART4, USART_FLAG_TC));
	}
}

/*************************************************************
*Function Name  : XFS_SendPkt
*Auther         : ����
*Version        : v1.0
*Date           : 2017-05-03
*Description    : ������Ϣ���������
*Input          ��*XFSdata  Ҫ���͵�����������ָ��
*Output         ��
*Return         ��
*************************************************************/
void XFS_SendPkt(char *XFSdata)
{
	u8  dataBuf[50] = {0};
	u8 XFS_Length = 0;
	
	XFS_Length = strlen(XFSdata);
	
	/**************֡�̶�������Ϣ**************/           
	dataBuf[0] = 0xFD ; 		 //����֡ͷFD
	dataBuf[1] = 0x00 ; 		 //�������������ȵĸ��ֽ�
	dataBuf[2] = XFS_Length + 2; //�������������ȵĵ��ֽ�
	dataBuf[3] = 0x01 ; 		 //���������֣��ϳɲ�������		 		 
	dataBuf[4] = 0x00;           //�ı������ʽ��GB2312 
	
	memcpy(&dataBuf[5], XFSdata, XFS_Length);  //���ݴ��
	
	UART4_Send_Data(dataBuf, XFS_Length + 5);  //��������
}

