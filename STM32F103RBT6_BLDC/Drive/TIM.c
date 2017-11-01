#include "stdio.h"
#include "TIM.h"
#include "USART2.h"
#include "ADC.h"
#include "Encode.h"
#include "PID.h"
#include "ADC.h"
#include "BLDC.h"
#include "main.h"

extern u16 Adjust;

extern u16 Exti_speed;
extern u16 Exti_speed_1;
extern u8 frequent;
extern u8 frequent_1;

extern u8 Assist;
extern vu16 I_Thrut[4];  // 电压值
extern u16 feedback_value;
/******标志位********/
u8 AD_flag=0;
u8 Frequent_flag=0;

u32 adcx;
extern u16 KEY_Time;
extern vu16 KEY_UP[3];
extern u8 time_over;
extern u8 Adjust_flag;
extern u32 frequent_sum;

void TIM4_IRQHandler(void)
{ 
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{	
     TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  //清除TIMx更新中断标志  
        
        Ref_Lcd_flag++;  // 刷新屏幕计时标志位
    
      if(Exti_speed<200){
         Exti_speed++;  	   //记录霍尔测速模块两个脉冲之间的差值
        }else{
         Exti_speed_1=0;
        }
       
        Frequent_flag++;
     if(Frequent_flag==50)
        {   
        frequent_1=frequent;
        frequent=0;
        Frequent_flag=0;         
        }
  
       AD_flag++;     //30 ms 采集电流/电压,DMA传送  然后去main里判断
       if(AD_flag>=1)   
       {     
           Get_Adc_Average();          
           AD_flag=0;          
       }
              
       if(KEY_UP[0]==0||KEY_UP[1]==0||KEY_UP[2]==0)
       {
           KEY_Time++;
       }
			
			
       //超流不操作状态，只影响它的输出与否  变换特别快  所以不操作定时器
//     if(Adjust>100)
//    {
//       if(I_Thrut[0]>150||I_Thrut[1]>150||I_Thrut[2]>150)  //50ms采样一次  即停止以后，需要50ms重新启动
//         {  
//           Speed_PIDAdjust(5000);
//             // IR2136_EN_RESET; 
//            // BLDC_Stop();
//         }else{
//           //  IR2136_EN_SET; 
//            // BLDC_Start();   //不影响PWM波输出  只是短暂关闭驱动MOS   马上恢复
//         }  
//    }      
    
	}	    
}



extern u8 Start_flag;
u8 a=0;
void TIM2_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)//是更新中断
	{	 
    //  if(Start_flag!=0){ 
           a++;  // 30S计数一次
         if(a==60)
          {
           TIMER++;    // 分钟为单位 
           a=0;  
           }
    //  }        
      TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx更新中断标志  
	}	       
}



void TIM4_Init(u16 arr,u16 psc)
{	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
  TIM_Cmd(TIM4, ENABLE);  //使能TIMx		
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM4中断,允许更新中断 	  
}



void TIM2_Init(u16 arr,u16 psc)
{	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 
  TIM_Cmd(TIM2, ENABLE);  
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); 	  
}
