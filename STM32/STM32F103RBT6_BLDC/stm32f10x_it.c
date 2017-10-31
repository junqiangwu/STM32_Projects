#include "stdio.h"
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "stm32f10x_it.h"
#include "BLDC.h"
#include "KEY.h"
#include "USART2.h"
#include "PID.h"
#include "main.h"

uint16_t Speed_count;
u8 duzhuan_value;		   //堵转计数器
extern u32 feedback_value;

void TIM1_TRG_COM_IRQHandler(void)
{ 
   TIM1->SR&=0;           //清中断标志 
 //  Get_Adc_Average();	  
   // uprintf(USART1,"feedback %d\n",feedback_value);   
}
extern u8 ERROR_flag;
u16 duzhuan;
extern u8 frequent_1;
void TIM3_IRQHandler(void)
{ u8 j;
  static uint8_t time_over  = 0; 	//定时器溢出计数
  static u8 enc_j;
  static u32 sp_sum;
  static u32 data[6]; 
  
  if(TIM3->SR&0x0010)       		//产生比较/捕获事件４中断计数器溢出  
  {
     
	TIM3->SR&=0xffef;	    		//清中断标志
	if(time_over>2)
	{
	   /*溢出清零　堵转计数*/
	   time_over=0;	   		  
	   duzhuan_value++;       	//stalling_count 堵转计数*/	  
	}
	time_over++;       
//  if(frequent_1!=0&&state==RUN&&duzhuan_value>3){
//      state=FAULT; 
//      ERROR_flag=3;
//      BLDC_Stop();
//      }  
  if(state==CART&&duzhuan_value>4){
      state=FAULT; 
      ERROR_flag=3;
      BLDC_Stop();
      }  
  
  } 
	
if(TIM3->SR&0x0040)      		//TIF触发中断标记   0100 0000  
  { 
    TIM3->SR&=0xffbf;        	//清中断标志
      
  // TIM3->CCR2=TIM3->CCR1>>1;	// 计算换向点   bug触发不了com事件
   huanxiang(); 
   data[enc_j]=TIM3->CCR1+(65535*time_over);
      
   enc_j++;   
  if(enc_j>5)		 
    {
        for(j=0;j<6;j++)
        {
            sp_sum+=data[j];
        }
	   sp_sum/=6;
	   enc_j=0;
	}	
   feedback_value=40000-sp_sum*2;  //输出速度反馈值	  // 除以它 变成一个正比关系
                                  // 速度越快，捕获值越小，倒数即  速度快--返回值大
	time_over = 0;			  	//溢出计数清零
	duzhuan_value = 0;		
  }     		
}





