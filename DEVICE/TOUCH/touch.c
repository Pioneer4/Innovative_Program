/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : touch.c
*Auther         : ����
*Version        : v1.0
*Date           : 2017-04-30
*Description    : ����������   (֧��XPT2046)
*Function List  : 
*History        :
1. ��ate: 2017-05-01  Auther: ����
Modification: ������TP_Save_Adjdata��TP_Adj_Info_Show����
*************************************************************/
#include "myinclude.h"

#if SYSTEM_SUPPORT_OS
#include "includes.h"  //ucos ʹ��	  
#endif

extern OS_FLAG_GRP *flagTP;

_m_tp_dev tp_dev=
{
	TP_Init,
	TP_Scan,
	TP_Adjust,
	0,
	0, 
	0,
	0,
	0,
	0,	  	 		
	0,
	0,	  	 		
};		

/******Ĭ��Ϊtouchtype=0������******/
u8 CMD_RDX=0XD0;
u8 CMD_RDY=0X90;
 	 			    					   
/*************************************************************
*Function Name  : TP_Write_Byte
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-30
*Description    : ������ICд��1byte����
*Input          ��num  Ҫд�������
*Output         ��
*Return         ��
*************************************************************/
void TP_Write_Byte(u8 num)    
{  
	u8 count=0;  
	
	for(count=0;count<8;count++)  
	{ 	  
		if (num & 0x80)
		{
			TDIN = 1;  
		}
		else 
		{
			TDIN = 0;   
		}
		num <<= 1;    
		TCLK = 0; 
		delay_us(1);
		TCLK = 1;    //��������Ч	        
	}		 			    
} 		 


/*************************************************************
*Function Name  : TP_Read_AD
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-30
*Description    : �Ӵ�����IC��ȡadcֵ
*Input          ��CMD  ָ��
*Output         ��
*Return         ������������
*************************************************************/
u16 TP_Read_AD(u8 CMD)	  
{ 	 
	u8 count = 0; 	  
	u16 Num = 0; 
	TCLK=0;	           //������ʱ�� 	 
	TDIN=0; 	       //����������
	TCS=0; 		       //ѡ�д�����IC
	TP_Write_Byte(CMD);//����������
	delay_us(6);       //ADS7846��ת��ʱ���Ϊ6us
	TCLK=0; 	     	    
	delay_us(1);    	   
	TCLK=1;		       //��1��ʱ�ӣ����BUSY
	delay_us(1);    
	TCLK=0; 
    /*****����16λ����,ֻ�и�12λ��Ч*****/	
	for (count=0; count<16; count++)
	{ 				  
		Num <<= 1; 	 
		TCLK = 0;	   //�½�����Ч  	    	   
		delay_us(1);    
 		TCLK = 1;
 		if (DOUT)
		{
			Num++; 		 
		}
	}  	
	Num >>= 4;   	   //ֻ�и�12λ��Ч.
	TCS = 1;	       //�ͷ�Ƭѡ	 
	return (Num);   
}


/*************************************************************
*Function Name  : TP_Read_XOY
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-30
*Description    : 1.��ȡһ������ֵ(x����y)
                  2.������ȡREAD_TIMES������,����Щ������������,
                    Ȼ��ȥ����ͺ����LOST_VAL����,ȡƽ��ֵ 
*Input          ��xy  ָ�CMD_RDX/CMD_RDY��
*Output         ��
*Return         ������������
*************************************************************/
#define  READ_TIMES 5 	 //��ȡ����
#define  LOST_VAL 1	     //����ֵ
u16 TP_Read_XOY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum = 0;
	u16 temp;
	
	/*******ȡֵ*******/
	for (i=0; i<READ_TIMES; i++)
	{
		buf[i] = TP_Read_AD(xy);		 		    
	}
	
	/******��������******/
	for (i=0; i<READ_TIMES-1; i++)
	{
		for (j=i+1; j<READ_TIMES; j++)
		{
			if (buf[i] > buf[j])
			{
				temp = buf[i];
				buf[i] = buf[j];
				buf[j] = temp;
			}
		}
	}	  
	
	for (i=LOST_VAL; i<READ_TIMES-LOST_VAL; i++)
	{
		sum += buf[i];
	}
	temp = sum / (READ_TIMES - 2*LOST_VAL);
	return temp;   
} 


