/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : infraded.h
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-10-08
*Description    : ���ļ�����infraded.c�����еĺ���ԭ��
*Function List  : 
*************************************************************/
#ifndef __INFRADED_H
#define __INFRADED_H

#include "myinclude.h"

#define INFRADED_PIN    PCin(1)   //��ȡ�������������ŵ�ƽ

#define FIRST_FLOOR     PEin(0)   //��ȡ�������������ŵ�ƽ
#define SECOND_FLOOR    PEin(1)   //��ȡ�������������ŵ�ƽ

void InfradedInit(void);

#endif
