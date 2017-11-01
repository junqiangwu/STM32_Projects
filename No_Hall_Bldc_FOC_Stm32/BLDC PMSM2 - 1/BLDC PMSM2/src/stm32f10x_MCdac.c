/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : stm32f10x_MCdac.c
* Author             : IMS Systems Lab 
* Date First Issued  : 07/06/07
* Description        : This module manages all the necessary function to 
*                      implement DAC functionality
********************************************************************************
* History:
* 28/11/07 v1.0
* 29/05/08 v2.0
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* THIS SOURCE CODE IS PROTECTED BY A LICENSE.
* FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
* IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "stm32f10x_MClib.h"
#include "MC_Globals.h"
#include "stm32f10x_MCdac.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
s16 hMeasurementArray[23];

u8 *OutputVariableNames[23] ={
  "0                   ","Ia                  ","Ib                  ",
  "Ialpha              ","Ibeta               ","Iq                  ",
  "Id                  ","Iq ref              ","Id ref              ",
  "Vq                  ","Vd                  ","Valpha              ",
  "Vbeta               ","Measured El Angle   ","Measured Rotor Speed",
  "Observed El Angle   ","Observed Rotor Speed","Observed Ialpha     ",
  "Observed Ibeta      ","Observed B-emf alpha","Observed B-emf beta ",
  "User 1              ","User 2              "};

#if (defined ENCODER || defined VIEW_ENCODER_FEEDBACK || defined HALL_SENSORS\
  || defined VIEW_HALL_FEEDBACK) && (defined NO_SPEED_SENSORS ||\
  defined OBSERVER_GAIN_TUNING)

u8 OutputVar[23]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22};
u8 max_out_var_num = 22;

#elif (defined NO_SPEED_SENSORS)
u8 OutputVar[21]={0,1,2,3,4,5,6,7,8,9,10,11,12,15,16,17,18,19,20,21,22};
u8 max_out_var_num = 20;

#elif (defined ENCODER || defined HALL_SENSORS)
u8 OutputVar[17]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,21,22};
u8 max_out_var_num = 16;
#endif

u8 bChannel_1_variable=1;
u8 bChannel_2_variable=1;

/*******************************************************************************
* Function Name : MCDAC_Configuration
* Description : provides a short description of the function
* Input : details the input parameters.
* Output : details the output parameters.
* Return : details the return value.
*******************************************************************************/
void MCDAC_Init (void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  GPIO_InitTypeDef GPIO_InitStructure; 
  TIM_OCInitTypeDef TIM_OCInitStructure;
  
  /* Enable GPIOB */
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_StructInit(&GPIO_InitStructure);
 
  /* Configure PB.00 as alternate function output */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* Enable TIM3 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  TIM_DeInit(TIM3);

  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

  TIM_OCStructInit(&TIM_OCInitStructure);
  
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 0x800;          
  TIM_TimeBaseStructure.TIM_Prescaler = 0x0;       
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* Output Compare PWM Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;                
  TIM_OCInitStructure.TIM_Pulse = 0x400; //Dummy value;    
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);
  
  /* Output Compare PWM Mode configuration: Channel4 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                   
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0x400; //Dummy value;    
  TIM_OC4Init(TIM3, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable); 
  
  /* Enable TIM3 counter */
  TIM_Cmd(TIM3, ENABLE);
}

/*******************************************************************************
* Function Name : MCDAC_Output
* Description : provides a short description of the function
* Input : details the input parameters.
* Output : details the output parameters.
* Return : details the return value.
*******************************************************************************/
void MCDAC_Update_Output(void)
{
    TIM_SetCompare3(TIM3, ((u16)((s16)((hMeasurementArray[OutputVar[bChannel_1_variable]]+32768)/32))));
    TIM_SetCompare4(TIM3, ((u16)((s16)((hMeasurementArray[OutputVar[bChannel_2_variable]]+32768)/32))));
}

/*******************************************************************************
* Function Name : MCDAC_Send_Output_Value
* Description : provides a short description of the function
* Input : details the input parameters.
* Output : details the output parameters.
* Return : details the return value.
*******************************************************************************/
void MCDAC_Update_Value(u8 bVariable, s16 hValue)
{
  hMeasurementArray[bVariable] = hValue;
}

/*******************************************************************************
* Function Name : MCDAC_Output_Choice
* Description : provides a short description of the function
* Input : details the input parameters.
* Output : details the output parameters.
* Return : details the return value.
*******************************************************************************/
void MCDAC_Output_Choice(s8 bStep, u8 bChannel)
{
  if (bChannel == DAC_CH1)
  {
     bChannel_1_variable += bStep;
     if (bChannel_1_variable > max_out_var_num)
     {
       bChannel_1_variable = 1;
     }
     else if (bChannel_1_variable == 0)
     {
       bChannel_1_variable = max_out_var_num;
     }
  }
  else
  {
     bChannel_2_variable += bStep;
     if (bChannel_2_variable > max_out_var_num)
     {
       bChannel_2_variable = 1;
     }
     else if (bChannel_2_variable == 0)
     {
       bChannel_2_variable = max_out_var_num;
     }
  }
}

/*******************************************************************************
* Function Name : MCDAC_Output_Var_Name
* Description : provides a short description of the function
* Input : details the input parameters.
* Output : details the output parameters.
* Return : details the return value.
*******************************************************************************/
u8 *MCDAC_Output_Var_Name(u8 bChannel)
{
  u8 *temp;
  if (bChannel == DAC_CH1)
  {
    temp = OutputVariableNames[OutputVar[bChannel_1_variable]];
  }
  else
  {
    temp = OutputVariableNames[OutputVar[bChannel_2_variable]];
  }
  return (temp);
}
    
  

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