/*************************************************************
*Function Name  : TP_Read_XY
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-30
*Description    : 1.��ȡx,y����
                  2.������ȡREAD_TIMES������,����Щ������������,
                    Ȼ��ȥ����ͺ����LOST_VAL����,ȡƽ��ֵ 
*Input          ��1. *x  x������洢��
                  2. *y  y������洢��
*Output         ��
*Return         ��1,�ɹ�  0,ʧ��
*************************************************************/
u8 TP_Read_XY(u16 *x,u16 *y)
{
	u16 xtemp,ytemp;			 	 		  
	xtemp = TP_Read_XOY(CMD_RDX);
	ytemp = TP_Read_XOY(CMD_RDY);	  												   
	*x = xtemp;
	*y = ytemp;
	return 1;//�����ɹ�
}


/*************************************************************
*Function Name  : TP_Read_XY2
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-30
*Description    : 1.����2�ζ�ȡ������IC,�������ε�ƫ��ܳ���
                    ERR_RANGE,��������,����Ϊ������ȷ,�����������.
                  2.�ú����ܴ�����׼ȷ�� 
*Input          ��1. *x  x������洢��
                  2. *y  y������洢��
*Output         ��
*Return         ��1,�ɹ�  0,ʧ��
*************************************************************/
#define  ERR_RANGE  50   //��Χ 
u8 TP_Read_XY2(u16 *x, u16 *y) 
{
	u16 x1, y1;
 	u16 x2, y2;
 	u8 flag;  
	
    flag = TP_Read_XY(&x1, &y1);   
    if(flag==0)
	{
		return 0;       //��ȡʧ��
	}
	
    flag = TP_Read_XY(&x2, &y2);	   
    if(flag==0)
	{
		return 0;       //��ȡʧ��
	}
	
	/*******ǰ�����β�����+-50��*******/
    if(( (x2<=x1 && x1<x2+ERR_RANGE) || (x1<=x2 && x2<x1+ERR_RANGE) )
    && ( (y2<=y1 && y1<y2+ERR_RANGE) || (y1<=y2 && y2<y1+ERR_RANGE) ))
    {
        *x = (x1 + x2) / 2;
        *y = (y1 + y2) / 2;
        return 1;
    }
	else 
	{
		return 0;	  
	}
}  		  

/*************************************************************
*Function Name  : TP_Drow_Touch_Point
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-30
*Description    : ��һ�������㣨����У׼�õģ�
*Input          ��1. x      x������
                  2. y      y������
                  3. color  �����ɫ
*Output         ��
*Return         ��
*************************************************************/
void TP_Drow_Touch_Point(u16 x,u16 y,u16 color)
{
	POINT_COLOR=color;
	LCD_DrawLine(x-12,y,x+13,y);//����
	LCD_DrawLine(x,y-12,x,y+13);//����
	LCD_DrawPoint(x+1,y+1);
	LCD_DrawPoint(x-1,y+1);
	LCD_DrawPoint(x+1,y-1);
	LCD_DrawPoint(x-1,y-1);
	LCD_Draw_Circle(x,y,6);//������Ȧ
}	  

/*************************************************************
*Function Name  : TP_Draw_Big_Point
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-30
*Description    : ��һ�����(2*2�ĵ�)
*Input          ��1. x      x������
                  2. y      y������
                  3. color  �����ɫ
*Output         ��
*Return         ��
*************************************************************/
void TP_Draw_Big_Point(u16 x,u16 y,u16 color)
{	    
	POINT_COLOR=color;
	LCD_DrawPoint(x,y);  //���ĵ� 
	LCD_DrawPoint(x+1,y);
	LCD_DrawPoint(x,y+1);
	LCD_DrawPoint(x+1,y+1);	 	  	
}						  


