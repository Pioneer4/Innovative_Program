/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : infraded.h
*Auther         : 张沁
*Vertion        : v1.0
*Date           : 2017-10-08
*Description    : 该文件包含infraded.c中所有的函数原型
*Function List  : 
*************************************************************/
#ifndef __INFRADED_H
#define __INFRADED_H

#include "myinclude.h"

#define INFRADED_PIN    PCin(1)   //读取人体红外输出引脚电平

#define FIRST_FLOOR     PEin(0)   //读取人体红外输出引脚电平
#define SECOND_FLOOR    PEin(1)   //读取人体红外输出引脚电平

void InfradedInit(void);

#endif
