/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : w25qxx.c
*Auther         : ����
*Version        : v1.0
*Date           : 2016-09-23
*Description    : W25QXX ��������
*Function List  : 
*************************************************************/
#include "w25qxx.h"

u16 W25QXX_TYPE = W25Q16;  //Ĭ��Ϊ��W25Q16

/*************************************************************
*Function Name  : W25QXX_Init
*Auther         : ����
*Version        : v1.0
*Date           : 2016-09-23
*Description    : W25Q16 ��ʼ��
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void W25QXX_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	W25QXX_CS = 1;                         //CS���øߵ�ƽ  FLASH��ѡ��
	
	SPI1_Init();                           //��ʼ��SPI1�ӿ�
    SPI1_SetSpeed(SPI_BaudRatePrescaler_2);//��Ƶ168M PB2=84M  SPI=PB2/2=42Mʱ��,����ģʽ 
	W25QXX_TYPE = W25QXX_ReadID();         //��ȡFLASH ID.
}


/*************************************************************
*Function Name  : W25QXX_ReadID
*Auther         : ����
*Vertion        : v1.0
*Date           : 2016-09-23
*Description    : ��ȡW25QXXоƬID
*Input          ��
*Output         ��
*Return         ��оƬ��ID
*************************************************************/
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80  
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16    
//0XEF15,��ʾоƬ�ͺ�ΪW25Q32  
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
//0XEF17,��ʾоƬ�ͺ�ΪW25Q128 
u16  W25QXX_ReadID(void)
{
	u16 Temp = 0;
	
	W25QXX_CS = 0;
	SPI1_ReadWriteByte(W25X_ManufactDeviceID);  //���Ͷ�ȡID�������豸������
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0x00); 	
	Temp |= (SPI1_ReadWriteByte(0xFF) << 8);
	Temp |= SPI1_ReadWriteByte(0xFF);
	W25QXX_CS = 1;
	
	return Temp;
}


/*************************************************************
*Function Name  : W25QXX_ReadSR
*Auther         : ����
*Vertion        : v1.0
*Date           : 2016-09-23
*Description    : ��ȡW25QXX��״̬�Ĵ���
*Input          ��
*Output         ��
*Return         ��״̬�Ĵ�������
*************************************************************/
//��ȡW25QXX��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
u8	 W25QXX_ReadSR(void)
{
	u8 byte = 0;
	
	W25QXX_CS = 0;                          //Ƭѡ
	SPI1_ReadWriteByte(W25X_ReadStatusReg); //���Ͷ�ȡ״̬�Ĵ�������
	byte = SPI1_ReadWriteByte(0Xff);	    //��ȡһ���ֽ�
	W25QXX_CS = 1;							//ȡ��Ƭѡ
	
	return byte;
}


/*************************************************************
*Function Name  : W25QXX_Write_SR
*Auther         : ����
*Vertion        : v1.0
*Date           : 2016-09-23
*Description    : д��25QXX��״̬�Ĵ���
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
//дW25QXX״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void W25QXX_Write_SR(u8 sr)
{
	W25QXX_CS=0;                             //Ƭѡ  
	SPI1_ReadWriteByte(W25X_WriteStatusReg); //����дȡ״̬�Ĵ�������
	SPI1_ReadWriteByte(sr);                  //д��һ���ֽ�
	W25QXX_CS = 1; 				             //ȡ��Ƭѡ
}


/*************************************************************
*Function Name  : W25QXX_Write_Enable
*Auther         : ����
*Vertion        : v1.0
*Date           : 2016-09-23
*Description    : W25QXXдʹ�ܣ���WEL��λ 
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void W25QXX_Write_Enable(void)
{
	W25QXX_CS = 0;                         //Ƭѡ
	SPI1_ReadWriteByte(W25X_WriteEnable);  //����дʹ��ָ��
	W25QXX_CS = 1;                         //ȡ��Ƭѡ
}


/*************************************************************
*Function Name  :  W25QXX_Write_Disable
*Auther         : ����
*Vertion        : v1.0
*Date           : 2016-09-23
*Description    : W25QXXд��ֹ
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void W25QXX_Write_Disable(void)
{
	W25QXX_CS = 0;                         //Ƭѡ
	SPI1_ReadWriteByte(W25X_WriteDisable); //����д��ָֹ��
	W25QXX_CS = 1;                         //ȡ��Ƭѡ
}


/*************************************************************
*Function Name  : W25QXX_Write_NoCheck
*Auther         : ����
*Vertion        : v1.0
*Date           : 2016-09-23
*Description    : �޼���дFLASH
                  1.����ȷ����д�ĵ�ַ��Χ�ڵ�ȫ������Ϊ0xff�������ڷ�0xff��д������ݽ�ʧ��!
				  2.�����Զ���ҳ�Ĺ���
				  3.��ָ����ַ��ʼд��ָ�����ȵ����ݣ�����Ҫȷ����ַ��Խ�磡
*Input          ��1. pBuffer        ���ݴ洢��
				  2. WriteAddr      ��ʼд��ĵ�ַ��24bit��
				  3. NumByteToWrite Ҫд����ֽ��������65535��
*Output         ��
*Return         ��
*************************************************************/
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
	u16 pageremain;	
	pageremain = 256 - WriteAddr%256;  //��ҳʣ����ֽ���
	
	if(NumByteToWrite <= pageremain)   //������256���ֽ�
	{
		pageremain = NumByteToWrite;
	}
	while(1)
	{
		W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
		
		/********д�����*********/
		if(NumByteToWrite == pageremain)
			break;  
		
		/******* NumByteToWrite > pageremain **********/
		else
		{
			pBuffer += pageremain;  //����λ�ƶ�
			WriteAddr += pageremain;  //��ַλ�ƶ�
			NumByteToWrite -= pageremain;  //��ȥ�Ѿ�д����ֽ���
			
			if(NumByteToWrite > 256)
				pageremain = 256;  //һ��д��256���ֽ�
			else
				pageremain = NumByteToWrite;  //����256���ֽ�
		}
	}
}


