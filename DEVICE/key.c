/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : key.c
*Auther         : ����
*Version        : V2.0
*Date           : 2017-04-16
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

extern OS_EVENT *semWifi;
extern OS_EVENT *semPaperPut;

/*************************************************************
*Function Name  : KeyInit
*Auther         : ����
*Version        : V2.0
*Date           : 2017-04-16
*Description    : ���ļ������˶԰���Key0(PE4)�ĳ�ʼ��
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void KeyInit(void)
{
	GPIO_InitTypeDef GPIO_KEY_InitStructure;
	EXTI_InitTypeDef EXTI_KEY_InitStructure;
	NVIC_InitTypeDef NVIC_KEY_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);   //ʹ��AHB1ʼ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);  //ʹ��APB2ʼ��
	
	GPIO_KEY_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_KEY_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_KEY_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_KEY_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_KEY_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_KEY_InitStructure);
	
	/******����EXTI4�ж��߿�����***********/
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);
	
	EXTI_KEY_InitStructure.EXTI_Line = EXTI_Line4;            //�ⲿ�ж���4
	EXTI_KEY_InitStructure.EXTI_LineCmd = ENABLE;             //ʹ��
	EXTI_KEY_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;   //�ж��¼�
	EXTI_KEY_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//�����ش���
	EXTI_Init(&EXTI_KEY_InitStructure);
	
	/******����Ƕ�������жϿ�����***********/
	NVIC_KEY_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_KEY_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_KEY_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;  //��ռ���ȼ�2
	NVIC_KEY_InitStructure.NVIC_IRQChannelSubPriority = 0x03;   //�����ȼ�3
	NVIC_Init(&NVIC_KEY_InitStructure);
	
}

/*************************************************************
*Function Name  : EXTI4_IRQHandler
*Auther         : ����
*Version        : V2.0
*Date           : 2017-04-16
*Description    : KEY0���жϷ������� 
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void EXTI4_IRQHandler(void)
{	
	//char retValue[60] = {0};
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	
	EXTI_ClearFlag(EXTI_Line4);  //���Line4�ϵı�־λ
	
	printf("�ж���Ӧ�ɹ�\r\n");
	
	//SendCmd("͸���ɹ�", NULL, NULL,retValue, 1000);
	OSSemPost(semPaperPut);
	
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
}
