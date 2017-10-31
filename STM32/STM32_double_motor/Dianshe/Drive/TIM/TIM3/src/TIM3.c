#include "includes.h"


/**********************************************************************
* Function Name  : TIM2_RCCConfig
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
**********************************************************************/
void TIM3_RCCConfig(void)
{
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3 , ENABLE);	
}
/**********************************************************************
* Function Name  : TIM2_NVICConfig
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
**********************************************************************/
void TIM3_NVICConfig(void)
{
  NVIC_InitTypeDef NVIC_InitStruct;
  	
  NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0; 
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
}
/**********************************************************************
* Function Name  : TIM2_CapGPIOConfig
* Description    : (TIM3_CH1 PA6),(TIM3_CH2 PA7),(TIM3_CH3 PB0),(TIM3_CH4 PB1)
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
**********************************************************************/
void TIM3_CapGPIOConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Pin   	= 	GPIO_Pin_6;
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
void  TIM3_Config(void)
{
   TIM_TimeBaseInitTypeDef TIM_InitStruct;
 
   TIM_DeInit( TIM3);
   TIM_InternalClockConfig( TIM3);

   TIM_InitStruct.TIM_Prescaler 	=  72;
   TIM_InitStruct.TIM_CounterMode 	=  TIM_CounterMode_Up;
   TIM_InitStruct.TIM_Period		=  65535;
   TIM_InitStruct.TIM_ClockDivision =  TIM_CKD_DIV1;

   TIM_TimeBaseInit (TIM3, &TIM_InitStruct);

   TIM_ClearFlag( TIM3, TIM_FLAG_Update);
   
   TIM_ITConfig( TIM3, TIM_IT_Update, ENABLE); 
   
   TIM_Cmd( TIM3,ENABLE);
}
/**********************************************************************
* Function Name  : TIM2_CapConfig
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
**********************************************************************/
void TIM3_CapConfig(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStruct; //定时器初始化结构
  TIM_ICInitTypeDef TIM_ICInitStruct;          //通道输入初始化结构

//TIM3输出初始化
  TIM_TimeBaseStruct.TIM_Period 			= 65535;     
  TIM_TimeBaseStruct.TIM_Prescaler 			= 72 - 1;       		  //时钟分频
  TIM_TimeBaseStruct.TIM_ClockDivision 		= TIM_CKD_DIV1;       //时钟分割
  TIM_TimeBaseStruct.TIM_CounterMode 		= TIM_CounterMode_Up; //模式
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStruct);					  //基本初始化

//TIM2通道1的捕捉初始化  
  TIM_ICInitStruct.TIM_Channel 			= TIM_Channel_1;//通道选择
  TIM_ICInitStruct.TIM_ICPolarity 		= TIM_ICPolarity_Falling;//下降沿
  TIM_ICInitStruct.TIM_ICSelection 		= TIM_ICSelection_DirectTI;//管脚与寄存器对应关系
  TIM_ICInitStruct.TIM_ICPrescaler 		= TIM_ICPSC_DIV1;//分频器
  TIM_ICInitStruct.TIM_ICFilter 		= 0x4;//滤波设置，经历几个周期跳变认定波形稳定0x0～0xF
  TIM_ICInit(TIM3, &TIM_ICInitStruct);        //初始化
//TIM3通道4的捕捉初始化
// TIM_ICInitStruct.TIM_Channel 		= TIM_Channel_4;//通道选择
//  TIM_ICInit(TIM2, &TIM_ICInitStruct);        		//初始化

  
  TIM_SelectInputTrigger(TIM3, TIM_TS_TI2FP2);   	//选择时钟触发源
  TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);	//触发方式
  TIM_SelectMasterSlaveMode(TIM3, TIM_MasterSlaveMode_Enable); //启动定时器的被动触发

  TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);

  TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);        //打开中断
//  TIM_ITConfig(TIM2, TIM_IT_CC4, ENABLE);      //打开中断
  
  TIM_Cmd(TIM3, ENABLE);                         //启动TIM2
}
/**********************************************************************
* Function Name  : TIM2_Cap1Start
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
**********************************************************************/
void TIM3_Cap1Start(void)
{
   TIM_CCxCmd( TIM3, TIM_Channel_1, TIM_CCx_Enable);	 
}
/**********************************************************************
* Function Name  : TIM2_Cap1Stop
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
**********************************************************************/
void TIM3_Cap1Stop(void)
{
   TIM_CCxCmd( TIM3, TIM_Channel_1, TIM_CCx_Disable);
} 
/**********************************************************************
* Function Name  : TIM2_Cap4Start
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
**********************************************************************/
void TIM3_Cap4Start(void)
{
	TIM_CCxCmd( TIM3, TIM_Channel_4, TIM_CCx_Enable);	 
}
/**********************************************************************
* Function Name  : TIM2_Cap4Stop
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
**********************************************************************/
void TIM3_Cap4Stop(void)
{
	TIM_CCxCmd( TIM3, TIM_Channel_4, TIM_CCx_Disable);
}
