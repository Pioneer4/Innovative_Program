/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : main.c
*Auther         : ����
*Version        : v1.0
*Date           : 2016-11-10
*Description    : ��������ڣ����UC/OS-II�����Ŀ
*Function List  : 
*History        :
1. Date: 2017-04-23  Auther: ����
Modification: ���esp8266 ATָ�� �ɹ�����WIFI��OneNet������
2. Date: 2017-05-02  Auther: ����
Modification: ��Ӵ�������XPT2046����������
2. Date: 2017-05-03  Auther: ����
Modification: ���DHT11��������  ����������� ����ģ�����
*************************************************************/
/*
*��Ŀ����:
*����UCOS����ϵͳ�������������
*����һ���ڶ�ʱ�ĵ�ƺ�ι��
*�������Ⲣ��Ӧ����
*��������������ʾ������ʾ����һ���������״̬������������LED��״̬��������а�����״̬��
*/
#include "myinclude.h"
#include "includes.h"

OS_EVENT *semWifi;    //�ź��������յ�ESP8266��������
OS_EVENT *semWifiData;//�ź��������յ�����������

OS_EVENT *semPaperPut;   //�ź�����װֽ�����ɿ�ʼ����
OS_EVENT *semFloorFir;   //�ź�������������һ����Կ�ʼ��ֽ
OS_EVENT *semFloorSec;   //�ź�����װֽ���ڶ�����Կ�ʼ��ֽ

/*
�¼���־�飺���������
BIT0: �������Ƿ�ʼɨ�败���� 
*/
OS_FLAG_GRP *flagTP; 

u8 wifiConnectState;  //��־��WIFI�����Ƿ�ɹ� 
u8 TCP_ConnectState;  //��־��TCP�����Ƿ�ɹ�
u8 regProcess;        //��־���Ƿ���ע���ACK
u8 putPaperState;     //��־��װֽ��־  0����ʼװֽ  1��װֽ���

INT16U firstLevelNum; //��һ���Ѿ������Ĵ�������
INT16U secondLevelNum;//�ڶ����Ѿ������Ĵ�������

INT8U temperature = 0;  	    
INT8U humidity = 0; 

extern WIFI_Data  Wifi_Data_Buf;


/********************************************
����        ��WifiRevTask  wifi���ݽ�������
�������ȼ�  ��8
�����ջ��С��128
�����ջ    ��wifiRevTaskStk[WIFI_REV_STK_SIZE]
*********************************************/
#define WIFI_REV_TASK_PRIO    8
#define WIFI_REV_STK_SIZE  	  128
OS_STK wifiRevTaskStk[WIFI_REV_STK_SIZE];
void WifiRevTask(void *pdata);


/********************************************
����        ��WifiTrmTask   wifi���ݷ�������
�������ȼ�  ��9
�����ջ��С��256
�����ջ    ��wifiTrmTaskStk[WIFI_TRM_STK_SIZE]
*********************************************/
#define WIFI_TRM_TASK_PRIO        9
#define WIFI_TRM_STK_SIZE  	      256            //!128 �Ͳ���
OS_STK wifiTrmTaskStk[WIFI_TRM_STK_SIZE];
void WifiTrmTask(void *pdata);


/********************************************
����        :UartTask     UART1�������
�������ȼ�  :10
�����ջ��С:128
�����ջ    :uartTaskStk[KEYO_STK_SIZE]
*********************************************/
#define UART_TASK_PRIO       10  	  
#define UART_STK_SIZE  		 128
OS_STK uartTaskStk[UART_STK_SIZE];
void UartTask(void *pdata);

/********************************************
����        ��DC1_MotorTask
�������ȼ�  ��11
�����ջ��С��128
�����ջ    ��DC1_MotorTaskStk[DC1_MOTOR_STK_SIZE]
*********************************************/
#define DC1_MOTOR_TASK_PRIO   11
#define DC1_MOTOR_STK_SIZE    128
OS_STK DC1_MotorTaskStk[DC1_MOTOR_STK_SIZE];
void DC1MotorTask(void *pdata);


