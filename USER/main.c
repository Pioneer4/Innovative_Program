/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : main.c
*Auther         : 张沁
*Version        : v1.0
*Date           : 2016-11-10
*Description    : 主函数入口，完成UC/OS-II相关项目
*Function List  : 
*History        :
1. Date: 2017-04-23  Auther: 张沁
Modification: 添加esp8266 AT指令 成功连接WIFI及OneNet服务器
2. Date: 2017-05-02  Auther: 张沁
Modification: 添加触摸屏（XPT2046）驱动代码
2. Date: 2017-05-03  Auther: 张沁
Modification: 添加DHT11驱动代码  电机驱动代码 语音模块代码
*************************************************************/
/*
*项目描述:
*基于UCOS操作系统，完成三个任务，
*任务一用于定时的点灯和喂狗
*任务二检测并响应按键
*任务三用于在显示屏上显示任务一和任务二的状态，比如任务中LED的状态，任务二中按键的状态。
*/
#include "myinclude.h"
#include "includes.h"

OS_EVENT *semWifi;    //信号量：接收到ESP8266发送数据
OS_EVENT *semWifiData;//信号量：接收到服务器数据

OS_EVENT *semPaperPut;   //信号量：装纸机构可开始运行
OS_EVENT *semFloorFir;   //信号量：传单机第一层可以开始出纸
OS_EVENT *semFloorSec;   //信号量：装纸机第二层可以开始出纸

/*
事件标志组：触摸屏相关
BIT0: 触摸屏是否开始扫描触摸点 
*/
OS_FLAG_GRP *flagTP; 

u8 wifiConnectState;  //标志：WIFI连接是否成功 
u8 TCP_ConnectState;  //标志：TCP连接是否成功
u8 regProcess;        //标志：是否处理注册包ACK
u8 putPaperState;     //标志：装纸标志  0：开始装纸  1：装纸完成

INT16U firstLevelNum; //第一层已经发出的传单数量
INT16U secondLevelNum;//第二层已经发出的传单数量

INT8U temperature = 0;  	    
INT8U humidity = 0; 

extern WIFI_Data  Wifi_Data_Buf;


/********************************************
任务        ：WifiRevTask  wifi数据接收任务
任务优先级  ：8
任务堆栈大小：128
任务堆栈    ：wifiRevTaskStk[WIFI_REV_STK_SIZE]
*********************************************/
#define WIFI_REV_TASK_PRIO    8
#define WIFI_REV_STK_SIZE  	  128
OS_STK wifiRevTaskStk[WIFI_REV_STK_SIZE];
void WifiRevTask(void *pdata);


/********************************************
任务        ：WifiTrmTask   wifi数据发送任务
任务优先级  ：9
任务堆栈大小：256
任务堆栈    ：wifiTrmTaskStk[WIFI_TRM_STK_SIZE]
*********************************************/
#define WIFI_TRM_TASK_PRIO        9
#define WIFI_TRM_STK_SIZE  	      256            //!128 就不行
OS_STK wifiTrmTaskStk[WIFI_TRM_STK_SIZE];
void WifiTrmTask(void *pdata);


/********************************************
任务        :UartTask     UART1输出任务
任务优先级  :10
任务堆栈大小:128
任务堆栈    :uartTaskStk[KEYO_STK_SIZE]
*********************************************/
#define UART_TASK_PRIO       10  	  
#define UART_STK_SIZE  		 128
OS_STK uartTaskStk[UART_STK_SIZE];
void UartTask(void *pdata);

/********************************************
任务        ：DC1_MotorTask
任务优先级  ：11
任务堆栈大小：128
任务堆栈    ：DC1_MotorTaskStk[DC1_MOTOR_STK_SIZE]
*********************************************/
#define DC1_MOTOR_TASK_PRIO   11
#define DC1_MOTOR_STK_SIZE    128
OS_STK DC1_MotorTaskStk[DC1_MOTOR_STK_SIZE];
void DC1MotorTask(void *pdata);


/********************************************
任务        ：DC2_MotorTask
任务优先级  ：12
任务堆栈大小：128
任务堆栈    ：DC2_MotorTaskStk[DC2_MOTOR_STK_SIZE];
*********************************************/
#define DC2_MOTOR_TASK_PRIO   12
#define DC2_MOTOR_STK_SIZE    128
OS_STK DC2_MotorTaskStk[DC2_MOTOR_STK_SIZE];
void DC2MotorTask(void *pdata);


