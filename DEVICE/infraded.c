/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : infraded.c
*Auther         : 张沁
*Vertion        : v1.0
*Date           : 2017-10-08
*Description    : 该文件包含了红外传感器(infraded sensor)的初始化
*Function List  : 
*************************************************************/

#include "myinclude.h"

/*************************************************************
*Function Name  : InfradedInit
*Auther         : 张沁
*Vertion        : v1.0
*Date           : 2017-10-08
*Description    : 初始化红外传感器相关引脚
                  PC1     ：读取人体红外输出引脚电平
                  PE0，PE1：读取红外对管输出引脚电平
*Input          ：
*Output         ：
*Return         ：
*************************************************************/
void InfradedInit(void)
{
	GPIO_InitTypeDef GPIOC_InitStructure;
	GPIO_InitTypeDef GPIOE_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	/************人体红外输出引脚************/
	GPIOC_InitStructure.GPIO_Mode = GPIO_Mode_IN;          //输入模式
	GPIOC_InitStructure.GPIO_Pin = GPIO_Pin_1;             //GPIOC1
	GPIOC_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;        //下拉
	GPIOC_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //速度50M
	GPIO_Init(GPIOC, &GPIOC_InitStructure);                //初始化PC1
	
	/************2个红外对管输出引脚************/
	GPIOE_InitStructure.GPIO_Mode = GPIO_Mode_IN;          //输入模式
	GPIOE_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;//GPIOE0,GPIOE1
	GPIOE_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;        //下拉
	GPIOE_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //速度50M
	GPIO_Init(GPIOE, &GPIOE_InitStructure);                //初始化PE0，PE1
}
