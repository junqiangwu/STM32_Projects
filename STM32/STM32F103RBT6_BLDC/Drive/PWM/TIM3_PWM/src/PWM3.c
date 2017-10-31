/**
  *************************************************************************
  *@FileName
  *@Author
  *@Version
  *@Date
  *@History
  *@Dependence
  *@Description
  *************************************************************************
  *@CopyRight
  *************************************************************************
  *
	*/
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
void PWM3_GPIOConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  GPIO_InitStruct.GPIO_Pin 		= GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_AF_PP; 
  GPIO_InitStruct.GPIO_Speed 	= GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
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
/*void  TIM3_Config(void)
{
   TIM_TimeBaseInitTypeDef TIM_InitStruct;
 
   TIM_DeInit( TIM3);
   TIM_InternalClockConfig( TIM3);

   TIM_InitStruct.TIM_Prescaler 	=  72;
   TIM_InitStruct.TIM_CounterMode 	=  TIM_CounterMode_Up;
   TIM_InitStruct.TIM_Period		=  492;
   TIM_InitStruct.TIM_ClockDivision =  TIM_CKD_DIV1;

   TIM_TimeBaseInit (TIM3, &TIM_InitStruct);

//   TIM_ClearFlag( TIM2, TIM_FLAG_Update);
   
//   TIM_ITConfig( TIM2, TIM_IT_Update, ENABLE); 
   
//   TIM_Cmd( TIM2,ENABLE);
} */
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
void PWM3_Config(void)
{
   TIM_OCInitTypeDef TIM_OCInitStruct;
   TIM_TimeBaseInitTypeDef TIM_InitStruct;

   TIM_DeInit( TIM3);
   TIM_InternalClockConfig( TIM3);

   TIM_InitStruct.TIM_Prescaler 	=  72;
   TIM_InitStruct.TIM_CounterMode 	=  TIM_CounterMode_Up;
   TIM_InitStruct.TIM_Period		=  492;
   TIM_InitStruct.TIM_ClockDivision =  TIM_CKD_DIV1;

   TIM_TimeBaseInit (TIM3, &TIM_InitStruct);
//   TIM3_Config();
   TIM_OCInitStruct.TIM_OCMode 		= TIM_OCMode_PWM1;
   TIM_OCInitStruct.TIM_Pulse 		= 360;
   TIM_OCInitStruct.TIM_OCPolarity 	= TIM_OCPolarity_High;//比较后输出低
   TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    
   TIM_OC1Init(TIM3, &TIM_OCInitStruct);
   TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);//使能预装载器

   TIM_OC2Init(TIM3, &TIM_OCInitStruct);
   TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);//使能预装载器

   TIM_Cmd( TIM3,ENABLE);
   TIM_CtrlPWMOutputs(TIM3, ENABLE);
}
/***************************************************
* Descirbe  :
* Input     :
* Output    :
* Attention :
* author 	:
***************************************************/
void PWM3_OC1Start(void)
{
   TIM_CCxCmd( TIM3, TIM_Channel_1, TIM_CCx_Enable);
}
/***************************************************
* Descirbe  :
* Input     :
* Output    :
* Attention :
* author 	:
***************************************************/
void PWM3_OC2Start(void)
{
   TIM_CCxCmd( TIM3, TIM_Channel_2, TIM_CCx_Enable);
}
/***************************************************
* Descirbe  :
* Input     :
* Output    :
* Attention :
* author 	:
***************************************************/
void PWM3_OC1Stop(void)
{
   TIM_CCxCmd( TIM3, TIM_Channel_1, TIM_CCx_Disable);
}
/***************************************************
* Descirbe  :
* Input     :
* Output    :
* Attention :
* author 	:
***************************************************/
void PWM3_OC2Stop(void)
{
   TIM_CCxCmd( TIM3, TIM_Channel_2, TIM_CCx_Disable);
}
/***************************************************
* Descirbe  :
* Input     :
* Output    :
* Attention :
* author 	:
***************************************************/
void SetOC1Pulse(uint16_t pulse)
{
   TIM_SetCompare1(TIM3, pulse);
}
/***************************************************
* Descirbe  :
* Input     :
* Output    :
* Attention :
* author 	:
***************************************************/
void SetOC2Pulse(uint16_t pulse)
{
   TIM_SetCompare2(TIM3, pulse);
}
/***************************************************
* Descirbe  :
* Input     :
* Output    :
* Attention :
* author 	:
***************************************************/