/********************************************
����        ��DC2_MotorTask
�������ȼ�  ��12
�����ջ��С��128
�����ջ    ��DC2_MotorTaskStk[DC2_MOTOR_STK_SIZE];
*********************************************/
#define DC2_MOTOR_TASK_PRIO   12
#define DC2_MOTOR_STK_SIZE    128
OS_STK DC2_MotorTaskStk[DC2_MOTOR_STK_SIZE];
void DC2MotorTask(void *pdata);


/********************************************
����        ��TravelMotorTask
�������ȼ�  ��13
�����ջ��С��128
�����ջ    ��travelMotorTaskStk[TRAVEL_MOTOR_STK_SIZE]
*********************************************/
#define TRAVEL_MOTOR_TASK_PRIO   13
#define TRAVEL_MOTOR_STK_SIZE  	 128
OS_STK travelMotorTaskStk[TRAVEL_MOTOR_STK_SIZE];
void TravelMotorTask(void *pdata);


/********************************************
����        ��LcdShowTask  LCD��ʾ������
�������ȼ�  ��14
�����ջ��С��128
�����ջ    ��lcdShowTaskStk[LCD_SHOW_STK_SIZE]
*********************************************/
#define LCD_SHOW_TASK_PRIO 14    	  
#define LCD_SHOW_STK_SIZE  128  		    	
OS_STK lcdShowTaskStk[LCD_SHOW_STK_SIZE];
void LcdShowTask(void *pdata);


/********************************************
����        ��TP_Task    TP����������
�������ȼ�  ��15
�����ջ��С��256
�����ջ    ��tpTaskStk[TP_STK_SIZE];
*********************************************/
#define TP_TASK_PRIO      15
#define TP_STK_SIZE  	  128
OS_STK tpTaskStk[TP_STK_SIZE];
void TP_Task(void *pdata);

/********************************************
����        ��START ����������������
�������ȼ�  ��16
�����ջ��С��256
�����ջ    ��startTaskSTK[START_STK_SIZE]
*********************************************/
#define START_TASK_PRIO      16
#define START_STK_SIZE  	 128
OS_STK startTaskSTK[START_STK_SIZE];
void StartTask(void *pdata);


/*************************************************************
*Function Name  : IwdgTmrCallBack
*Auther         : ����
*Vertion        : v1.0
*Date           : 2016-12-06
*Description    : ��ʱ���ص�����������ι����С�Ƶ���˸
*Input          : 1. void *ptmr
                  2. void *parg
				  û�д��ݵĲ��������������
*Output         :
*Return         :
*************************************************************/
INT8U timeNum = 0;  //ʱ�����
OS_TMR *pTmrIwdg;
void IwdgTmrCallBack(void *ptmr, void *parg)
{
	OS_CPU_SR cpu_sr = 0;
	
	if(GET_LED2_STATUS() == LED_STATUS_ON)
	{
        Led2Switch(OFF);
	}
	else
	{
		Led2Switch(ON);
	}
	
	IwdgFeed();  //ι��
	
	/*ÿ��1���ӽ���һ�����ݱ���*/   
	if (timeNum == 60)            
	{
		timeNum = 0;
		
		/*��FLASH����д����ʱ����������ٽ�Σ���ֹ�ж�����ʱ�����*/
		OS_ENTER_CRITICAL(); 
		PaperNumSaveData();
		OS_EXIT_CRITICAL();
	}
	else
	{
		timeNum++;
	}
	
}


u8 err = 0;  //���󷵻���


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	
	delay_init(168);      //��ʼ����ʱ����
	LedInit();		      //��ʼ��LED�˿� 
	UART1_Init(115200);   //��ʼ������1
	UART4_Init(9600);     //��ʼ������4 ��������ģ��ͨ��
	XFS_SendPkt("��λ��ʦ�ã������ǻ���ucos�����ܴ�����ʵ����ʾ");
	LCD_Init();	          //��ʼ��LCD	
	KeyInit();   	      //��ʼ������
	InfradedInit();       //��ʼ�����⴫����
    TravelMotorInit();    //��ʼ���г̵��
	DcMotorInit();        //��ʼ��ֱ�����
	
	OSInit();             //��ʼ��uC/OS
	OSTaskCreate(StartTask, (void*)0, (OS_STK *)&startTaskSTK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
	OSStart();	
	
	return 0;
}

