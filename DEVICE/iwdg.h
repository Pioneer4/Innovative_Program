/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : iwdg.h
*Auther         : ����
*Version        : v1.0
*Date           : 2017-04-16
*Description    : ���ļ�������iwdg.c�����еĺ���ԭ��
*Function List  : 
*************************************************************/

#ifndef __IWDG_H
#define __IWDG_H

#include "myinclude.h"

void IwdgInit(u8 prr, u16 rlr);
void IwdgFeed(void);

#endif