/*************************************************************
*Function Name  : W25QXX_Write_Page
*Auther         : ����
*Vertion        : v1.0
*Date           : 2016-09-23
*Description    : ��һҳ��256Byte����д�벻����256���ֽڵ�����
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void W25QXX_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
	u16 i ;
	W25QXX_Write_Enable();                //����д����
	W25QXX_CS = 0;                        //Ƭѡ
	SPI1_ReadWriteByte(W25X_PageProgram); //SET WEL(The WEL bit must be set prior to every Page Program)
	
	/********����24bit��ַ***********/
	SPI1_ReadWriteByte((u8)((WriteAddr)>>16));
	SPI1_ReadWriteByte((u8)((WriteAddr)>>8));
	SPI1_ReadWriteByte((u8)WriteAddr);
	
	/******ѭ��д��********/
	for(i = 0; i < NumByteToWrite; i++)
	{
		SPI1_ReadWriteByte(pBuffer[i]);
	}
	
	W25QXX_CS = 1;                       //ȡ��Ƭѡ
	W25QXX_Wait_Busy();                  //�ȴ�д�����
}


/*************************************************************
*Function Name  : W25QXX_Read
*Auther         : ����
*Vertion        : v1.0
*Date           : 2016-09-23
*Description    : ��ȡFLASH
*Input          ��1. pBuffer       ���ݴ洢��
				  2. ReadAddr      ��ȡ��ַ
				  3. NumByteToRead Ҫ��ȡ���ֽ���(���65535)
*Output         ��
*Return         ��
*************************************************************/
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)  
{
	u16 i; 
	W25QXX_CS=0;                        //Ƭѡ
	SPI1_ReadWriteByte(W25X_ReadData);  //���Ͷ�ȡָ��

	/*******����24bit��ַ*******/
	SPI1_ReadWriteByte((u8)((ReadAddr)>>16)); 
	SPI1_ReadWriteByte((u8)((ReadAddr)>>8));
	SPI1_ReadWriteByte((u8)ReadAddr);
	
	/*****ѭ������ ******/
	for(i = 0; i < NumByteToRead; i++)
	{
		 pBuffer[i] = SPI1_ReadWriteByte(0XFF);   	 
	}
    
	W25QXX_CS = 1;
}