/*************************************************************
*Function Name  : StartTask
*Auther         : ����
*Vertion        : v1.0
*Date           : 2016-12-05
*Description    : ��ʼ���� ���������ڽ�������������
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void StartTask(void *pdata)
{
	OS_CPU_SR cpu_sr = 0;
	
	pdata = pdata; 
	
	semWifi = OSSemCreate(0);	       //�����ź���	���յ�ESP8266��������	 
	semWifiData = OSSemCreate(0);      //�����ź���	���յ�����������
	wifiConnectState = ESP8266_Init(); //��ʼ��ESP8266����ӿڣ����������״̬
	
	semFloorFir = OSSemCreate(0);      //�����ź��� ��������һ����Կ�ʼ��ֽ
	semFloorSec = OSSemCreate(0);      //�����ź��� װֽ���ڶ�����Կ�ʼ��ֽ
	semPaperPut = OSSemCreate(0);      //�����ź��� װֽ�����ɿ�ʼ����
	
	flagTP = OSFlagCreate(0, &err);    //�����¼���־��
	
	OSStatInit();					   //��ʼ��ͳ������.�������ʱ1��������
	
	OS_ENTER_CRITICAL();		       //�����ٽ���(�޷����жϴ��)    
	
	pTmrIwdg = OSTmrCreate(0, OS_TMR_CFG_TICKS_PER_SEC, OS_TMR_OPT_PERIODIC, (OS_TMR_CALLBACK)IwdgTmrCallBack, NULL, NULL, &err);
	
	if(OSTmrStart(pTmrIwdg, &err) == OS_TRUE)
	{
		printf("Iwdg Tmr start!\r\n");
		
	}
	else
	{
		printf("Iwdg Tmr err!");
	}
	
	OSTaskCreate(UartTask,(void *)0,(OS_STK *)&uartTaskStk[UART_STK_SIZE-1],UART_TASK_PRIO);
    OSTaskCreate(TP_Task,(void *)0,(OS_STK*)&tpTaskStk[TP_STK_SIZE-1],TP_TASK_PRIO);
	OSTaskCreate(WifiTrmTask,(void *)0,(OS_STK*)&wifiTrmTaskStk[WIFI_TRM_STK_SIZE-1],WIFI_TRM_TASK_PRIO);
	OSTaskCreate(WifiRevTask,(void *)0,(OS_STK*)&wifiRevTaskStk[WIFI_REV_STK_SIZE-1],WIFI_REV_TASK_PRIO);
	OSTaskCreate(DC1MotorTask,(void *)0,(OS_STK *)&DC1_MotorTaskStk[DC1_MOTOR_STK_SIZE-1],DC1_MOTOR_TASK_PRIO);
	OSTaskCreate(DC2MotorTask,(void *)0,(OS_STK *)&DC2_MotorTaskStk[DC2_MOTOR_STK_SIZE-1],DC2_MOTOR_TASK_PRIO);
	OSTaskCreate(TravelMotorTask,(void *)0,(OS_STK *)&travelMotorTaskStk[TRAVEL_MOTOR_STK_SIZE-1],TRAVEL_MOTOR_TASK_PRIO);
	OSTaskCreate(LcdShowTask,(void *)0,(OS_STK *)&lcdShowTaskStk[LCD_SHOW_STK_SIZE-1],LCD_SHOW_TASK_PRIO);
	
	IwdgInit(4,1500);                 //IWDG�ĳ�ʱ����Ϊ3S
	OSTaskSuspend(START_TASK_PRIO);	  //������ʼ����.
	OS_EXIT_CRITICAL();				  //�˳��ٽ���(���Ա��жϴ��)
} 



/*************************************************************
*Function Name  : 
*Auther         : 
*Vertion        : 
*Date           : 
*Description    :  
*Input          :
*Output         :
*Return         : 
*************************************************************/
/*************************************************************
*Function Name  : WifiRevTask
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-20
*Description    : wifi���ݽ�������
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void WifiRevTask(void *pdata)
{
	pdata =  pdata;
	
	while(1)
	{
		OSTimeDly(OS_TICKS_PER_SEC*3);
	}		
} 


/*************************************************************
*Function Name  : WifiTrmTask
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-20
*Description    : wifi���ݷ�������
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void WifiTrmTask(void *pdata)
{
	pdata =  pdata;
		
	
	u8 errWifiTrmTask;
	char HTTP_Buf[100];        //HTTP���Ļ�����
	char charBuff[8] = {0};    //wifi�����ϴ�������
	u8 len;
	ESP8266_Con_Udp(wifiConnectState);
	
	while(1)
	{
		OSSemPend(semWifiData, OS_TICKS_PER_SEC*10, &errWifiTrmTask);
		memset(&Wifi_Data_Buf, 0, sizeof(Wifi_Data_Buf));//��սṹ��Wifi_Data_Buf
		
		/******������ʪ��******/
		sprintf(charBuff, "%d", temperature);
		len = HTTP_PostPkt(HTTP_Buf, API_KEY, DEV_ID, "temp", charBuff);
		USART3_Send_Data((u8 *)HTTP_Buf, len);
		memset(HTTP_Buf, 0, sizeof(HTTP_Buf));           //���HTTP_Buf
		sprintf(charBuff, "%d", humidity);
		len = HTTP_PostPkt(HTTP_Buf, API_KEY, DEV_ID, "humi", charBuff);
		USART3_Send_Data((u8 *)HTTP_Buf, len);
		memset(HTTP_Buf, 0, sizeof(HTTP_Buf));           //���HTTP_Buf
		
		
		/******���͵�һ���ѷ���������******/
		sprintf(charBuff, "%d", firstLevelNum);
		len = HTTP_PostPkt(HTTP_Buf, API_KEY, DEV_ID, "firstLevelNum", charBuff);
		USART3_Send_Data((u8 *)HTTP_Buf, len);
		memset(HTTP_Buf, 0, sizeof(HTTP_Buf));           //���HTTP_Buf
		
		/******���͵ڶ����ѷ���������******/
		sprintf(charBuff, "%d", secondLevelNum);
		len = HTTP_PostPkt(HTTP_Buf, API_KEY, DEV_ID, "secondLevelNum", charBuff);
		USART3_Send_Data((u8 *)HTTP_Buf, len);
		memset(HTTP_Buf, 0, sizeof(HTTP_Buf));           //���HTTP_Buf
		
		/******��Ⲣ���͵�һ���Ƿ�ȱ����־******/
		if (FIRST_FLOOR == 0)                            //��һ�㲻ȱ��
		{
			len = HTTP_PostPkt(HTTP_Buf, API_KEY, DEV_ID, "FirstWarn", (char*)"0");
		}
		else                                             //��һ��ȱ��
		{
			len = HTTP_PostPkt(HTTP_Buf, API_KEY, DEV_ID, "FirstWarn", (char*)"1");
		}
		USART3_Send_Data((u8 *)HTTP_Buf, len);
		memset(HTTP_Buf, 0, sizeof(HTTP_Buf));           //���HTTP_Buf
		
		/******��Ⲣ���͵ڶ����Ƿ�ȱ����־******/
		if (SECOND_FLOOR == 0)                           //�ڶ��㲻ȱ��
		{
			len = HTTP_PostPkt(HTTP_Buf, API_KEY, DEV_ID, "SecondWarn", (char*)"0");
		}
		else                                             //�ڶ���ȱ��
		{
			len = HTTP_PostPkt(HTTP_Buf, API_KEY, DEV_ID, "SecondWarn", (char*)"1");
		}
		USART3_Send_Data((u8 *)HTTP_Buf, len);
		memset(HTTP_Buf, 0, sizeof(HTTP_Buf));           //���HTTP_Buf
		
		OSTimeDly(OS_TICKS_PER_SEC*3);
	}
}


