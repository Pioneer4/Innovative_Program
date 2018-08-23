/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : delay.c
*Auther         : ����
*Version        : v2.0  ����uCOS-II
*Date           : 2017-04-15
*Description    : ��systickʵ�ֽϾ�׼����ʱ
*Function List  : 
*Histoty		:
1. Date: 2017-04-15    Auther: ����
   Modifycation: ���uCOS-II���ݳ���
2. Date: 2017-05-16    Auther: ����
   Modifycation: �޸��˴����ڷ�uCOS�Ĺ����У���ʱ�����BUG
*************************************************************/
#include "myinclude.h"
#include "sys.h"

#if SYSTEM_SUPPORT_OS
#include "includes.h"	//֧��OSʱ��ʹ��	  
#endif

static u8 fac_us = 0;   //us��ʱ������
static u16 fac_ms = 0;  //ms��ʱ����������os�£�����ÿ�����ĵ�ms��

#if SYSTEM_SUPPORT_OS

#ifdef 	OS_CRITICAL_METHOD						//OS_CRITICAL_METHOD������,˵��Ҫ֧��UCOSII				
#define delay_osrunning		OSRunning			//OS�Ƿ����б��,0,������;1,������
#define delay_ostickspersec	OS_TICKS_PER_SEC	//OSʱ�ӽ���,��ÿ����ȴ���
#define delay_osintnesting 	OSIntNesting		//�ж�Ƕ�׼���,���ж�Ƕ�״���
#endif

/*************************************************************
*Function Name  : delay_osschedlock
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-15
*Description    : us����ʱʱ,�ر��������(��ֹ���us���ӳ�)
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void delay_osschedlock(void)
{
	OSSchedLock();  //��ֹ���ȣ���ֹ���us��ʱ
}

/*************************************************************
*Function Name  : delay_osschedunlock
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-15
*Description    : us����ʱʱ,�ָ��������
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void delay_osschedunlock(void)
{
	OSSchedUnlock();  //�ָ�����
}

/*************************************************************
*Function Name  : delay_ostimedly
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-15
*Description    : ����OS�Դ�����ʱ������ʱ
*Input          ��1. ticks  ��ʱ�Ľ�����
*Output         ��
*Return         ��
*************************************************************/
void delay_ostimedly(u32 ticks)
{
	OSTimeDly(ticks);						//UCOSII��ʱ
}

/*************************************************************
*Function Name  : SysTick_Handler
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-15
*Description    : systick�жϷ�����,ʹ��OSʱ�õ�
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void SysTick_Handler(void)
{
	if (delay_osrunning == 1)
	{
		OSIntEnter();						//�����ж�
		OSTimeTick();       				//����ucos��ʱ�ӷ������               
		OSIntExit();       	 				//���������л����ж�
	}
}

#endif

/*************************************************************
*Function Name  : delay_init
*Auther         : ����
*Vertion        : v2.0 �������uCOS-II
*Date           : 2017-04-15
*Description    : SysTick��ʼ����ʱ��Դѡ��Ϊ�ⲿʱ�ӣ�
*Input          ��1. SYSCLK ϵͳʱ��Ƶ�� ��λ:M
*Output         ��
*Return         ��
*************************************************************/
void delay_init(u8 SYSCLK)
{
#if SYSTEM_SUPPORT_OS  //�����Ҫ֧��OS.
	u32 reload;
#endif
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	fac_us = SYSCLK / 8;
	
#if SYSTEM_SUPPORT_OS
	reload = SYSCLK / 8;                        //ÿ��ļ������� ��λ��M
	reload *= 1000000 / delay_ostickspersec;    //����OSʱ�ӽ����趨���ʱ��
	                                            //reloadΪ24λ�Ĵ���,���ֵ:16777216,��168M��,Լ��0.7989s����
	fac_ms = 1000 / delay_ostickspersec;        //����OS������ʱ�����ٵ�λ  1000ms/OS_TICKS_PER_SEC
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;  //����SYSTICK�ж�
	SysTick->LOAD = reload;                     //ÿ 1/delay_ostickspersec ���ж�һ��
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   //����SYSTICK
	
#else
	fac_ms = (u16)fac_us * 1000;                 //��OS��,����ÿ��ms��Ҫ��systickʱ����
	
#endif
}

