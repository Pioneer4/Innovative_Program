/*************************************************************
*  Open source but not allowed to modify(All rights reserved)
*File name      : motor.c
*Auther         : ����
*Version        : v1.0
*Date           : 2017-05-03
*Description    : ���ļ��������йؼ��ٵ�����ŵĳ�ʼ��
*Function List  : 
*************************************************************/

#include "myinclude.h"


/*************************************************************
*Function Name  : TravelMotorInit
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-10-08
*Description    : ��ʼ���г̵��  
                  �˿ڣ�PA4 PA5
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void TravelMotorInit(void)
{
	GPIO_InitTypeDef GPIOA_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_OUT;           //���ģʽ
	GPIOA_InitStructure.GPIO_OType = GPIO_OType_PP;          //�������
	GPIOA_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;  
	GPIOA_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            //����
	GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;      //�ٶ�100M
	
	GPIO_Init(GPIOA, &GPIOA_InitStructure);                 
}

/*************************************************************
*Function Name  : TravelMotorCtrl
*Auther         : ����
*Version        : v1.0
*Date           : 2017-05-03
*Description    : ���ڵ���˶�����
*Input          ��motorType ��
				  @arg MOTOR_FORWARD  ���ǰ��
				  @arg MOTOR_BACKWARD �������
				  @arg MOTOR_STOP     ���ֹͣ
*Output         ��
*Return         ��
*************************************************************/
void TravelMotorCtrl(u8 runType)
{
	switch (runType)
	{
		case MOTOR_FORWARD:
		{
			MOTOR1_PIN1 = 1;
			MOTOR1_PIN2 = 0;
			break;
		}
		case MOTOR_BACKWARD:
		{
			MOTOR1_PIN1 = 0;
			MOTOR1_PIN2 = 1;
			break;
		}
		case MOTOR_STOP:
		{
			MOTOR1_PIN1 = 0;
			MOTOR1_PIN2 = 0;
			break;
		}
		default:
			break;
	}
}

/*************************************************************
*Function Name  : DcMotorInit
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-08-05
*Description    : ��ʼ��ֱ�����  TIM2��������PWM��10KHz��
                  PA0~PA1 �ֱ��Ӧ CH1~CH4
*Input          ��
*Output         ��
*Return         ��
*************************************************************/
void DcMotorInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  	  //TIM2ʱ��ʹ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	  //ʹ��PORTAʱ��	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;              //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	      //�ٶ�100MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);                    //��ʼ��PA0,PA1,PA2,PA3
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM2);   //GPIOA0����Ϊ��ʱ��5
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);   //GPIOA1����Ϊ��ʱ��5
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_TIM2);   //GPIOA2����Ϊ��ʱ��5
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_TIM2);   //GPIOA3����Ϊ��ʱ��5
	
	TIM_TimeBaseStructure.TIM_Prescaler = 21 - 1;             //��ʱ����Ƶ 84M/21 = 4M
	TIM_TimeBaseStructure.TIM_Period = 400 - 1;               //�Զ���װ��ֵ 4M/400 = 10KHz
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OCInitStructure.TIM_Pulse = 0;                        //���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�Ƚ����ʹ��
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);  
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  //ʹ��TIM2��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);  //ʹ��TIM2��CCR2�ϵ�Ԥװ�ؼĴ���
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);  //ʹ��TIM2��CCR3�ϵ�Ԥװ�ؼĴ���
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);  //ʹ��TIM2��CCR4�ϵ�Ԥװ�ؼĴ���
 
    TIM_ARRPreloadConfig(TIM2, ENABLE);//ARPEʹ�� 
	
	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIM2									  
}

/*************************************************************
*Function Name  : Motor1SpeedOut
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-08-05
*Description    : ���1��PWM���
*Input          ��motor1SpeedVal: 0~399  
                  ռ�ձ� = ��motor1SpeedVal+1��/ 400
*Output         ��
*Return         ��
*************************************************************/
void Motor1SpeedOut(u16 motor1SpeedVal)
{
	TIM_SetCompare1(TIM2, motor1SpeedVal);	//�޸ıȽ�ֵ���޸�ռ�ձ�
}

/*************************************************************
*Function Name  : Motor2SpeedOut
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-08-05
*Description    : ���2��PWM���
*Input          ��motor2SpeedVal: 0~399  
                  ռ�ձ� = ��motor2SpeedVal+1��/ 400
*Output         ��
*Return         ��
*************************************************************/
void Motor2SpeedOut(u16 motor2SpeedVal)
{
	TIM_SetCompare2(TIM2, motor2SpeedVal);	//�޸ıȽ�ֵ���޸�ռ�ձ�
}

/*************************************************************
*Function Name  : Motor3SpeedOut
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-08-05
*Description    : ���3��PWM���
*Input          ��motor3SpeedVal: 0~399  
                  ռ�ձ� = ��motor3SpeedVal+1��/ 400
*Output         ��
*Return         ��
*************************************************************/
void Motor3SpeedOut(u16 motor3SpeedVal)
{
	TIM_SetCompare3(TIM2, motor3SpeedVal);	//�޸ıȽ�ֵ���޸�ռ�ձ�
}

/*************************************************************
*Function Name  : Motor4SpeedOut
*Auther         : ����
*Vertion        : v1.0
*Date           : 2017-08-05
*Description    : ���4��PWM���
*Input          ��motor4SpeedVal: 0~399  
                  ռ�ձ� = ��motor4SpeedVal+1��/ 400
*Output         ��
*Return         ��
*************************************************************/
void Motor4SpeedOut(u16 motor4SpeedVal)
{
	TIM_SetCompare4(TIM2, motor4SpeedVal);	//�޸ıȽ�ֵ���޸�ռ�ձ�
}
