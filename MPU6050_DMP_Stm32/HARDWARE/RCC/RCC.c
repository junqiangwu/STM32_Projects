#include "rcc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_flash.h"


/******
*AHB    1分频   72M    最大72M
*APB1   2分频   36M    最大36M
*APB2   1分频   72M    最大72M
*/

void RCC_Configuration(void)
{ 
    ErrorStatus HSEStartUpStatus;
  /*将外设RCC寄存器重设为缺省值*/
  RCC_DeInit();
 
  /*设置外部高速晶振（HSE）*/
  RCC_HSEConfig(RCC_HSE_ON);   //RCC_HSE_ON——HSE晶振打开(ON)
 
  /*等待HSE起振*/
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
 
  if(HSEStartUpStatus == SUCCESS)        //SUCCESS：HSE晶振稳定且就绪
  {
    /*设置AHB时钟（HCLK）*/ 
    RCC_HCLKConfig(RCC_SYSCLK_Div1);  //RCC_SYSCLK_Div1——AHB时钟= 系统时钟
 
    /* 设置高速AHB时钟（PCLK2）*/ 
    RCC_PCLK2Config(RCC_HCLK_Div1);   //RCC_HCLK_Div1——APB2时钟= HCLK
 
    /*设置低速AHB时钟（PCLK1）*/    
RCC_PCLK1Config(RCC_HCLK_Div2);   //RCC_HCLK_Div2——APB1时钟= HCLK / 2
 
    /*设置FLASH存储器延时时钟周期数*/
    FLASH_SetLatency(FLASH_Latency_2);    //FLASH_Latency_2  2延时周期
   
 /*选择FLASH预取指缓存的模式*/  
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);       // 预取指缓存使能
 
    /*设置PLL时钟源及倍频系数*/ 
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);     
// PLL的输入时钟= HSE时钟频率；RCC_PLLMul_9——PLL输入时钟x 9
   
  /*使能PLL */
    RCC_PLLCmd(ENABLE); 
 
    /*检查指定的RCC标志位(PLL准备好标志)设置与否*/   
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)      
       {
       }
 
    /*设置系统时钟（SYSCLK）*/ 
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); 
//RCC_SYSCLKSource_PLLCLK——选择PLL作为系统时钟
 
    /* PLL返回用作系统时钟的时钟源*/
    while(RCC_GetSYSCLKSource() != 0x08)        //0x08：PLL作为系统时钟
       { 
       }
     
   }
}
void Rcc_init(){
    
    SystemInit();  
	RCC_Configuration();
    
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
    
}