/*************************************************************
*Function Name  : Load_Drow_Dialog
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-05-01
*Description    : �����Ļ�������Ͻ���ʾ"CLR"
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);  //����   
 	POINT_COLOR=BLUE;  //��������Ϊ��ɫ 
	LCD_ShowString(lcddev.width-24, 0, 24, 16, 16, (u8*)"CLR");//��ʾ��������
  	POINT_COLOR=RED;   //���û�����ɫ 
}


/*************************************************************
*Function Name  : TP_Scan
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-30
*Description    : ��������ɨ��
*Input          ��tp  
				  @arg 0  ��Ļ����
				  @arg 1  ��������(У׼�����ⳡ����)
*Output         ��
*Return         ��1,�����д��� 0,�����޴���
*************************************************************/
u8 TP_Scan(u8 tp)
{
	/****** �а������� ******/
	if (PEN == 0)  
	{
		if (tp)
		{
			TP_Read_XY2(&tp_dev.x[0], &tp_dev.y[0]); //��ȡ��������
		}
		
		/****** ��ȡ�������꣬������װ������Ļ���� ******/
		else if (TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))   
		{
	 		tp_dev.x[0]=tp_dev.xfac*tp_dev.x[0]+tp_dev.xoff;
			tp_dev.y[0]=tp_dev.yfac*tp_dev.y[0]+tp_dev.yoff;  
	 	} 
		
		/****** ֮ǰû�б����� ******/
		if ( (tp_dev.sta & TP_PRES_DOWN) == 0 )      
		{		 
			tp_dev.sta = TP_PRES_DOWN | TP_CATH_PRES; //״̬λ8��7ͬʱ��1 
			
			/**��¼��һ�ΰ���ʱ������**/
			tp_dev.x[4]=tp_dev.x[0];                 
			tp_dev.y[4]=tp_dev.y[0];  	   			 
		}			   
	}
	
	/****** �ް������� ******/
	else
    {
		if(tp_dev.sta & TP_PRES_DOWN) //֮ǰ�Ǳ����µ�
		{
			tp_dev.sta &= ~(1<<7);    //��Ǵ������ɿ�	
		}
		
		
		else                          //֮ǰ��û�б�����
		{
			tp_dev.x[4]=0;
			tp_dev.y[4]=0;
			tp_dev.x[0]=0xff;
			tp_dev.y[0]=0xff;
		}		
	}
	
	return (tp_dev.sta & TP_PRES_DOWN);//���ص�ǰ�Ĵ���״̬
}	  


/*************************************************************
*Function Name  : TP_Save_Adjdata
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-30
*Description    : ���津����У׼����
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
//����FLASH����ĵ�ַ�����ַ,ռ��18���ֽ�(RANGE:SAVE_ADDR_BASE~SAVE_ADDR_BASE+17)
#define  SAVE_ADDR_BASE  1
void TP_Save_Adjdata(void)
{
	u8 result_data[18];  //�洢������
	int temp = 0;

	/******************У�����ת��Ϊ�洢����******************/
	/*xУ������*/
    temp = tp_dev.xfac*100000000;
	result_data[0] =  (u8)(temp >> 24);
	result_data[1] =  (u8)(temp >> 16);
	result_data[2] =  (u8)(temp >> 8);
	result_data[3] =  (u8)temp;
	/*yУ������*/
	temp = tp_dev.yfac * 100000000;    
    result_data[4] =  (u8)(temp >> 24);
	result_data[5] =  (u8)(temp >> 16);
	result_data[6] =  (u8)(temp >> 8);
	result_data[7] =  (u8)temp;
	/*xƫ����*/
	temp = tp_dev.xoff * 100000;    
    result_data[8] =  (u8)(temp >> 24);
	result_data[9] =  (u8)(temp >> 16);
	result_data[10] = (u8)(temp >> 8);
	result_data[11] = (u8)temp;
	/*yƫ����*/
	temp = tp_dev.yoff * 100000;    
    result_data[12] = (u8)(temp >> 24);
	result_data[13] = (u8)(temp >> 16);
	result_data[14] = (u8)(temp >> 8);
	result_data[15] = (u8)temp;
	/*��������*/
	temp = tp_dev.touchtype;
	result_data[16] = (u8)temp;
    /*���У׼����*/
	temp=0x0A;
	result_data[17] = (u8)temp;
	
	W25QXX_Write(result_data, SAVE_ADDR_BASE, 18);  //У�����д��FLASH
}


