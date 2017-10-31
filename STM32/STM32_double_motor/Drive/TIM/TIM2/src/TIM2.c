#include "includes.h"


/**********************************************************************
* Function Name  : TIM2_RCCConfig
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
**********************************************************************/
void TIM2_RCCConfig(void)
{
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2 , ENABLE);	
}
/**********************************************************************
* Function Name  : TIM2_NVICConfig
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
**********************************************************************/
void TIM2_NVICConfig(void)
{
  NVIC_InitTypeDef NVIC_InitStruct;
  	
  NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0; 
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
}
/**********************************************************************
* Function Name  : TIM2_CapGPIOConfig
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
**********************************************************************/
void TIM2_CapGPIOConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Pin   	= 	GPIO_Pin_0 |GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Speed	=   GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode	=   GPIO_Mode_IN_FLOATING;
	GPIO_Init(	GPIOA, &GPIO_InitStruct);
}
/**********************************************************************
* Function Name  : TIM2_RCCConfig
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
**********************************************************************/
void  TIM2_Config(void)
{
   TIM_TimeBaseInitTypeDef TIM_InitStruct;
 
   TIM_DeInit( TIM2);
   TIM_InternalClockConfig( TIM2);

   TIM_InitStruct.TIM_Prescaler 	=  7200;
   TIM_InitStruct.TIM_CounterMode 	=  TIM_CounterMode_Up;
   TIM_InitStruct.TIM_Period		=  36000 - 1;
   TIM_InitStruct.TIM_ClockDivision =  TIM_CKD_DIV1;

   TIM_TimeBaseInit (TIM2, &TIM_InitStruct);

   TIM_ClearFlag( TIM2, TIM_FLAG_Update);
   
   TIM_ITConfig( TIM2, TIM_IT_Update, ENABLE); 
   
   TIM_Cmd( TIM2,ENABLE);
}
/**********************************************************************
* Function Name  : TIM2_CapConfig
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
**********************************************************************/
void TIM2_CapConfig(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStruct;//定时器初始化结构
  TIM_ICInitTypeDef TIM_ICInitStruct;         //通道输入初始化结构

//TIM2输出初始化
  TIM_TimeBaseStruct.TIM_Period 			= 50000 - 1;     
  TIM_TimeBaseStruct.TIM_Prescaler 			= 7200;       //时钟分频
  TIM_TimeBaseStruct.TIM_ClockDivision 		= TIM_CKD_DIV1;   //时钟分割
  TIM_TimeBaseStruct.TIM_CounterMode 		= TIM_CounterMode_Up;//模式
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStruct);//基本初始化

//TIM2通道1的捕捉初始化  
  TIM_ICInitStruct.TIM_Channel 			= TIM_Channel_1;//通道选择
  TIM_ICInitStruct.TIM_ICPolarity 		= TIM_ICPolarity_Falling;//上升沿
  TIM_ICInitStruct.TIM_ICSelection 		= TIM_ICSelection_DirectTI;//管脚与寄存器对应关系
  TIM_ICInitStruct.TIM_ICPrescaler 		= TIM_ICPSC_DIV1;//分频器
  TIM_ICInitStruct.TIM_ICFilter 		= 0x4;//滤波设置，经历几个周期跳变认定波形稳定0x0～0xF
  TIM_ICInit(TIM2, &TIM_ICInitStruct);        //初始化
//TIM2通道2的捕捉初始化
  TIM_ICInitStruct.TIM_Channel 		= TIM_Channel_4;//通道选择
  TIM_ICInit(TIM2, &TIM_ICInitStruct);        		//初始化

  
  TIM_SelectInputTrigger(TIM2, TIM_TS_TI1FP1);   	//选择时钟触发源
  TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Reset);	//触发方式
  TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Enable); //启动定时器的被动触发

  TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);        //打开中断
  TIM_ITConfig(TIM2, TIM_IT_CC4, ENABLE);        //打开中断
  
  TIM_Cmd(TIM2, ENABLE);                         //启动TIM2
}
/**********************************************************************
* Function Name  : TIM2_Cap1Start
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
**********************************************************************/
void TIM2_Cap1Start(void)
{
	 TIM_CCxCmd( TIM2, TIM_Channel_1, TIM_CCx_Enable);	 
}
/**********************************************************************
* Function Name  : TIM2_Cap1Stop
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
**********************************************************************/
void TIM2_Cap1Stop(void)
{
	TIM_CCxCmd( TIM2, TIM_Channel_1, TIM_CCx_Disable);
} 
/**********************************************************************
* Function Name  : TIM2_Cap4Start
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
**********************************************************************/
void TIM2_Cap4Start(void)
{
	 TIM_CCxCmd( TIM2, TIM_Channel_4, TIM_CCx_Enable);	 
}
/**********************************************************************
* Function Name  : TIM2_Cap4Stop
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
**********************************************************************/
void TIM2_Cap4Stop(void)
{
	TIM_CCxCmd( TIM2, TIM_Channel_4, TIM_CCx_Disable);
}
