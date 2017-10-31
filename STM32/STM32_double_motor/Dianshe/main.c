#include "main.h"
#include "pwm.h"

/*********全局变量***********/

u32 SET_SPEED=200;
u8 Key=0;	
u8 Adjust_Mode_Flag;


int main(void)
{
  /***********IO时钟初始化*********************/
    Rcc_init();
  /********中断优先级设置********/
    NVIC_Config();
    
  /**********屏幕初始化*****************/  
    delay_init();
    PWM3_RESET;
    PWM_Init(3999,0);    //0分频。PWM频率=72000/(4000*1)=14Khz 
    
/********串口/AD/定时器/外部中断初始化*************************/ 
    
	 USART1_GPIOConfig();
     USART1_Config();
    
    TIM1_init();
    TIM2_init();   // TIM1/2 编码器
    
    TIM4_Init(4999,71);   // 5ms 编码器计时  
    Motor_Slave_Pid_Init(); 
    Motor_Pid_Init();     //PID初始化  为起步设置一个固定速度
    Adjust_Mode_Flag = SPEED_MODE;  
    
 while(1){
  
   //    USART_Reci();      
   
 }
}
 


/*****************简陋版，需要配套写解析函数*********************/
void USART_Reci(void)
 {
     if(USART1_RX_STA)			//接收到一次数据了
   {
      if(USART1_RX_BUF[0]=='s')
        {  
          SET_SPEED+=100;
        }	
       else if(USART1_RX_BUF[0]=='d')
         {  
            SET_SPEED-=100;            
         }	
       else if(USART1_RX_BUF[0]=='p')
         {  
            Motor1(0); 
            SET_SPEED=0;            
         }	
         
       USART1_RX_STA=0;	
   }	
}		

  


void NVIC_Config(void)
{
	 NVIC_InitTypeDef NVIC_InitStructure; 					 //复位NVIC寄存器为默认值	
     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);           //设置中断优先级组
	
	
     NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;   // 10ms 中断
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;
	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		
	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	 NVIC_Init(&NVIC_InitStructure);	
          
      NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1; 
      NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&NVIC_InitStructure);
   
}


