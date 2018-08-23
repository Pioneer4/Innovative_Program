/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : iwdg.c
*Auther         : ����
*Version        : v1.0
*Date           : 2017-04-16
*Description    : ���ļ����ڶ������Ź��ĳ��Ի�
*Function List  : 
*************************************************************/

#include "myinclude.h"

/*************************************************************
*Function Name  : iwdgInit
*Auther         : ����
*Version        : v1.0
*Date           : 2016-09-07
*Description    : ����IWDG�ĳ�ʼ��
*Input          ��1. prr ��Ƶϵ�� ��Ƶ = 4*2^prr  prr(0~6)
                  2. rlr ��װ��ֵ 0~65535
*Output         ��
*Return         ��
*************************************************************/
void IwdgInit(u8 prr, u16 rlr)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //ʹ�ܶ� IWDG_PR �� IWDG_RLR �Ĵ����ķ���
	IWDG_SetPrescaler(prr);                       //����Ԥ��Ƶֵ
	IWDG_SetReload(rlr);                          //������װ��ֵ
	IWDG_Enable();                                //���Ź�ʹ��
	IWDG_ReloadCounter();                         //���¼�����װ��ֵ
}

/*************************************************************
*Function Name  : IwdgFeed
*Auther         : ����
*Version        : v1.0
*Date           : 2017-04-16
*Description    : IWDG��ι������
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void IwdgFeed(void)
{

	IWDG_ReloadCounter();
}
