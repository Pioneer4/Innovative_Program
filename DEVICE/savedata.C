/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : savedata.c
*Auther         : ����
*Version        : v1.0
*Date           : 2016-05-03
*Description    : ���ļ������˴������������������
*Function List  : 
*************************************************************/

#include "myinclude.h"
#include "includes.h"

extern INT16U firstLevelNum; //��һ���Ѿ������Ĵ�������
extern INT16U secondLevelNum;//�ڶ����Ѿ������Ĵ�������

/*************************************************************
*Function Name  : PaperNumSaveData
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-10-09
*Description    : ���洫�������Ѿ����͵Ĵ�������
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
//����FLASH����ĵ�ַ�����ַ,ռ��24���ֽ�(RANGE:PID_SAVE_ADDR_BASE~PID_SAVE_ADDR_BASE+23)
#define  PID_SAVE_ADDR_BASE  20
void PaperNumSaveData(void)
{
	INT8U result_data[4];  //�洢������
	INT16U temp = 0;
	
	/******��firstLevelNum��secondLevelNum��Ϊ�洢����******/
	/*firstLevelNum*/
	temp = firstLevelNum;
	result_data[0] =  (u8)(temp >> 8);
	result_data[1] =  (u8)temp;	
	/*secondLevelNum*/
	temp = secondLevelNum;
	result_data[2] =  (u8)(temp >> 8);
	result_data[3] =  (u8)temp;
	
	W25QXX_Write(result_data, PID_SAVE_ADDR_BASE, 4);  //У�����д��FLASH
}


/*************************************************************
*Function Name  : PaperNumGetData
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-10-09
*Description    : �õ�������FLASH����Ĵ�������ֵ
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void PaperNumGetData(void)
{
	INT8U result_data[4];  //�洢������
	INT16U temp = 0;
	
	W25QXX_Read(result_data, PID_SAVE_ADDR_BASE, 4);  //��ȡFLASH�е�����
	
	/*�õ�firstLevelNum*/
	temp = result_data[0];
	temp = (temp << 8) + result_data[1];
	firstLevelNum = temp;
	/*�õ�secondLevelNum*/
	temp = result_data[2];
	temp = (temp << 8) + result_data[3];
	secondLevelNum = temp;
}