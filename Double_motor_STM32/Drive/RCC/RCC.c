#include "rcc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_flash.h"


/******
*AHB    1分频   72M    最大72M
*APB1   2分频   36M    最大36M
*APB2   1分频   72M    最大72M
*/
void System_clk_set(void){ 
    ErrorStatus HSEStartUpStatus;
    RCC_DeInit();
 
    RCC_HSEConfig(RCC_HSE_ON );   //打开外部时钟
 
//    RCC_HSEConfig(RCC_HSE_OFF);                           //关闭外部高速震荡器
//	RCC_HSICmd(ENABLE);                                   //使能内部高速震荡器
    
    HSEStartUpStatus = RCC_WaitForHSEStartUp();  //等待外部时钟打开至稳定
 
   if(HSEStartUpStatus == SUCCESS)     
   {
		FLASH_SetLatency(FLASH_Latency_2);   
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);       //flash时钟的相关配置  
		RCC_PLLCmd(DISABLE);  //配置PLL之前需要关闭PLL
		RCC_HCLKConfig(RCC_SYSCLK_Div1);   //HCLK分频
		RCC_PCLK2Config(RCC_HCLK_Div1);   //PCLK2分频
		RCC_PCLK1Config(RCC_HCLK_Div2);    //PCLK1分频
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
		RCC_PLLCmd(ENABLE); //使能PLL
	 
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){      //等待，，直到PLL使能完毕
    }
 
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  //选择PLL时钟为系统时钟
 
    while(RCC_GetSYSCLKSource() != 0x08)       //等待，至PLL时钟设置成系统时钟完毕
       { 
       }
     }

}


void Rcc_init(){
  //  SystemInit();  
	System_clk_set();
    
	RCC_APB2PeriphClockCmd(   RCC_APB2Periph_GPIOA 
							| RCC_APB2Periph_GPIOB
			 	 	 	 	| RCC_APB2Periph_GPIOC 
                            | RCC_APB2Periph_GPIOD 
							| RCC_APB2Periph_AFIO
							| RCC_APB2Periph_TIM1
							| RCC_APB2Periph_ADC1
                            |RCC_APB2Periph_USART1
 							, ENABLE );

 	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3
							| RCC_APB1Periph_USART2 
							| RCC_APB1Periph_TIM4
                            | RCC_APB1Periph_TIM2
							, ENABLE);
    
}
