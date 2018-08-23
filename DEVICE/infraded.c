/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : infraded.c
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-10-08
*Description    : ���ļ������˺��⴫����(infraded sensor)�ĳ�ʼ��
*Function List  : 
*************************************************************/

#include "myinclude.h"

/*************************************************************
*Function Name  : InfradedInit
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-10-08
*Description    : ��ʼ�����⴫�����������
                  PC1     ����ȡ�������������ŵ�ƽ
                  PE0��PE1����ȡ����Թ�������ŵ�ƽ
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void InfradedInit(void)
{
	GPIO_InitTypeDef GPIOC_InitStructure;
	GPIO_InitTypeDef GPIOE_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	/************��������������************/
	GPIOC_InitStructure.GPIO_Mode = GPIO_Mode_IN;          //����ģʽ
	GPIOC_InitStructure.GPIO_Pin = GPIO_Pin_1;             //GPIOC1
	GPIOC_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;        //����
	GPIOC_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //�ٶ�50M
	GPIO_Init(GPIOC, &GPIOC_InitStructure);                //��ʼ��PC1
	
	/************2������Թ��������************/
	GPIOE_InitStructure.GPIO_Mode = GPIO_Mode_IN;          //����ģʽ
	GPIOE_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;//GPIOE0,GPIOE1
	GPIOE_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;        //����
	GPIOE_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //�ٶ�50M
	GPIO_Init(GPIOE, &GPIOE_InitStructure);                //��ʼ��PE0��PE1
}