/*************************************************************
*Function Name  : UartTask
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-16
*Description    : Uart1��ʾ����
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void UartTask(void *pdata)
{
	pdata =  pdata;
	
	while(1)
	{
		#if 0
		printf("CPU Usage = %d\r\n", OSCPUUsage);
		printf("OSIdleCtrMax = %d\r\n", OSIdleCtrMax);
		printf("OSIdleCtr = %d\r\n", OSIdleCtr);
		printf("WIFI����״̬ %d\r\n", wifiConnectState);
		printf("OSTaskCtr = %d\r\n", OSTaskCtr);
		#endif

		OSTimeDly(OS_TICKS_PER_SEC*3);
	}
}

/*************************************************************
*Function Name  : DC1MotorTask
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-10-09
*Description    : һ��ֱ������������� 
                  ���𴫵���һ����ĳ�ֽ
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void DC1MotorTask(void *pdata)
{
	INT8U errsemFloorFir;
	pdata = pdata;
	
	while (1)
	{
		/******�ȴ���һ���ֽ�ź�******/
		OSSemPend(semFloorFir, 0, &errsemFloorFir);
		XFS_SendPkt("׼������1��������");
		Motor1SpeedOut(200);    //ռ�ձ�50%
		OSTimeDly(OS_TICKS_PER_SEC*5);
		Motor1SpeedOut(0);      //ռ�ձ�0%
		firstLevelNum++;
		OSSemSet(semFloorFir, 0, &errsemFloorFir);//...���ڴ�����δ�޸���BUG����Ȼ֪��ԭ��
			                                      //...�ź����ᱻ�ᷢ����Σ����������
	}
}


