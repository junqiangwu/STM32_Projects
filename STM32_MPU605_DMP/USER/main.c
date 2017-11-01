#include "sys.h"	
#include "delay.h"	
#include "key.h"   
#include "mpu6050.h"  
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "USART2.h"
#include "pwm.h"
#include "PID.h"
#include "TIM.h"
#include "KEY.h"
#include "outputdata.h"

u8 RUN_MODE=0;
u8 Print=0;

float pitch,roll,yaw; 		//欧拉角
short aacx,aacy,aacz;		//加速度传感器原始数据
short gyrox,gyroy,gyroz;	//陀螺仪原始数据
Pid_struct motor13_parameter,motor24_parameter;  // 电机pid参数

void NVIC_Config(void);

int main(void)
{ 
	  short temp;					//温度	    
	  
		SystemInit();
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_Config(); 
   	delay_init();	    	 //延时函数初始化	
    PWM_Init(3000,0);    //0分频。PWM频率=72000/(4000*1)=14Khz 
    PWM_Init_2(3000,0);   
    USART1_Config();   
    KEY_Init();    
 
   Speed_PIDInit(&motor13_parameter,30,0.2,100);   // 电机pid 参数初始化
	 Speed_PIDInit(&motor24_parameter,60,0.2,200);
	
 	 MPU_Init();		
   delay_ms(500);   
   while(mpu_dmp_init());   //初始化MPU6050_DMP
     
   TIM4_Init(10000-1,71);   //定时器初始化
   uprintf(USART1," 系统启动成功!\r\n");
	 
while(1)
{               
   //  	 MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
   //    MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据   
  KEY_Scan();
  if(Print==1){   // 发送曲线到上位机
    
     OutData[0]=pitch;
     OutData[1]=roll;
     OutData[2]= motor13_parameter.out_duty/100;         
     OutPut_Data(); 
}  	} 	}


void NVIC_Config(void)
{
         
    
	 NVIC_InitTypeDef NVIC_InitStructure; 					 //复位NVIC寄存器为默认值	
     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);           //设置中断优先级组

	RCC_APB2PeriphClockCmd(   RCC_APB2Periph_GPIOA 
							| RCC_APB2Periph_GPIOB
			 	 	 	 	| RCC_APB2Periph_GPIOC 
         //   | RCC_APB2Periph_GPIOD 
					//	| RCC_APB2Periph_AFIO
							| RCC_APB2Periph_TIM1
			  	//	| RCC_APB2Periph_ADC1
              | RCC_APB2Periph_USART1
 							, ENABLE );

 	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3
							| RCC_APB1Periph_TIM4
              | RCC_APB1Periph_TIM2
							, ENABLE);
      
    
              //复位NVIC寄存器为默认值	  
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;	
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   //抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  	    //响应优先级0
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	            
	  NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0; 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