/*************************************************************
*Function Name  : TP_Get_Adjdata
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-30
*Description    : �õ�������FLASH�����У׼ֵ
*Input          ��
*Output         ��
*Return         ��1���ɹ���ȡ����    0����ȡʧ�ܣ�Ҫ����У׼
*************************************************************/
u8 TP_Get_Adjdata(void)
{					
	u8 result_data[18];  //�洢������
	int temp = 0;

	W25QXX_Read(result_data, SAVE_ADDR_BASE, 18);  //��ȡFLASH�е�����
	
	temp = result_data[17]; //��ȡ�����,���Ƿ�У׼���� 
	
	/***********�������Ѿ�У׼����***********/
	if (temp == 0x0A)			   
	{
		/*�õ�xУ׼����*/
		temp = result_data[0];
		temp = (temp << 8) + result_data[1];
		temp = (temp << 8) + result_data[2];
		temp = (temp << 8) + result_data[3];
		tp_dev.xfac = (float)temp / 100000000;
		/*�õ�yУ׼����*/
		temp = result_data[4];
		temp = (temp << 8) + result_data[5];
		temp = (temp << 8) + result_data[6];
		temp = (temp << 8) + result_data[7];
		tp_dev.yfac = (float)temp / 100000000;
        /*�õ�xƫ����*/
        temp = result_data[8];
		temp = (temp << 8) + result_data[9];
		temp = (temp << 8) + result_data[10];
		temp = (temp << 8) + result_data[11];
		tp_dev.xoff = (float)temp / 100000;
		/*�õ�yƫ����*/
        temp = result_data[12];
		temp = (temp << 8) + result_data[13];
		temp = (temp << 8) + result_data[14];
		temp = (temp << 8) + result_data[15];
		tp_dev.yoff = (float)temp / 100000;
		/*�õ��������ͱ��*/
 		temp = result_data[16];		
 		tp_dev.touchtype = temp;
		if(tp_dev.touchtype)   //X,Y��������Ļ�෴
		{
			CMD_RDX=0X90;
			CMD_RDY=0XD0;	 
		}else				   //X,Y��������Ļ��ͬ
		{
			CMD_RDX=0XD0;
			CMD_RDY=0X90;	 
		}		 
		return 1;	 
	}
	return 0;  //������δУ׼
}	 