/*************************************************************
*Function Name  : DC2MotorTask
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-10-09
*Description    : ����ֱ������������� 
                  ���𴫵���������ĳ�ֽ
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void DC2MotorTask(void *pdata)
{
	INT8U errsemFloorSec;
	pdata = pdata;
	
	while (1)
	{
		/******�ȴ���һ���ֽ�ź�******/
		OSSemPend(semFloorSec, 0, &errsemFloorSec);
		XFS_SendPkt("׼������2��������");
		Motor2SpeedOut(200);    //ռ�ձ�50%
		OSTimeDly(OS_TICKS_PER_SEC*5);
		Motor2SpeedOut(0);      //ռ�ձ�0%
		secondLevelNum++;
		OSSemSet(semFloorSec, 0, &errsemFloorSec);//...���ڴ�����δ�޸���BUG����Ȼ֪��ԭ��
			                                      //...�ź����ᱻ�ᷢ����Σ����������
	}
}

/*************************************************************
*Function Name  : TravelMotorTask
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-10-08
*Description    : �г̵���������� 
                  �ȴ�װֽ�ź���(�û����¡�PUT��,�򷢳��ź���)
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void TravelMotorTask(void *pdata)
{
	pdata = pdata;
	INT8U errsemPaperPut;
	
	while(1)
	{
		/******�ȴ�װֽֽ�ź�******/
		OSSemPend(semPaperPut, 0, &errsemPaperPut);
		if (putPaperState == 0)                       //��ʼװֽ
		{
			TravelMotorCtrl(MOTOR_FORWARD);
			Led3Switch(ON);
			OSTimeDly(OS_TICKS_PER_SEC*2.5);
			TravelMotorCtrl(MOTOR_STOP);
			Led3Switch(OFF);
			putPaperState = 1;
			OSSemSet(semPaperPut, 0, &errsemPaperPut);//...���ڴ�����δ�޸���BUG����Ȼ֪��ԭ��
			                                          //...�ź����ᱻ�ᷢ����Σ����������
			XFS_SendPkt("���������󵥵��OK");
		}  
		else				                          //װֽ���
		{
			TravelMotorCtrl(MOTOR_BACKWARD);
			Led3Switch(ON);
			OSTimeDly(OS_TICKS_PER_SEC*2.5);
			TravelMotorCtrl(MOTOR_STOP);
			Led3Switch(OFF);
			putPaperState = 0;
			OSSemSet(semPaperPut, 0, &errsemPaperPut);//...���ڴ�����δ�޸���BUG����Ȼ֪��ԭ��
			                                          //...�ź����ᱻ�ᷢ����Σ����������
		}
	}
}


