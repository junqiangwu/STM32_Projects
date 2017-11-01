#include "stdio.h"
#include "TIM.h"
#include "USART2.h"
#include "PID.h"
#include "math.h"
#include "pwm.h"
#include "mpu6050.h"
#include "stm32f10x.h"
#include "Control_Motor.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 

/******标志位********/

u32 USART_flag=0;

extern u8 RUN_MODE;

float pitch_offest= 2.3;
float roll_offest=-1.8;
 
extern  float pitch,roll,yaw;    //欧拉角
extern 	short aacx,aacy,aacz;		//加速度传感器原始数据
extern 	short gyrox,gyroy,gyroz;	//陀螺仪原始数据
extern Pid_struct motor13_parameter,motor24_parameter;   // 4个空心杯电机的参数

void TIM4_IRQHandler(void)
{ 	
    
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//是更新中断
	{	 
	
    mpu_dmp_get_data(&pitch,&roll,&yaw);  // 通过MPU6050的dmp获得三轴速度
     
    switch(RUN_MODE)    
    {
       case 1:  Test_1();  break;
       case 2:  Test_2();  break;
       case 3:  Test_3();  break;
       case 4:  Test_4();  break;
       case 5:  Test_5();  break;
       case 6:  Test_5();  break;
       default:  break;
    }
           
  //  uprintf(USART1,"USART_flag %d\n",USART_flag);      
      TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //清除TIMx更新中断标志  
	}	   }


void TIM2_Init(u16 arr,u16 psc)
{	  
    
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 
  TIM_ClearFlag(TIM2,TIM_FLAG_Update);
    
  TIM_Cmd(TIM2, ENABLE);  
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); 	  
}

/*
TIM4  定时中断
*/
void TIM4_Init(u16 arr,u16 psc)
{	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure; 	
    
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	
    
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
  TIM_ClearFlag(TIM4,TIM_FLAG_Update);
  TIM_Cmd(TIM4, ENABLE);  //使能TIMx		
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM4中断,允许更新中断 	
}



//void TIM1_init(){
//	GPIO_InitTypeDef GPIO_InitStructure;
//	TIM_TimeBaseInitTypeDef tim_TimeBaseInitTypeDef;
//	TIM_ICInitTypeDef tim_ICInitTypeDef;
//	
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_TIM1, ENABLE);	
//	   
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_8;   //  PA8/9
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	
//	 TIM_DeInit(TIM1);
//	 TIM_TimeBaseStructInit(&tim_TimeBaseInitTypeDef);
//	 
//	 tim_TimeBaseInitTypeDef.TIM_Prescaler = 0x0;  //不分频 
//	 tim_TimeBaseInitTypeDef.TIM_Period =0xffff; 
//	 tim_TimeBaseInitTypeDef.TIM_ClockDivision =TIM_CKD_DIV1;  
//	 tim_TimeBaseInitTypeDef.TIM_CounterMode =TIM_CounterMode_Up; 
//	 TIM_TimeBaseInit(TIM1,&tim_TimeBaseInitTypeDef);
//	 
//	 TIM_EncoderInterfaceConfig(TIM1,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);   //IC1 IC2都计数
//	 TIM_ICStructInit(&tim_ICInitTypeDef);
//	 tim_ICInitTypeDef.TIM_ICFilter = 0x6;
//	 TIM_ICInit(TIM1,&tim_ICInitTypeDef);
//	 
//	 TIM_ClearFlag(TIM1, TIM_FLAG_Update);
//	 TIM_ITConfig(TIM1, TIM_IT_Update,ENABLE);
//	 
//	 TIM1->CNT =6000;
//	 
//	 TIM_Cmd(TIM1, ENABLE); 
//}


//void TIM2_init(){
//	GPIO_InitTypeDef GPIO_InitStructure;
//	TIM_TimeBaseInitTypeDef tim_TimeBaseInitTypeDef;
//	TIM_ICInitTypeDef tim_ICInitTypeDef;
//	
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
//    
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;   //  PA8/9
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	
//	 TIM_DeInit(TIM2);
//	 TIM_TimeBaseStructInit(&tim_TimeBaseInitTypeDef);
//	 
//	 tim_TimeBaseInitTypeDef.TIM_Prescaler = 0x0;  //不分频 
//	 tim_TimeBaseInitTypeDef.TIM_Period =0xffff; 
//	 tim_TimeBaseInitTypeDef.TIM_ClockDivision =TIM_CKD_DIV1;  
//	 tim_TimeBaseInitTypeDef.TIM_CounterMode =TIM_CounterMode_Up; 
//	 TIM_TimeBaseInit(TIM2,&tim_TimeBaseInitTypeDef);
//	 
//	 TIM_EncoderInterfaceConfig(TIM2,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);   //IC1 IC2都计数
//	 TIM_ICStructInit(&tim_ICInitTypeDef);
//	 tim_ICInitTypeDef.TIM_ICFilter = 0x6;
//	 TIM_ICInit(TIM2,&tim_ICInitTypeDef);
//	 
//	 TIM_ClearFlag(TIM2, TIM_FLAG_Update);
//	 TIM_ITConfig(TIM2, TIM_IT_Update,ENABLE);
//	 
//	 TIM2->CNT =6000;
//	 
//	 TIM_Cmd(TIM2, ENABLE); 
//}


