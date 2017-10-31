#include "includes.h"
/**
  *@Function		
  *@Description			
  *@Calls			
  *@Call By		
  *@Param	xxx
  *		@arg
  *		@arg
  *@Param	xxx
  *		@arg
  *		@arg
  *@Reture			
  *@Others			
  **/
void PWM4_GPIOConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  //GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Pin 		= GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_AF_PP; 
  GPIO_InitStruct.GPIO_Speed 	= GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
}
/**
  *@Function		
  *@Description	 TIM2		
  *@Calls			
  *@Call By		
  *@Param	xxx
  *		@arg
  *		@arg
  *@Param	xxx
  *		@arg
  *		@arg
  *@Reture			
  *@Others			
  **/
void  TIM4_Config(void)
{
   TIM_TimeBaseInitTypeDef TIM_InitStruct;
 
   TIM_DeInit( TIM4);
   TIM_InternalClockConfig( TIM4);

   TIM_InitStruct.TIM_Prescaler 	=  18 - 1;
   TIM_InitStruct.TIM_CounterMode 	=  TIM_CounterMode_Up;
   TIM_InitStruct.TIM_Period		=  105;
   TIM_InitStruct.TIM_ClockDivision =  TIM_CKD_DIV1;

   TIM_TimeBaseInit (TIM4, &TIM_InitStruct);

//   TIM_ClearFlag( TIM2, TIM_FLAG_Update);
   
//   TIM_ITConfig( TIM2, TIM_IT_Update, ENABLE); 
   
//   TIM_Cmd( TIM2,ENABLE);
}
/**
  *@Function		
  *@Description			
  *@Calls			
  *@Call By		
  *@Param	xxx
  *		@arg
  *		@arg
  *@Param	xxx
  *		@arg
  *		@arg
  *@Reture			
  *@Others			
  **/
void PWM4_Config(void)
{
   TIM_OCInitTypeDef TIM_OCInitStruct;
   TIM4_Config();
   TIM_OCInitStruct.TIM_OCMode 		= TIM_OCMode_PWM1;
   TIM_OCInitStruct.TIM_Pulse 		= 50;
   TIM_OCInitStruct.TIM_OCPolarity 	= TIM_OCPolarity_High;//比较后输出低
   TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    
//   TIM_OC1Init(TIM4, &TIM_OCInitStruct);
//  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);//使能预装载器

//   TIM_OC2Init(TIM4, &TIM_OCInitStruct);
//   TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);//使能预装载器

//  TIM_OCInitStruct.TIM_Pulse 		= 230;
//  TIM_OC3Init(TIM4, &TIM_OCInitStruct);
//   TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);//使能预装载器

   TIM_OC4Init(TIM4, &TIM_OCInitStruct);
   TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);//使能预装载器

   TIM_Cmd( TIM4,ENABLE);
   TIM_CtrlPWMOutputs(TIM4, ENABLE);

//   PWM4_OC1Stop();
//   PWM4_OC2Stop();
//   PWM4_OC3Stop();
   PWM4_OC4Stop();

}
/***************************************************
* Descirbe  :
* Input     :
* Output    :
* Attention :
* author 	:
***************************************************/
void PWM4_OC1Start(void)
{
   TIM_CCxCmd( TIM4, TIM_Channel_1, TIM_CCx_Enable);
}
/***************************************************
* Descirbe  :
* Input     :
* Output    :
* Attention :
* author 	:
***************************************************/
void PWM4_OC2Start(void)
{
   TIM_CCxCmd( TIM4, TIM_Channel_2, TIM_CCx_Enable);
}
/***************************************************
* Descirbe  :
* Input     :
* Output    :
* Attention :
* author 	:
***************************************************/
void PWM4_OC3Start(void)
{
   TIM_CCxCmd( TIM4, TIM_Channel_3, TIM_CCx_Enable);
}
/***************************************************
* Descirbe  :
* Input     :
* Output    :
* Attention :
* author 	:
***************************************************/
void PWM4_OC4Start(void)
{
   TIM_CCxCmd( TIM4, TIM_Channel_4, TIM_CCx_Enable);
}
/***************************************************
* Descirbe  :
* Input     :
* Output    :
* Attention :
* author 	:
***************************************************/
void PWM4_OC1Stop(void)
{
   TIM_CCxCmd( TIM4, TIM_Channel_1, TIM_CCx_Disable);
}
/***************************************************
* Descirbe  :
* Input     :
* Output    :
* Attention :
* author 	:
***************************************************/
void PWM4_OC2Stop(void)
{
   TIM_CCxCmd( TIM4, TIM_Channel_2, TIM_CCx_Disable);
}
/***************************************************
* Descirbe  :
* Input     :
* Output    :
* Attention :
* author 	:
***************************************************/
void PWM4_OC3Stop(void)
{
   TIM_CCxCmd( TIM4, TIM_Channel_3, TIM_CCx_Disable);
}
/***************************************************
* Descirbe  :
* Input     :
* Output    :
* Attention :
* author 	:
***************************************************/
void PWM4_OC4Stop(void)
{
   TIM_CCxCmd( TIM4, TIM_Channel_4, TIM_CCx_Disable);
}
/***************************************************
* Descirbe  :
* Input     :
* Output    :
* Attention :
* author 	:
***************************************************/
void PWM4_SetOC1Pulse(uint16_t pulse)
{
   TIM_SetCompare1(TIM4, pulse);
}
/***************************************************
* Descirbe  :
* Input     :
* Output    :
* Attention :
* author 	:
***************************************************/
void PWM4_SetOC2Pulse(uint16_t pulse)
{
   TIM_SetCompare2(TIM4, pulse);
}
/***************************************************
* Descirbe  :
* Input     :
* Output    :
* Attention :
* author 	:
***************************************************/
void PWM4_SetOC3Pulse(uint16_t pulse)
{
   TIM_SetCompare3(TIM4, pulse);
}
/***************************************************
* Descirbe  :
* Input     :
* Output    :
* Attention :
* author 	:
***************************************************/
void PWM4_SetOC4Pulse(uint16_t pulse)
{
   TIM_SetCompare4(TIM4, pulse);
}