/*************************************************************
*Function Name  : TP_Adj_Info_Show
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-05-01
*Description    : ��ʾУ׼���(��������)
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
//��ʾ�ַ���
u8* const TP_REMIND_MSG_TBL = (u8*)"Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";
void TP_Adj_Info_Show(u16 x0, u16 y0, u16 x1, u16 y1, u16 x2, u16 y2, u16 x3, u16 y3, u16 fac)
{	  
	POINT_COLOR = RED;
	LCD_ShowString(40, 160, lcddev.width, lcddev.height, 16, (u8*)"x1:");
 	LCD_ShowString(40+80, 160, lcddev.width, lcddev.height, 16, (u8*)"y1:");
 	LCD_ShowString(40, 180, lcddev.width, lcddev.height,16,(u8*)"x2:");
 	LCD_ShowString(40+80, 180, lcddev.width, lcddev.height, 16, (u8*)"y2:");
	LCD_ShowString(40,200, lcddev.width, lcddev.height, 16, (u8*)"x3:");
 	LCD_ShowString(40+80, 200, lcddev.width, lcddev.height, 16, (u8*)"y3:");
	LCD_ShowString(40,220, lcddev.width, lcddev.height, 16, (u8*)"x4:");
 	LCD_ShowString(40+80, 220, lcddev.width, lcddev.height, 16, (u8*)"y4:");  
 	LCD_ShowString(40,240, lcddev.width, lcddev.height, 16, (u8*)"fac is:");     
	LCD_ShowNum(40+24, 160, x0, 4, 16);		//��ʾ��ֵ
	LCD_ShowNum(40+24+80, 160, y0, 4, 16);	//��ʾ��ֵ
	LCD_ShowNum(40+24, 180, x1, 4, 16);		//��ʾ��ֵ
	LCD_ShowNum(40+24+80, 180, y1, 4, 16);	//��ʾ��ֵ
	LCD_ShowNum(40+24, 200, x2, 4, 16);		//��ʾ��ֵ
	LCD_ShowNum(40+24+80, 200, y2, 4, 16);	//��ʾ��ֵ
	LCD_ShowNum(40+24, 220, x3, 4, 16);     //��ʾ��ֵ
	LCD_ShowNum(40+24+80, 220, y3, 4, 16);	//��ʾ��ֵ
 	LCD_ShowNum(40+56, 240, fac, 3, 16);	//��ʾ��ֵ,����ֵ������95~105��Χ֮��.

}


/*************************************************************
*Function Name  : TP_Adjust
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-30
*Description    : ������У׼  �õ��ĸ�У׼����
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void TP_Adjust(void)
{								 
	u16 pos_temp[4][2];  //���껺��ֵ
	u8  cnt = 0;	
	u16 d1, d2;
	u32 tem1, tem2;
	double fac; 	
	u16 outtime = 0;
 				
	POINT_COLOR = BLUE;
	BACK_COLOR  = WHITE;
	
	LCD_Clear(WHITE);   //����   
	POINT_COLOR=RED;    //�����ɫ 
	LCD_Clear(WHITE);   //���� 	   
	POINT_COLOR=BLACK;  //�����ɫ
	LCD_ShowString(40,40,160,100,16,(u8*)TP_REMIND_MSG_TBL);//��ʾ��ʾ��Ϣ
	
	TP_Drow_Touch_Point(20, 20, RED);      //����1 
	tp_dev.sta=0;      //���������ź� 
	tp_dev.xfac=0;     //xfac��������Ƿ�У׼��,����У׼֮ǰ�������!�������	 
	while (1)          //�������10����û�а���,���Զ��˳�
	{
		tp_dev.scan(1);           //ɨ����������
		
		/*** ����������һ�κ�(��ʱ�����ɿ���)�Ĵ������ ***/
		if ((tp_dev.sta&0xc0) == TP_CATH_PRES)
		{	
			outtime=0;		
			tp_dev.sta &= ~(1<<6);//��ǰ����Ѿ����������.
						   			   
			pos_temp[cnt][0]=tp_dev.x[0];
			pos_temp[cnt][1]=tp_dev.y[0];
			cnt++;
			
			switch(cnt)
			{			   
				case 1:						 
					TP_Drow_Touch_Point(20,20,WHITE);				          //�����1 
					TP_Drow_Touch_Point(lcddev.width-20,20,RED);	          //����2
					break;
				case 2:
 					TP_Drow_Touch_Point(lcddev.width-20,20,WHITE);	          //�����2
					TP_Drow_Touch_Point(20,lcddev.height-20,RED);	          //����3
					break;
				case 3:
 					TP_Drow_Touch_Point(20,lcddev.height-20,WHITE);           //�����3
 					TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,RED);//����4
					break;
				case 4:	 //ȫ���ĸ����Ѿ��õ�
					//�Ա����
				    /*** �õ�1,2�ľ��� ***/
					tem1 = abs(pos_temp[0][0] - pos_temp[1][0]);  //x1-x2
					tem2 = abs(pos_temp[0][1] - pos_temp[1][1]);  //y1-y2
					tem1 *= tem1;
					tem2 *= tem2;
					d1 = sqrt(tem1+tem2);
					
				    /*** �õ�3,4�ľ��� ***/
					tem1 = abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2 = abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1 *= tem1;
					tem2 *= tem2;
					d2=sqrt(tem1+tem2);  
				
					fac = (float)d1/d2;
				
				    /*** У׼���ϸ� �����У׼ ***/
					if (fac<0.95 || fac>1.05 || d1==0 || d2==0)
					{
						cnt = 0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);//�����4
   	 					TP_Drow_Touch_Point(20,20,RED);								//����1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
 						continue;
					}//ˮƽ�ߺϸ�
					
					/*** �õ�1,3�ľ��� ***/
					tem1 = abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2 = abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1 *= tem1;
					tem2 *= tem2;
					d1 = sqrt(tem1 + tem2);
					
					/*** �õ�2,4�ľ��� ***/
					tem1 = abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2 = abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1 *= tem1;
					tem2 *= tem2;
					d2 = sqrt(tem1 + tem2);
					
					fac = (float)d1/d2;
					
					/*** У׼���ϸ� �����У׼ ***/
					if (fac<0.95 || fac>1.05)
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);//�����4
   	 					TP_Drow_Touch_Point(20,20,RED);								//����1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
						continue;
					}//�Խ��ߺϸ�
					
					/*** �õ�2,3�ľ��� ***/					
					tem1 = abs(pos_temp[1][0]-pos_temp[2][0]);//x2-x3
					tem2 = abs(pos_temp[1][1]-pos_temp[2][1]);//y2-y3
					tem1 *= tem1;
					tem2 *= tem2;
					d1 = sqrt(tem1 + tem2);
	                 
					/*** �õ�1,4�ľ��� ***/		
					tem1 = abs(pos_temp[0][0]-pos_temp[3][0]);//x1-x4
					tem2 = abs(pos_temp[0][1]-pos_temp[3][1]);//y1-y4
					tem1 *= tem1;
					tem2 *= tem2;
					d2 = sqrt(tem1 + tem2);
					
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);//�����4
   	 					TP_Drow_Touch_Point(20,20,RED);								//����1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
						continue;
					}//��ֱ�ߺϸ�
					
					/*** ��ʼ������ ***/
					tp_dev.xfac=(float)(lcddev.width-40)/(pos_temp[1][0]-pos_temp[0][0]);     //�õ�xfac		 
					tp_dev.xoff=(lcddev.width-tp_dev.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2; //�õ�xoff
						  
					tp_dev.yfac=(float)(lcddev.height-40)/(pos_temp[2][1]-pos_temp[0][1]);    //�õ�yfac
					tp_dev.yoff=(lcddev.height-tp_dev.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//�õ�yoff  
					
					/********* ������Ԥ����෴�� *********/
					if (abs(tp_dev.xfac)>2 || abs(tp_dev.yfac)>2)
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//�����4
   	 					TP_Drow_Touch_Point(20,20,RED);								    //����1
						LCD_ShowString(40, 26, lcddev.width, lcddev.height, 16, (u8*)"TP Need readjust!");
						tp_dev.touchtype = !tp_dev.touchtype;   //�޸Ĵ�������.
						
						if(tp_dev.touchtype)  //X,Y��������Ļ�෴
						{
							CMD_RDX=0X90;
							CMD_RDY=0XD0;	 
						}
						else				  //X,Y��������Ļ��ͬ
						{
							CMD_RDX=0XD0;
							CMD_RDY=0X90;	 
						}			    
						continue;
					}		
					
					POINT_COLOR=BLUE;
					LCD_Clear(WHITE);  //����
					LCD_ShowString(35, 110, lcddev.width, lcddev.height, 16, (u8*)"Touch Screen Adjust OK!");//У�����
					delay_ms(1000);
					TP_Save_Adjdata();  
 					LCD_Clear(WHITE);  //����   
					return;            //У�����				 
			}
		}
		
		delay_ms(1);
		outtime++; 
		if(outtime>10000)
		{
			TP_Get_Adjdata();
			break;
	 	}
	}
}	


