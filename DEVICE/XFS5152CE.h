/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : XFS5152CE.h
*Auther         : 张沁
*Version        : v1.0
*Date           : 2017-05-03
*Description    : 该文件包含了XFS5152CE.c中所有的函数原型
*Function List  : 
*************************************************************/
#ifndef __XFS5152CE_H
#define __XFS5152CE_H

#include "myinclude.h"

void UART4_Init(u32 bound);
void UART4_Send_Data(u8 *data, u8 len);
void XFS_SendPkt(char *XFSdata);

#endif