/*************************************************************
*Function Name  : W25QXX_Write
*Auther         : ����
*Vertion        : v1.0
*Date           : 2016-09-23
*Description    : дFLASH
				  1. ��ָ����ַ��ʼд��ָ�����ȵ�����
				  2. �ú�������������!
*Input          ��1. pBuffer       ���ݴ洢��
				  2. WriteAddr     ��ʼд��ĵ�ַ(24bit)
				  3. NumByteToRead Ҫ��ȡ���ֽ���(���65535)
*Output         ��
*Return         ��
*************************************************************/
u8 W25QXX_BUFFER[4096];	 
void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
	u32 secpos;
	u16 secoff;
	u16 secremain;
	u16 i;
	
	u8 * W25QXX_BUF;
	W25QXX_BUF=W25QXX_BUFFER;
	
	secpos = WriteAddr/4096;                         //������ַ
	secoff = WriteAddr%4096;                         //�������ڵ�ƫ��
	secremain = 4096 - secoff;                       //����ʣ��ռ��С
	
	if(NumByteToWrite <= secremain) 
		secremain = NumByteToWrite;                  //������4096���ֽ�
	
	while(1)
	{
		W25QXX_Read(W25QXX_BUF, secpos * 4096, 4096);//��ȡ��������������
		for(i = 0; i < secremain; i++)
		{
			if(W25QXX_BUF[secoff + i] != 0xff)
				break;                               //��Ҫ����
		}
		if(i < secremain)                            //��Ҫ����
		{
			W25QXX_Erase_Sector(secpos);             //������������
			for(i = 0; i < secremain; i++)           //����
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];
			}
			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//д���������� 
		}
		else
			W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������.
		
		if(NumByteToWrite  == secremain)
			break;                         //д�����
		else                               //д��δ����
		{
			secpos++;                      //������ַ��1
			secoff=0;                      //ƫ��λ��Ϊ0
			
			pBuffer += secremain;          //ָ��ƫ��
			WriteAddr += secremain;        //д��ַƫ��
			NumByteToWrite -= secremain;   //�ֽ����ݼ�
			if(NumByteToWrite>4096) 
				secremain = 4096;	       //��һ����������д����
			else 
				secremain = NumByteToWrite;//��һ����������д����
		}
	};
}


/*************************************************************
*Function Name  : W25QXX_Erase_Sector
*Auther         : ����
*Vertion        : v1.0
*Date           : 2016-09-24
*Description    : ����һ������  ����һ������������ʱ��:150ms  
*Input          ��1. Dst_Addr  ������ַ ����ʵ����������
*Output         ��
*Return         ��
*************************************************************/
void W25QXX_Erase_Sector(u32 Dst_Addr)	
{
    //����falsh�������,������   
 	printf("fe:%x\r\n",Dst_Addr);
	
	Dst_Addr *= 4096;
	
	W25QXX_Write_Enable(); 
	W25QXX_Wait_Busy();  
	
	W25QXX_CS = 0;
	SPI1_ReadWriteByte(W25X_SectorErase);//������������ָ��
	
	/*******����24bit��ַ*******/
	SPI1_ReadWriteByte((u8)(Dst_Addr >> 16));
	SPI1_ReadWriteByte((u8)(Dst_Addr >> 8));
	SPI1_ReadWriteByte((u8)Dst_Addr);
	
	W25QXX_CS = 1;
	W25QXX_Wait_Busy();                  //�ȴ��������
}


/*************************************************************
*Function Name  : W25QXX_Erase_Chip
*Auther         : ����
*Vertion        : v1.0
*Date           : 2016-09-24
*Description    : ��������оƬ   �ȴ�ʱ�䳬��.. 
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void W25QXX_Erase_Chip(void)
{
	W25QXX_Write_Enable(); 
	W25QXX_Wait_Busy(); 
	
	W25QXX_CS = 0;                      //Ƭѡ
	SPI1_ReadWriteByte(W25X_ChipErase); //����Ƭ��������  
	W25QXX_CS=1;                        //ȡ��Ƭѡ     	      
	
	W25QXX_Wait_Busy();                 //�ȴ�оƬ��������
}


/*************************************************************
*Function Name  : W25QXX_Wait_Busy
*Auther         : ����
*Vertion        : v1.0
*Date           : 2016-09-23
*Description    : �ȴ�����
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void W25QXX_Wait_Busy(void)
{
	while((W25QXX_ReadSR()&0x01)==0x01);  // �ȴ�BUSYλ���
}


/*************************************************************
*Function Name  : W25QXX_PowerDown
*Auther         : ����
*Vertion        : v1.0
*Date           : 2016-09-24
*Description    : �������ģʽ
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void W25QXX_PowerDown(void)   
{ 
  	W25QXX_CS=0;                        //ʹ������   
    SPI1_ReadWriteByte(W25X_PowerDown); //���͵�������  
	W25QXX_CS=1;                        //ȡ��Ƭѡ   
	
    delay_us(3);                        //�ȴ�TPD  
}   


/*************************************************************
*Function Name  : W25QXX_WAKEUP
*Auther         : ����
*Vertion        : v1.0
*Date           : 2016-09-24
*Description    : ����FLASH
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void W25QXX_WAKEUP(void)   
{  
  	W25QXX_CS=0;                              //ʹ������   
    SPI1_ReadWriteByte(W25X_ReleasePowerDown);//���ͻ�������  
	W25QXX_CS=1;                              //ȡ��Ƭѡ     
	
    delay_us(3);                              //�ȴ�TRES1
}   