/*************************************************************
*Function Name  : TP_Init
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-30
*Description    : ��������ʼ��  
                  ����W25Q16��SPI�ӿڣ����ģ�⣩��T_PEN��
*Input          ��
*Output         ��
*Return         ��1,���й�У׼  0,û�н���У׼
*************************************************************/
u8 TP_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;	
	EXTI_InitTypeDef EXTI_PT_InitStructure;
	NVIC_InitTypeDef NVIC_PT_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);  //ʹ��SYCFGʱ��

    /****** PCin(5)   T_PEN ******/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;      //����ģʽ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      //����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);         

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource5);	
	
	EXTI_PT_InitStructure.EXTI_Line = EXTI_Line5;  
	EXTI_PT_InitStructure.EXTI_LineCmd = ENABLE;                  //ʹ��
	EXTI_PT_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;        //�ж��¼�
	EXTI_PT_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;    //�½��ش���
	EXTI_Init(&EXTI_PT_InitStructure);
	
	NVIC_PT_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_PT_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_PT_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//��ռ���ȼ�2
	NVIC_PT_InitStructure.NVIC_IRQChannelSubPriority = 0x00;       //�����ȼ�0
	NVIC_Init(&NVIC_PT_InitStructure);

	/****** PBin(14)  T_MISO ******/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;      //����ģʽ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      //����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);           

    /*** T_MOSI:PBout(15) T_SCK:PBout(13) T_CS:PBout(12) ***/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_13 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     //���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);           	

	W25QXX_Init();		                    //��ʼ��W25Q16
	TP_Read_XY(&tp_dev.x[0], &tp_dev.y[0]); //��һ�ζ�ȡ��ʼ��
	
	if ( TP_Get_Adjdata() )
	{
		return 0;                           //�ɹ���ȡУ׼����
	}
	
	else			                        //δУ׼���ȡУ׼����ʧ��
	{ 										    
		LCD_Clear(WHITE);                   //����
		TP_Adjust();  	                    //��ĻУ׼ 
		TP_Save_Adjdata();	 
	}			
	TP_Get_Adjdata();	
	
	return 1; 									 
}


/*************************************************************
*Function Name  : EXTI9_5_IRQHandler
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-30
*Description    : �����Ļ������ T_PEN���Ŵ������ж�  
                  �����¼���־ ��Ļ��ʼɨ�败��������
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void EXTI9_5_IRQHandler(void)
{
	u8 errEXTI9_5_IRQHandler;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	
	if (EXTI_GetFlagStatus(EXTI_Line5) == SET)
	{
		EXTI_ClearFlag(EXTI_Line5);  //���Lin5�ϵı�־λ
		OSFlagPost(flagTP, 0x01, OS_FLAG_SET, &errEXTI9_5_IRQHandler);
	}
	
#if SYSTEM_SUPPORT_OS 	           //���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
}
