/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : spi.c
*Auther         : ����
*Version        : v1.0
*Date           : 2016-09-22
*Description    : ���ļ��������й�SPI1�ĳ�ʼ�����ж�ʹ��
*Function List  : 
*************************************************************/
#include "myinclude.h"

//u8 SPI1_Data_Rev = 0x00;

/*************************************************************
*Function Name  : SPI1_Init
*Auther         : ����
*Version        : v1.0
*Date           : 2016-09-22
*Description    : ��ʼ��SPI1
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;  
	SPI_InitTypeDef SPI_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);             //ʹ��SPI1ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  	 		 //ʹ��GPIOB��ʱ��
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;                        //���ù���
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;                      //�������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5; //PB3��4��5�������
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;  					     //����
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;                    //�ٶ�100MHz
	GPIO_Init(GPIOB, &GPIO_InitStruct);  //��ʼ��GPIOB
	
	
	/********���Ÿ���***************/
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);          //PB4  SPI1_MISO
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);  		 //PB5  SPI1_MOSI
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);          //PB3  SPI1_SCK
	
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//������Ԥ��ƵֵΪ256
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;                        //����ʱ������ĵĵڶ��������أ����������������ݱ�����
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;                         //����ʱ������ߵ�ƽ��Ч
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;	                 //����SPI�����ݴ�С�����ͽ���8Ϊ֡�ṹ
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;  				 //���ݴӸ�λ��ʼ����
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;  					 //��SPI
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft; 							 //�ڲ�NSS�ź���SSIλ����
	SPI_Init(SPI1, &SPI_InitStruct);
	
	SPI_Cmd(SPI1, ENABLE);  //ʹ��SPI����
	
	SPI1_ReadWriteByte(0xff);//��������	
}


/*************************************************************
*Function Name  : SPI1_ReadWriteByte
*Auther         : ����
*Version        : v1.0
*Date           : 2016-09-23
*Description    : SPI1 ��дһ���ֽ�
*Input          ��TxData  Ҫд����ֽ�
*Output         ��
*Return         ���������ֽ�
*************************************************************/
u8 SPI1_ReadWriteByte(u8 TxData)
{
	while(SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE) == RESET);  //�ȴ�������Ϊ��
	SPI_SendData(SPI1, (u16)TxData);                        //ͨ��SPI1����һ��Byte����
	
	while (SPI_GetFlagStatus(SPI1, SPI_FLAG_RXNE) == RESET);//�ȴ����ջ������ǿգ�������1Byte��
	
	return SPI_ReceiveData(SPI1);                           //ͨ��SPI���ؽ��յ�����
}


/*************************************************************
*Function Name  : SPI1_SetSpeed
*Auther         : ����
*Version        : v1.0
*Date           : 2016-09-23
*Description    : SPI1�ٶ����ú�����SPI�ٶ�=fAPB2/��Ƶϵ��
*Input          ��SPI_BaudRatePrescaler  ��Ƶϵ��
*Output         ��
*Return         ��
*************************************************************/
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//�ж���Ч��
	SPI1->CR1 &= 0XFFC7;                 //λ3-5���㣬�������ò�����
	SPI1->CR1 |= SPI_BaudRatePrescaler;  //����SPI1�ķ�Ƶϵ��
	SPI_Cmd(SPI1,ENABLE);	             //ʹ��SPI1
}