/*************************************************************
*Function Name  : LcdShowTask
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-01-14
*Description    : ����ʾ������ʾ LED2��״̬�Ͱ�����״̬��
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void LcdShowTask(void *pdata)
{
	pdata =  pdata; 
	
	DHT11_Init();
	
	OSTimeDly(OS_TICKS_PER_SEC);
	while(1)
	{
		DHT11_Read_Data(&temperature, &humidity);  //��ȡ��ʪ��ֵ
		
		/******���ݺ��⴫������������Ļ������********/
		if (INFRADED_PIN == 1)
		{
			LCD_LED = 1;			               //��������	
		}
		else
		{
			LCD_LED = 0;                           //Ϩ�𱳹�	
		}	
		
		/******CPUʹ���������********/
		LCD_ShowString(56, 0, 104, 16, 16, (u8*)"CPU Usage:  %");
		LCD_ShowNum(136, 0, OSCPUUsage, 2, 16);	
		
	    /******WIFI��TCP�����������ʾ����********/
		LCD_DrawRectangle(10, 30, 180, 90);
		LCD_ShowString(43, 33, 56, 16, 16, (u8*)"Status:");
		
		if (wifiConnectState == WIFI_CONNECT)
		{
			LCD_ShowString(15, 52, 160, 16, 16, (u8*)"WIFI CONNECTED  ");  
		}
		else if(wifiConnectState == WIFI_NOT_CONNECT)
		{
			LCD_ShowString(15, 52, 160, 16, 16, (u8*)"WIFI NOT CONNECT");  
		}
		else
		{
			LCD_ShowString(15, 52, 160, 16, 16, (u8*)"reconnect wifi......");
		}
		
		if (TCP_ConnectState == TCP_CONNECT)
		{
			LCD_ShowString(15, 71, 160, 16, 16, (u8*)"TCP CONNECT OK!");  
		}
		else
		{
			LCD_ShowString(15, 71, 160, 16, 16, (u8*)"TCP NOT CONNECT!");  
		}
		
		/******��ʪ�Ƚ���********/
		LCD_ShowString(10, 100, 72, 16, 16, (u8*)"Temp:   C");	
        LCD_Draw_Circle(71, 104, 2);		
 	    LCD_ShowString(90, 100, 64, 16, 16, (u8*)"Humi:  %");
		LCD_ShowNum(50, 100, temperature, 2, 16);			   		   
		LCD_ShowNum(130, 100, humidity, 2, 16);				 	  
		
		/******ֱ��������ƽ���********/
		LCD_DrawRectangle(20, 120, 90, 150);
		LCD_ShowString(25, 123, 60, 24, 24, (u8*)"First ");
		LCD_DrawRectangle(120, 120, 202, 150);
		LCD_ShowString(125, 123, 72, 24, 24, (u8*)"Second");
		
		/******�г̵�����ƽ���********/
		LCD_DrawRectangle(20, 180, 66, 210);
		if (putPaperState == 0)    //��ʾ��PUT������
		{
			LCD_ShowString(25, 183, 48, 24, 24, (u8*)"PUT");
		}
		else                       //��ʾ��OK������ 
		{
			LCD_ShowString(25, 183, 48, 24, 24, (u8*)"OK ");
		}
		
		OSTimeDly(100);  //��ʱ100��Ticks = 200ms 
	}
}


/*************************************************************
*Function Name  : TP_Task
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-04-20
*Description    : TP����������
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void TP_Task(void *pdata)
{
	pdata =  pdata;
	u8 errTP_Task;
	
	/*********************��������ʼ��*********************/
    tp_dev.init();		  //��������ʼ��
	POINT_COLOR=RED;      //��������Ϊ��ɫ 
	if (tp_dev.touchtype != 0xFF)
	{
		LCD_ShowString(30, 130 ,200, 16, 16, (u8*)"Press KEY0 to Adjust");
	}
	Load_Drow_Dialog();   //�����Ļ�������Ͻ���ʾ"CLR"
	/******************************************************/
	PaperNumGetData();    //��FLASH�����ȡֽ����������
	
	while(1)
	{
		OSFlagPend(flagTP, 0x01, OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 0, &errTP_Task);
		tp_dev.scan(0);
		if (tp_dev.x[0]>(lcddev.width-24) && tp_dev.y[0]<16)
		{
			Load_Drow_Dialog();     //���
			OSFlagPost(flagTP, 0x04, OS_FLAG_SET, &errTP_Task);
		}
		else if (tp_dev.x[0]>20 && tp_dev.x[0]<68 && tp_dev.y[0]>180 && tp_dev.y[0]<210)
		{
			OSSemPost(semPaperPut);//����װֽ�ź���
		}
		else if (tp_dev.x[0]>20 && tp_dev.x[0]<90 && tp_dev.y[0]>120 && tp_dev.y[0]<150)
		{
			OSSemPost(semFloorFir);//���͵�һ���ֽ�ź���
		}
		else if (tp_dev.x[0]>120 && tp_dev.x[0]<202 && tp_dev.y[0]>120 && tp_dev.y[0]<150)
		{
			OSSemPost(semFloorSec);//���͵ڶ����ֽ�ź���
		}
		else
		{
			TP_Draw_Big_Point(tp_dev.x[0], tp_dev.y[0], RED);
		}
	}
}
/*******************************����ݸ���*******************************/
#if 0

