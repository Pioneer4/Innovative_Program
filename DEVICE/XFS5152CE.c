/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : XFS5152CE.c
*Auther         : 张沁
*Version        : v1.0
*Date           : 2017-05-03
*Description    : 初始化UART4 以及语音模块数据包的打包函数
*Function List  : 
*************************************************************/

#include "myinclude.h"


/*************************************************************
*Function Name  : UART4_Init
*Auther         : 张沁
*Vertion        : v1.0
*Date           : 2016-08-25 
*Description    : 用于uart4的初始化
                  Tx: PC10    Rx: PC11
*Input          ：1. bound  串口通信的波特率
*Output         ：
*Return         ：
*************************************************************/
void UART4_Init(u32 bound)
{
	GPIO_InitTypeDef GPIOC_InitStructure;
	USART_InitTypeDef UART4_InitStructure;
	
	/*******初始化GPIOC和UART4的时钟************/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	
	/*****复用GPIOC10和GPIOC11为UART4***********/
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);
	
	GPIOC_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIOC_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOC_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIOC_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIOC_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	
    GPIO_Init(GPIOC, &GPIOC_InitStructure);
	
	UART4_InitStructure.USART_BaudRate = bound;  //波特率
	UART4_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //无硬件流控制
	UART4_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //接收和发送模式
	UART4_InitStructure.USART_Parity = USART_Parity_No;  //无奇偶校验
	UART4_InitStructure.USART_StopBits = USART_StopBits_1;  //1个停止位
	UART4_InitStructure.USART_WordLength = USART_WordLength_8b;  //8个字长
	
	USART_Init(UART4, &UART4_InitStructure);  //UART1初始化
	
	USART_Cmd(UART4, ENABLE);  //使能UART4
}


/*************************************************************
*Function Name  : UART4_Send_Data
*Auther         : 张沁
*Version        : V1.0
*Date           : 2017-04-19
*Description    : UART3数据发送
*Input          ：1. *data 要发送到内容的指针
                  2. len   数据长度
*Output         ：
*Return         ：
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
*Auther         : 张沁
*Version        : v1.0
*Date           : 2017-05-03
*Description    : 语音信息打包并发送
*Input          ：*XFSdata  要发送的语音包缓存指针
*Output         ：
*Return         ：
*************************************************************/
void XFS_SendPkt(char *XFSdata)
{
	u8  dataBuf[50] = {0};
	u8 XFS_Length = 0;
	
	XFS_Length = strlen(XFSdata);
	
	/**************帧固定配置信息**************/           
	dataBuf[0] = 0xFD ; 		 //构造帧头FD
	dataBuf[1] = 0x00 ; 		 //构造数据区长度的高字节
	dataBuf[2] = XFS_Length + 2; //构造数据区长度的低字节
	dataBuf[3] = 0x01 ; 		 //构造命令字：合成播放命令		 		 
	dataBuf[4] = 0x00;           //文本编码格式：GB2312 
	
	memcpy(&dataBuf[5], XFSdata, XFS_Length);  //数据打包
	
	UART4_Send_Data(dataBuf, XFS_Length + 5);  //发送数据
}