/********************************************
任务        ：TravelMotorTask
任务优先级  ：13
任务堆栈大小：128
任务堆栈    ：travelMotorTaskStk[TRAVEL_MOTOR_STK_SIZE]
*********************************************/
#define TRAVEL_MOTOR_TASK_PRIO   13
#define TRAVEL_MOTOR_STK_SIZE  	 128
OS_STK travelMotorTaskStk[TRAVEL_MOTOR_STK_SIZE];
void TravelMotorTask(void *pdata);


/********************************************
任务        ：LcdShowTask  LCD显示屏任务
任务优先级  ：14
任务堆栈大小：128
任务堆栈    ：lcdShowTaskStk[LCD_SHOW_STK_SIZE]
*********************************************/
#define LCD_SHOW_TASK_PRIO 14    	  
#define LCD_SHOW_STK_SIZE  128  		    	
OS_STK lcdShowTaskStk[LCD_SHOW_STK_SIZE];
void LcdShowTask(void *pdata);


/********************************************
任务        ：TP_Task    TP触摸屏任务
任务优先级  ：15
任务堆栈大小：256
任务堆栈    ：tpTaskStk[TP_STK_SIZE];
*********************************************/
#define TP_TASK_PRIO      15
#define TP_STK_SIZE  	  128
OS_STK tpTaskStk[TP_STK_SIZE];
void TP_Task(void *pdata);

/********************************************
任务        ：START 进行其它任务设置
任务优先级  ：16
任务堆栈大小：256
任务堆栈    ：startTaskSTK[START_STK_SIZE]
*********************************************/
#define START_TASK_PRIO      16
#define START_STK_SIZE  	 128
OS_STK startTaskSTK[START_STK_SIZE];
void StartTask(void *pdata);


/*************************************************************
*Function Name  : IwdgTmrCallBack
*Auther         : 张沁
*Vertion        : v1.0
*Date           : 2016-12-06
*Description    : 定时器回调函数，用于喂狗和小灯的闪烁
*Input          : 1. void *ptmr
                  2. void *parg
				  没有传递的参数，则无需操作
*Output         :
*Return         :
*************************************************************/
INT8U timeNum = 0;  //时间变量
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
	
	IwdgFeed();  //喂狗
	
	/*每隔1分钟进行一次数据保存*/   
	if (timeNum == 60)            
	{
		timeNum = 0;
		
		/*对FLASH进行写数据时，必须进入临界段，防止中断扰乱时序出错*/
		OS_ENTER_CRITICAL(); 
		PaperNumSaveData();
		OS_EXIT_CRITICAL();
	}
	else
	{
		timeNum++;
	}
	
}


u8 err = 0;  //错误返回区


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	
	delay_init(168);      //初始化延时函数
	LedInit();		      //初始化LED端口 
	UART1_Init(115200);   //初始化串口1
	UART4_Init(9600);     //初始化串口4 用于语音模块通信
	XFS_SendPkt("各位老师好，下面是基于ucos的智能传单机实物演示");
	LCD_Init();	          //初始化LCD	
	KeyInit();   	      //初始化按键
	InfradedInit();       //初始化红外传感器
    TravelMotorInit();    //初始化行程电机
	DcMotorInit();        //初始化直流电机
	
	OSInit();             //初始化uC/OS
	OSTaskCreate(StartTask, (void*)0, (OS_STK *)&startTaskSTK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	OSStart();	
	
	return 0;
}

