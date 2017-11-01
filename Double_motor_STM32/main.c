#include "main.h"
#include "pwm.h"
#include "SDS.h"
#include "DataScope_DP.h"


u32 SET_SPEED=300;
u8 Key=0;	
Pid_struct Main_pid_parameter,Slave_pid_parameter;

int main(void)
{
    unsigned char i;          //计数变量
    unsigned char Send_Count; //串口需要发送的数据个数
 
 /********中断优先级设置********/    
    Rcc_init();
    NVIC_Config();
 
    delay_init();
    PWM_Init(3999,0);    //0分频。PWM频率=72000/(4000*1)=14Khz 
  
    USART1_Config();
    KEY_Init();    
   
    TIM1_init();
    TIM2_init();   // TIM1/2 编码器
    
    TIM4_Init(4999,71);   // 5ms 编码器计时  
    
    Speed_PIDInit(&Main_pid_parameter);    //PID初始化 
    Speed_PIDInit(&Slave_pid_parameter); 
		
 while(1){
     
//      Send_Count = DataScope_Data_Generate(3); //生成10个通道的 格式化帧数据，返回帧数据长度
//	    for( i = 0 ; i < Send_Count; i++)  //循环发送,直到发送完毕   
//	    {
//	      while((USART1->SR&0X40)==0);  
//  	      USART1->DR = DataScope_OutPut_Buffer[i]; //从串口丢一个字节数据出去      
//	     }
//        
//      delay_ms(5); //20fps, 帧间隔时间。 不同电脑配置及 USB-TTL 设备的优劣均会影响此时间的长短，建议实测为准。  
    
 }   }


void NVIC_Config(void)
{
	 NVIC_InitTypeDef NVIC_InitStructure; 					 //复位NVIC寄存器为默认值	
     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);           //设置中断优先级组
	
	
     NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;   // 10ms 中断
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;
	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		
	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	 NVIC_Init(&NVIC_InitStructure);	
          
      NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0; 
      NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&NVIC_InitStructure);  
}