#if SYSTEM_SUPPORT_OS 						//�����Ҫ֧��OS

/*************************************************************
*Function Name  : delay_us
*Auther         : ����
*Vertion        : v2.0 �������uCOS-II
*Date           : 2017-04-15
*Description    : ʵ��us��ʱ
*Input��        : 1.nus ��ʱ��us�� ��Ҫ����798915us
*Output         ��
*Return         ��
*************************************************************/
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt = 0;
	u32 reload = SysTick->LOAD;	 //LOAD��ֵ	    	 
	ticks = nus * fac_us;  	     //��Ҫ�Ľ����� 
	delay_osschedlock();		 //��ֹOS���ȣ���ֹ���us��ʱ
	told = SysTick->VAL;       	 //�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow=SysTick->VAL;	
		if (tnow != told)
		{	    
			if (tnow < told) tcnt+=told-tnow;//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if (tcnt >= ticks) break;		 //ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	};
	delay_osschedunlock();					 //�ָ�OS����											    
} 

/*************************************************************
*Function Name  : delay_ms
*Auther         : ����
*Vertion        : v2.0 �������uCOS-II
*Date           : 2017-04-15
*Description    : ʵ��us��ʱ
*Input��        : 1.nms ��ʱ��ms�� 0~65535
*Output         ��
*Return         ��
*************************************************************/
void delay_ms(u16 nms)
{	
    if (delay_osrunning && delay_osintnesting == 0)//���OS�Ѿ�������,���Ҳ������ж�����(�ж����治���������)	    
	{		 
		if (nms>=fac_ms)						   //��ʱ��ʱ�����OS������ʱ������ 
		{ 
   			delay_ostimedly(nms/fac_ms);           //OS��ʱ
		}
		nms %= fac_ms;						       //OS�Ѿ��޷��ṩ��ôС����ʱ��,������ͨ��ʽ��ʱ    
	}
	delay_us((u32)(nms*1000));				       //��ͨ��ʽ��ʱ����ֹOS���ȣ���ֹ���us��ʱ��
}

#else
/************************************************************/
//Function Name: delay_xms
//Auther: ����
//Vertion: v1.0
//Date: 2016-09-01
//Description: ʵ��ms����ʱ
//Input��1.nms ��ʱ��ms�� nms <= 798ms
//Output��
//Return��
/************************************************************/
void delay_xms(u16 nms)
{
	u32 temp;
	
	SysTick->LOAD = (u32) nms * fac_ms;        //ʱ�����ֵ
	SysTick->VAL = 0x00;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;  //��ʼ����
	
	do
	{
		temp = SysTick->CTRL;
	}
	while((temp & 0x01) && !(temp & (1<<16)));
	
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; //�رռ�����
	SysTick->VAL = 0x00;  //��ռ�����
}

/************************************************************/
//Function Name: delay_us
//Auther: ����
//Vertion: v1.0
//Date: 2016-08-31
//Description: ʵ��us��ʱ
//Input��1.nus ��ʱ��us�� ��Ҫ����798915us
//Output��
//Return��
/************************************************************/
void delay_us(u32 nus)
{
	u32 temp;
	SysTick->LOAD = (u32) nus * fac_us;        //ʱ�����ֵ
	SysTick->VAL = 0x00;                       //��ռ�����
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;  //��ʼ����
	
	do
	{
		temp = SysTick->CTRL;
	}
	while((temp & 0x01) && !(temp & (1<<16))); //�ȴ�ʱ�䵽��
	
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; //�رռ�����
	SysTick->VAL = 0x00;                       //��ռ�����
}

/************************************************************/
//Function Name: delay_ms
//Auther: ����
//Vertion: v1.0
//Date: 2016-08-31
//Description: ʵ��ms��ʱ
//Input��1.nms ��ʱ��ms�� 0~65535
//Output��
//Return��
/************************************************************/
void delay_ms(u16 nms)
{
	u8 repeat = nms / 540;  //������540,�ǿ��ǵ�ĳЩ�ͻ����ܳ�Ƶʹ��,
						    //���糬Ƶ��248M��ʱ��,delay_xms���ֻ����ʱ541ms������
	
	u16 remain = nms % 540;
	
	while(repeat)
	{
		delay_xms(540);
		repeat--;
	}
	
	if(remain)
	{
		delay_xms(remain);
	}
}
#endif