if(GET_LED2_STATUS() == LED_STATUS_ON)
{
	LCD_ShowString(68,40,8,16,16,(u8*)" ");  
	LCD_ShowString(20,40,56,16,16,(u8*)"LED:ON");  
}
else
{
	LCD_ShowString(20,40,56,16,16,(u8*)"LED:OFF");  
}

		if(GET_KEY0_STATUS() == KEY0_ON)
{
	LCD_ShowString(76,64,8,16,16,(u8*)" ");  
	LCD_ShowString(20,64,64,16,16,(u8*)"KEYO:ON");  
}
else
{
	LCD_ShowString(20,64,64,16,16,(u8*)"KEYO:OFF");  
}

		/******������ƽ���********/
		LCD_ShowString(0,150,56,16,16,(u8*)"MOTOR1:"); 
		LCD_ShowString(64,150,56,16,16,(u8*)"Forward"); 
        LCD_ShowString(130,150,32,16,16,(u8*)"Stop");		
		LCD_ShowString(172,150,64,16,16,(u8*)"Backward");

		LCD_ShowString(0,190,56,16,16,(u8*)"MOTOR2:"); 
		LCD_ShowString(64,190,56,16,16,(u8*)"Forward"); 
        LCD_ShowString(130,190,32,16,16,(u8*)"Stop");		
		LCD_ShowString(172,190,64,16,16,(u8*)"Backward");



if (tp_dev.x[0]>(lcddev.width-24) && tp_dev.y[0]<16)
		{
			Load_Drow_Dialog();     //���
			OSFlagPost(flagTP, 0x04, OS_FLAG_SET, &errTP_Task);
		}
		else if (tp_dev.x[0]>64 && tp_dev.x[0]<120 && tp_dev.y[0]>150 && tp_dev.y[0]<166)
		{
			Led3Switch(ON);
			TravelMotorCtrl(MOTOR_FORWARD);
		}
		else if (tp_dev.x[0]>130 && tp_dev.x[0]<162 && tp_dev.y[0]>150 && tp_dev.y[0]<166)
		{
			Led3Switch(OFF);
			TravelMotorCtrl(MOTOR_STOP);
		}
		else if (tp_dev.x[0]>172 && tp_dev.y[0]>150 && tp_dev.y[0]<166)
		{
			Led3Switch(ON);
			MotorCtrl(MOTOR_BACKWARD);
		}
		else
		{
			TP_Draw_Big_Point(tp_dev.x[0], tp_dev.y[0], RED);
		}
		

		sprintf(charBuff, "%d", testNum);
		
		len = HTTP_PostPkt(HTTP_Buf, API_KEY, DEV_ID, "testNumber", charBuff);
		USART3_Send_Data((u8 *)HTTP_Buf, len);
		memset(HTTP_Buf, 0, sizeof(HTTP_Buf));           //���HTTP_Buf
		
		len = HTTP_PostPkt(HTTP_Buf, API_KEY, DEV_ID, "myTemphaha", tempStr);
		USART3_Send_Data((u8 *)HTTP_Buf, len);
		memset(HTTP_Buf, 0, sizeof(HTTP_Buf));           //���HTTP_Buf
		
		len = HTTP_PostPkt(HTTP_Buf, API_KEY, DEV_ID, "myhumi", humiStr);   
		USART3_Send_Data((u8 *)HTTP_Buf, len);
		memset(HTTP_Buf, 0, sizeof(HTTP_Buf));           //���HTTP_Buf
#endif