/*************************************************************
*Function Name  : StartTask
*Auther         : 张沁
*Vertion        : v1.0
*Date           : 2016-12-05
*Description    : 开始任务 该任务用于建立了其它任务
*Input          ：
*Output         ：
*Return         ：
*************************************************************/
void StartTask(void *pdata)
{
	OS_CPU_SR cpu_sr = 0;
	
	pdata = pdata; 
	
	semWifi = OSSemCreate(0);	       //创建信号量	接收到ESP8266发送数据	 
	semWifiData = OSSemCreate(0);      //创建信号量	接收到服务器数据
	wifiConnectState = ESP8266_Init(); //初始化ESP8266所需接口，并获得连接状态
	
	semFloorFir = OSSemCreate(0);      //创建信号量 传单机第一层可以开始出纸
	semFloorSec = OSSemCreate(0);      //创建信号量 装纸机第二层可以开始出纸
	semPaperPut = OSSemCreate(0);      //创建信号量 装纸机构可开始运行
	
	flagTP = OSFlagCreate(0, &err);    //创建事件标志组
	
	OSStatInit();					   //初始化统计任务.这里会延时1秒钟左右
	
	OS_ENTER_CRITICAL();		       //进入临界区(无法被中断打断)    
	
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
	
	IwdgInit(4,1500);                 //IWDG的超时周期为3S
	OSTaskSuspend(START_TASK_PRIO);	  //挂起起始任务.
	OS_EXIT_CRITICAL();				  //退出临界区(可以被中断打断)
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
*Auther         : 张沁
*Vertion        : v1.0
*Date           : 2017-04-20
*Description    : wifi数据接收任务
*Input          ：
*Output         ：
*Return         ：
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
*Auther         : 张沁
*Vertion        : v1.0
*Date           : 2017-04-20
*Description    : wifi数据发送任务
*Input          ：
*Output         ：
*Return         ：
*************************************************************/
void WifiTrmTask(void *pdata)
{
	pdata =  pdata;
		
	
	u8 errWifiTrmTask;
	char HTTP_Buf[100];        //HTTP报文缓存区
	char charBuff[8] = {0};    //wifi数据上传缓冲区
	u8 len;
	ESP8266_Con_Udp(wifiConnectState);
	
	while(1)
	{
		OSSemPend(semWifiData, OS_TICKS_PER_SEC*10, &errWifiTrmTask);
		memset(&Wifi_Data_Buf, 0, sizeof(Wifi_Data_Buf));//清空结构体Wifi_Data_Buf
		
		/******发送温湿度******/
		sprintf(charBuff, "%d", temperature);
		len = HTTP_PostPkt(HTTP_Buf, API_KEY, DEV_ID, "temp", charBuff);
		USART3_Send_Data((u8 *)HTTP_Buf, len);
		memset(HTTP_Buf, 0, sizeof(HTTP_Buf));           //清空HTTP_Buf
		sprintf(charBuff, "%d", humidity);
		len = HTTP_PostPkt(HTTP_Buf, API_KEY, DEV_ID, "humi", charBuff);
		USART3_Send_Data((u8 *)HTTP_Buf, len);
		memset(HTTP_Buf, 0, sizeof(HTTP_Buf));           //清空HTTP_Buf
		
		
		/******发送第一层已发传单数量******/
		sprintf(charBuff, "%d", firstLevelNum);
		len = HTTP_PostPkt(HTTP_Buf, API_KEY, DEV_ID, "firstLevelNum", charBuff);
		USART3_Send_Data((u8 *)HTTP_Buf, len);
		memset(HTTP_Buf, 0, sizeof(HTTP_Buf));           //清空HTTP_Buf
		
		/******发送第二层已发传单数量******/
		sprintf(charBuff, "%d", secondLevelNum);
		len = HTTP_PostPkt(HTTP_Buf, API_KEY, DEV_ID, "secondLevelNum", charBuff);
		USART3_Send_Data((u8 *)HTTP_Buf, len);
		memset(HTTP_Buf, 0, sizeof(HTTP_Buf));           //清空HTTP_Buf
		
		/******检测并发送第一层是否缺单标志******/
		if (FIRST_FLOOR == 0)                            //第一层不缺单
		{
			len = HTTP_PostPkt(HTTP_Buf, API_KEY, DEV_ID, "FirstWarn", (char*)"0");
		}
		else                                             //第一层缺单
		{
			len = HTTP_PostPkt(HTTP_Buf, API_KEY, DEV_ID, "FirstWarn", (char*)"1");
		}
		USART3_Send_Data((u8 *)HTTP_Buf, len);
		memset(HTTP_Buf, 0, sizeof(HTTP_Buf));           //清空HTTP_Buf
		
		/******检测并发送第二层是否缺单标志******/
		if (SECOND_FLOOR == 0)                           //第二层不缺单
		{
			len = HTTP_PostPkt(HTTP_Buf, API_KEY, DEV_ID, "SecondWarn", (char*)"0");
		}
		else                                             //第二层缺单
		{
			len = HTTP_PostPkt(HTTP_Buf, API_KEY, DEV_ID, "SecondWarn", (char*)"1");
		}
		USART3_Send_Data((u8 *)HTTP_Buf, len);
		memset(HTTP_Buf, 0, sizeof(HTTP_Buf));           //清空HTTP_Buf
		
		OSTimeDly(OS_TICKS_PER_SEC*3);
	}
}


/*************************************************************
*Function Name  : UartTask
*Auther         : 张沁
*Vertion        : v1.0
*Date           : 2017-04-16
*Description    : Uart1显示任务
*Input          ：
*Output         ：
*Return         ：
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
		printf("WIFI连接状态 %d\r\n", wifiConnectState);
		printf("OSTaskCtr = %d\r\n", OSTaskCtr);
		#endif

		OSTimeDly(OS_TICKS_PER_SEC*3);
	}
}

/*************************************************************
*Function Name  : DC1MotorTask
*Auther         : 张沁
*Vertion        : v1.0
*Date           : 2017-10-09
*Description    : 一号直流电机驱动任务 
                  负责传单机一号箱的出纸
*Input          ：
*Output         ：
*Return         ：
*************************************************************/
void DC1MotorTask(void *pdata)
{
	INT8U errsemFloorFir;
	pdata = pdata;
	
	while (1)
	{
		/******等待第一层出纸信号******/
		OSSemPend(semFloorFir, 0, &errsemFloorFir);
		XFS_SendPkt("准备发放1号宣传单");
		Motor1SpeedOut(200);    //占空比50%
		OSTimeDly(OS_TICKS_PER_SEC*5);
		Motor1SpeedOut(0);      //占空比0%
		firstLevelNum++;
		OSSemSet(semFloorFir, 0, &errsemFloorFir);//...由于触摸屏未修复的BUG（虽然知道原因）
			                                      //...信号量会被会发出多次，在这里清除
	}
}


/*************************************************************
*Function Name  : DC2MotorTask
*Auther         : 张沁
*Vertion        : v1.0
*Date           : 2017-10-09
*Description    : 二号直流电机驱动任务 
                  负责传单机二号箱的出纸
*Input          ：
*Output         ：
*Return         ：
*************************************************************/
void DC2MotorTask(void *pdata)
{
	INT8U errsemFloorSec;
	pdata = pdata;
	
	while (1)
	{
		/******等待第一层出纸信号******/
		OSSemPend(semFloorSec, 0, &errsemFloorSec);
		XFS_SendPkt("准备发放2号宣传单");
		Motor2SpeedOut(200);    //占空比50%
		OSTimeDly(OS_TICKS_PER_SEC*5);
		Motor2SpeedOut(0);      //占空比0%
		secondLevelNum++;
		OSSemSet(semFloorSec, 0, &errsemFloorSec);//...由于触摸屏未修复的BUG（虽然知道原因）
			                                      //...信号量会被会发出多次，在这里清除
	}
}

/*************************************************************
*Function Name  : TravelMotorTask
*Auther         : 张沁
*Vertion        : v1.0
*Date           : 2017-10-08
*Description    : 行程电机驱动任务 
                  等待装纸信号量(用户按下“PUT”,则发出信号量)
*Input          ：
*Output         ：
*Return         ：
*************************************************************/
void TravelMotorTask(void *pdata)
{
	pdata = pdata;
	INT8U errsemPaperPut;
	
	while(1)
	{
		/******等待装纸纸信号******/
		OSSemPend(semPaperPut, 0, &errsemPaperPut);
		if (putPaperState == 0)                       //开始装纸
		{
			TravelMotorCtrl(MOTOR_FORWARD);
			Led3Switch(ON);
			OSTimeDly(OS_TICKS_PER_SEC*2.5);
			TravelMotorCtrl(MOTOR_STOP);
			Led3Switch(OFF);
			putPaperState = 1;
			OSSemSet(semPaperPut, 0, &errsemPaperPut);//...由于触摸屏未修复的BUG（虽然知道原因）
			                                          //...信号量会被会发出多次，在这里清除
			XFS_SendPkt("放入宣传后单点击OK");
		}  
		else				                          //装纸完成
		{
			TravelMotorCtrl(MOTOR_BACKWARD);
			Led3Switch(ON);
			OSTimeDly(OS_TICKS_PER_SEC*2.5);
			TravelMotorCtrl(MOTOR_STOP);
			Led3Switch(OFF);
			putPaperState = 0;
			OSSemSet(semPaperPut, 0, &errsemPaperPut);//...由于触摸屏未修复的BUG（虽然知道原因）
			                                          //...信号量会被会发出多次，在这里清除
		}
	}
}


/*************************************************************
*Function Name  : LcdShowTask
*Auther         : 张沁
*Vertion        : v1.0
*Date           : 2017-01-14
*Description    : 在显示屏上显示 LED2的状态和按键的状态。
*Input          ：
*Output         ：
*Return         ：
*************************************************************/
void LcdShowTask(void *pdata)
{
	pdata =  pdata; 
	
	DHT11_Init();
	
	OSTimeDly(OS_TICKS_PER_SEC);
	while(1)
	{
		DHT11_Read_Data(&temperature, &humidity);  //读取温湿度值
		
		/******根据红外传感器来控制屏幕的亮灭********/
		if (INFRADED_PIN == 1)
		{
			LCD_LED = 1;			               //点亮背光	
		}
		else
		{
			LCD_LED = 0;                           //熄灭背光	
		}	
		
		/******CPU使用情况界面********/
		LCD_ShowString(56, 0, 104, 16, 16, (u8*)"CPU Usage:  %");
		LCD_ShowNum(136, 0, OSCPUUsage, 2, 16);	
		
	    /******WIFI和TCP连接情况的显示界面********/
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
		
		/******温湿度界面********/
		LCD_ShowString(10, 100, 72, 16, 16, (u8*)"Temp:   C");	
        LCD_Draw_Circle(71, 104, 2);		
 	    LCD_ShowString(90, 100, 64, 16, 16, (u8*)"Humi:  %");
		LCD_ShowNum(50, 100, temperature, 2, 16);			   		   
		LCD_ShowNum(130, 100, humidity, 2, 16);				 	  
		
		/******直流电机控制界面********/
		LCD_DrawRectangle(20, 120, 90, 150);
		LCD_ShowString(25, 123, 60, 24, 24, (u8*)"First ");
		LCD_DrawRectangle(120, 120, 202, 150);
		LCD_ShowString(125, 123, 72, 24, 24, (u8*)"Second");
		
		/******行程电机控制界面********/
		LCD_DrawRectangle(20, 180, 66, 210);
		if (putPaperState == 0)    //显示“PUT”字样
		{
			LCD_ShowString(25, 183, 48, 24, 24, (u8*)"PUT");
		}
		else                       //显示“OK”字样 
		{
			LCD_ShowString(25, 183, 48, 24, 24, (u8*)"OK ");
		}
		
		OSTimeDly(100);  //延时100个Ticks = 200ms 
	}
}


/*************************************************************
*Function Name  : TP_Task
*Auther         : 张沁
*Vertion        : v1.0
*Date           : 2017-04-20
*Description    : TP触摸屏任务
*Input          ：
*Output         ：
*Return         ：
*************************************************************/
void TP_Task(void *pdata)
{
	pdata =  pdata;
	u8 errTP_Task;
	
	/*********************触摸屏初始化*********************/
    tp_dev.init();		  //触摸屏初始化
	POINT_COLOR=RED;      //设置字体为红色 
	if (tp_dev.touchtype != 0xFF)
	{
		LCD_ShowString(30, 130 ,200, 16, 16, (u8*)"Press KEY0 to Adjust");
	}
	Load_Drow_Dialog();   //清空屏幕并在右上角显示"CLR"
	/******************************************************/
	PaperNumGetData();    //从FLASH里面读取纸张数量数据
	
	while(1)
	{
		OSFlagPend(flagTP, 0x01, OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 0, &errTP_Task);
		tp_dev.scan(0);
		if (tp_dev.x[0]>(lcddev.width-24) && tp_dev.y[0]<16)
		{
			Load_Drow_Dialog();     //清除
			OSFlagPost(flagTP, 0x04, OS_FLAG_SET, &errTP_Task);
		}
		else if (tp_dev.x[0]>20 && tp_dev.x[0]<68 && tp_dev.y[0]>180 && tp_dev.y[0]<210)
		{
			OSSemPost(semPaperPut);//发送装纸信号量
		}
		else if (tp_dev.x[0]>20 && tp_dev.x[0]<90 && tp_dev.y[0]>120 && tp_dev.y[0]<150)
		{
			OSSemPost(semFloorFir);//发送第一层出纸信号量
		}
		else if (tp_dev.x[0]>120 && tp_dev.x[0]<202 && tp_dev.y[0]>120 && tp_dev.y[0]<150)
		{
			OSSemPost(semFloorSec);//发送第二层出纸信号量
		}
		else
		{
			TP_Draw_Big_Point(tp_dev.x[0], tp_dev.y[0], RED);
		}
	}
}
/*******************************代码草稿区*******************************/
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

		/******电机控制界面********/
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
			Load_Drow_Dialog();     //清除
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
		memset(HTTP_Buf, 0, sizeof(HTTP_Buf));           //清空HTTP_Buf
		
		len = HTTP_PostPkt(HTTP_Buf, API_KEY, DEV_ID, "myTemphaha", tempStr);
		USART3_Send_Data((u8 *)HTTP_Buf, len);
		memset(HTTP_Buf, 0, sizeof(HTTP_Buf));           //清空HTTP_Buf
		
		len = HTTP_PostPkt(HTTP_Buf, API_KEY, DEV_ID, "myhumi", humiStr);   
		USART3_Send_Data((u8 *)HTTP_Buf, len);
		memset(HTTP_Buf, 0, sizeof(HTTP_Buf));           //清空HTTP_Buf
#endif

