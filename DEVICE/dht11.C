/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : dht11.c
*Auther         : ����
*Version        : v1.0
*Date           : 2016-05-03
*Description    : ���ļ�������dht11����������
*Function List  : 
*************************************************************/

#include "myinclude.h"

/*************************************************************
*Function Name  : DHT11_Init
*Auther         : ����
*Version        : v1.0
*Date           : 2016-05-03
*Description    : ��ʼ��DHT11��DAT��(PE2) 
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void DHT11_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}


/*************************************************************
*Function Name  : DHT11_Rst
*Auther         : ����
*Version        : v1.0
*Date           : 2016-05-03
*Description    : ��λDHT11
*Input          ��
*Output         ��
*Return         ��1,����    0,������
*************************************************************/
void DHT11_Rst(void)
{
	DHT11_IO_OUT(); 
	DHT11_DQ_OUT = 0;
    delay_ms(20);	  //������������18ms
	DHT11_DQ_OUT = 1;
	delay_us(30);	  //������������20~40us
}


/*************************************************************
*Function Name  : DHT11_Check
*Auther         : ����
*Version        : v1.0
*Date           : 2016-05-03
*Description    : �ȴ�DHT11�Ļ�Ӧ ��û�л�Ӧ���ʾ������
*Input          ��
*Output         ��
*Return         ��1,����    0,������
*************************************************************/
u8 DHT11_Check(void) 	
{
	u8 retry=0;
	
	DHT11_IO_IN();
	while (DHT11_DQ_IN && retry<60) //DHT11������40~50us
	{
		retry++;
		delay_us(1);
	}
	if (retry == 60)
	{
		return 0;                   //�ȴ�Ӧ��ʧ��
	}
	else
	{
		retry = 0;
	}
	
	while (!DHT11_DQ_IN && retry<60)//DHT11������40~50us
	{
		retry++;
		delay_us(1);
	}
	if (retry == 60)
	{
		return 0;                   //�ȴ�Ӧ��ʧ��
	}
	
	return 1;                       //�ȴ�Ӧ��ɹ�
}

/*************************************************************
*Function Name  : DHT11_Read_Bit
*Auther         : ����
*Version        : v1.0
*Date           : 2016-05-03
*Description    : ��DHT11��ȡһ��λ
*Input          ��
*Output         ��
*Return         ��1,�����ߵ�ƽ    0�������͵�ƽ
*************************************************************/
u8 DHT11_Read_Bit(void)
{
	u8 retry = 0;
	
	while (DHT11_DQ_IN && retry<100) //�ȴ���Ϊ�͵�ƽ
	{
		retry++;
		delay_us(1);
	}
	
	retry=0;
	while (!DHT11_DQ_IN && retry<100)//�ȴ���ߵ�ƽ
	{
		retry++;
		delay_us(1);
	}
	
	/***��ʾ����0���ź�,�ߵ�ƽ����26~28us.����1���ź�,�ߵ�ƽ����116~118us***/
	delay_us(40);                  
	if (DHT11_DQ_IN == 1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*************************************************************
*Function Name  : DHT11_Read_Bit
*Auther         : ����
*Version        : v1.0
*Date           : 2016-05-03
*Description    : ��DHT11��ȡһ���ֽ�����
*Input          ��
*Output         ��
*Return         ����ȡ��һ�ֽ�����
*************************************************************/
u8 DHT11_Read_Byte(void)
{
	u8 i = 0;
	u8 dat = 0;
	for (i=0; i<8; i++) 
	{
   		dat <<= 1; 
	    dat |= DHT11_Read_Bit();
    }
	
	return dat;
}


/*************************************************************
*Function Name  : DHT11_Read_Data
*Auther         : ����
*Version        : v1.0
*Date           : 2016-05-03
*Description    : ��DHT11��ȡһ���ֽ�����
*Input          ��1. *temp  �¶����ݱ�����
                  2. *humi  ʪ�����ݱ�����
*Output         ��
*Return         ��1,���ݻ�ȡ�ɹ�   0,���ݻ�ȡʧ��
*************************************************************/
u8 DHT11_Read_Data(u8 *temp, u8 *humi)
{
	u8 buf[5];
	u8 i;
	
	DHT11_Rst();
	if (DHT11_Check()==1)
	{
		for(i=0; i<5; i++)//��ȡ40λ����
		{
			buf[i] = DHT11_Read_Byte();
		}
		if ( (buf[0]+buf[1]+buf[2]+buf[3])==buf[4] )
		{
			*humi=buf[0];
			*temp=buf[2];
		}
	}
	else 
	{
		return 0;
	}
	
	return 1;
}