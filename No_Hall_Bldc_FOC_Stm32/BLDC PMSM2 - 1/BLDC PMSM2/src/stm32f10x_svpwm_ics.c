/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : STM32x_svpwm_ics.c
* Author             : IMS Systems Lab
* Date First Issued  : 21/11/07
* Description        : ICS current reading and PWM generation module 
********************************************************************************
* History:
* 21/11/07 v1.0
* 29/05/08 v2.0
* 09/07/08 v2.0.1
* 14/07/08 v2.0.2
* 17/07/08 v2.0.3
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

#include "STM32F10x_MCconf.h"

#ifdef ICS_SENSORS

/* Includes-------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "stm32f10x_svpwm_ics.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define NB_CONVERSIONS 16

#define SQRT_3		1.732051
#define T		(PWM_PERIOD * 4)
#define T_SQRT3         (u16)(T * SQRT_3)

#define SECTOR_1	(u32)1
#define SECTOR_2	(u32)2
#define SECTOR_3	(u32)3
#define SECTOR_4	(u32)4
#define SECTOR_5	(u32)5
#define SECTOR_6	(u32)6

#define PHASE_A_MSK       (u32)((u32)(PHASE_A_ADC_CHANNEL) << 10)
#define PHASE_B_MSK       (u32)((u32)(PHASE_B_ADC_CHANNEL) << 10)

#define TEMP_FDBK_MSK     (u32)((u32)(TEMP_FDBK_CHANNEL) <<15)
#define BUS_VOLT_FDBK_MSK (u32)((u32)(BUS_VOLT_FDBK_CHANNEL) <<15)
#define SEQUENCE_LENGHT    0x00100000

#define ADC_PRE_EMPTION_PRIORITY 1
#define ADC_SUB_PRIORITY 1

#define BRK_PRE_EMPTION_PRIORITY 0
#define BRK_SUB_PRIORITY 1

#define LOW_SIDE_POLARITY  TIM_OCIdleState_Reset

#define ADC_RIGHT_ALIGNMENT 3

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static u16 hPhaseAOffset;
static u16 hPhaseBOffset;
    
/* Private function prototypes -----------------------------------------------*/

void SVPWM_IcsInjectedConvConfig(void);

/*******************************************************************************
* Function Name  : SVPWM_IcsInit
* Description    : It initializes PWM and ADC peripherals
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVPWM_IcsInit(void)
{ 
  ADC_InitTypeDef ADC_InitStructure;
  TIM_TimeBaseInitTypeDef TIM1_TimeBaseStructure;
  TIM_OCInitTypeDef TIM1_OCInitStructure;
  TIM_BDTRInitTypeDef TIM1_BDTRInitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* ADC1, ADC2, DMA, GPIO, TIM1 clocks enabling -----------------------------*/
  
  /* ADCCLK = PCLK2/6 */
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);

  /* Enable DMA clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /* Enable ADC1 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  /* Enable ADC2 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
  
  /* Enable GPIOA-GPIOE clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | 
           RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                                                  RCC_APB2Periph_GPIOE, ENABLE);
   
  /* Enable TIM1 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  
  /* ADC1, ADC2, PWM pins configurations -------------------------------------*/
  GPIO_StructInit(&GPIO_InitStructure);
  /****** Configure phase A ADC channel GPIO as analog input ****/
  GPIO_InitStructure.GPIO_Pin = PHASE_A_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(PHASE_A_GPIO_PORT, &GPIO_InitStructure);
  /****** Configure phase B ADC channel GPIO as analog input ****/
  GPIO_InitStructure.GPIO_Pin = PHASE_B_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(PHASE_B_GPIO_PORT, &GPIO_InitStructure);
  GPIO_StructInit(&GPIO_InitStructure);  
  /****** Configure temperature reading ADC channel GPIO as analog input ****/
  GPIO_InitStructure.GPIO_Pin = TEMP_FDBK_CHANNEL_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(TEMP_FDBK_CHANNEL_GPIO_PORT, &GPIO_InitStructure);
  /****** Configure bus voltage reading ADC channel GPIO as analog input ****/
  GPIO_InitStructure.GPIO_Pin = BUS_VOLT_FDBK_CHANNEL_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(BUS_VOLT_FDBK_CHANNEL_GPIO_PORT, &GPIO_InitStructure);
    
  /* TIM1 Peripheral Configuration -------------------------------------------*/
  /* TIM1 Registers reset */
  TIM_DeInit(TIM1);
  TIM_TimeBaseStructInit(&TIM1_TimeBaseStructure);
  /* Time Base configuration */
  TIM1_TimeBaseStructure.TIM_Prescaler = PWM_PRSC;
  TIM1_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;
  TIM1_TimeBaseStructure.TIM_Period = PWM_PERIOD;
  TIM1_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV2;
  TIM1_TimeBaseStructure.TIM_RepetitionCounter = REP_RATE;
  TIM_TimeBaseInit(TIM1, &TIM1_TimeBaseStructure);

  TIM_OCStructInit(&TIM1_OCInitStructure);
  /* Channel 1, 2,3 and 4 Configuration in PWM mode */
  TIM1_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
  TIM1_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
  TIM1_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;                  
  TIM1_OCInitStructure.TIM_Pulse = 0x505; //dummy value
  TIM1_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
  TIM1_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;         
  TIM1_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
  TIM1_OCInitStructure.TIM_OCNIdleState = LOW_SIDE_POLARITY;          
  
  TIM_OC1Init(TIM1, &TIM1_OCInitStructure); 

  TIM1_OCInitStructure.TIM_Pulse = 0x505; //dummy value
  TIM_OC2Init(TIM1, &TIM1_OCInitStructure);

  TIM1_OCInitStructure.TIM_Pulse = 0x505; //dummy value
  TIM_OC3Init(TIM1, &TIM1_OCInitStructure);
  
  /*Timer1 alternate function full remapping*/  
  GPIO_PinRemapConfig(GPIO_FullRemap_TIM1,ENABLE);
  
  GPIO_StructInit(&GPIO_InitStructure);  

  /* GPIOE Configuration: Channel 1, 1N, 2, 2N, 3 and 3N Output */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOE, &GPIO_InitStructure); 
  
  /* Lock GPIOE Pin9 to Pin 13 */
  GPIO_PinLockConfig(GPIOE, GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13);
  GPIO_StructInit(&GPIO_InitStructure);
  
  /* GPIOE Configuration: BKIN pin */   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* Automatic Output enable, Break, dead time and lock configuration*/
  TIM1_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
  TIM1_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
  TIM1_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1; 
  TIM1_BDTRInitStructure.TIM_DeadTime = DEADTIME;
  TIM1_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
  TIM1_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;
  TIM1_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;

  TIM_BDTRConfig(TIM1, &TIM1_BDTRInitStructure);

  TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);
  
  TIM_ClearITPendingBit(TIM1, TIM_IT_Break);
  TIM_ITConfig(TIM1, TIM_IT_Break, ENABLE);
  
  /* TIM1 counter enable */
  TIM_Cmd(TIM1, ENABLE);
  
  /* ADC1 registers reset ----------------------------------------------------*/
  ADC_DeInit(ADC1);
  /* ADC2 registers reset ----------------------------------------------------*/
  ADC_DeInit(ADC2);
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  /* Enable ADC2 */
  ADC_Cmd(ADC2, ENABLE);
  
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_Mode = ADC_Mode_InjecSimult;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Left;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
   
  /* ADC2 Configuration ------------------------------------------------------*/
  ADC_StructInit(&ADC_InitStructure);  
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Left;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC2, &ADC_InitStructure);
  
  // Start calibration of ADC1
  ADC_StartCalibration(ADC1);
  // Start calibration of ADC2
  ADC_StartCalibration(ADC2);
  
  // Wait for the end of ADCs calibration 
  while (ADC_GetCalibrationStatus(ADC1) & ADC_GetCalibrationStatus(ADC2))
  {
  }
  ADC_InjectedSequencerLengthConfig(ADC1,2);
  SVPWM_IcsCurrentReadingCalibration();
    
  /* ADC2 Injected conversions configuration */ 
  ADC_InjectedSequencerLengthConfig(ADC2,2);
  ADC_InjectedChannelConfig(ADC2, PHASE_B_ADC_CHANNEL, 1, 
                                                      SAMPLING_TIME_CK);
  ADC_InjectedChannelConfig(ADC2, TEMP_FDBK_CHANNEL, 2,
                                                      SAMPLING_TIME_CK);
  
  ADC_ExternalTrigInjectedConvCmd(ADC2,ENABLE);
  
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  NVIC_StructInit(&NVIC_InitStructure);
  /* Enable the ADC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = ADC_PRE_EMPTION_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = ADC_SUB_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
    
  /* Enable the TIM1 BRK Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = BRK_PRE_EMPTION_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = BRK_SUB_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

} 

/*******************************************************************************
* Function Name  : SVPWM_IcsCurrentReadingCalibration
* Description    : Store zero current converted values for current reading 
                   network offset compensation in case of Ics 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void SVPWM_IcsCurrentReadingCalibration(void)
{
 static u8 bIndex;
   
  /* ADC1 Injected group of conversions end interrupt disabling */
  ADC_ITConfig(ADC1, ADC_IT_JEOC, DISABLE);
  
  hPhaseAOffset=0;
  hPhaseBOffset=0;
   
  /* ADC1 Injected conversions trigger is given by software and enabled */ 
  ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_None);  
  ADC_ExternalTrigInjectedConvCmd(ADC1,ENABLE); 
  
  /* ADC1 Injected conversions configuration */   
  ADC_InjectedChannelConfig(ADC1, PHASE_A_ADC_CHANNEL,1,SAMPLING_TIME_CK);
  ADC_InjectedChannelConfig(ADC1, PHASE_B_ADC_CHANNEL,2,SAMPLING_TIME_CK);
   
  /* Clear the ADC1 JEOC pending flag */
  ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);   
  ADC_SoftwareStartInjectedConvCmd(ADC1,ENABLE);
  
  /* ADC Channel used for current reading are read 
     in order to get zero currents ADC values*/ 
  for(bIndex=NB_CONVERSIONS; bIndex !=0; bIndex--)
  {
    while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_JEOC)) { }
  
    hPhaseAOffset += (ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_1)
                                                         >>ADC_RIGHT_ALIGNMENT);
    hPhaseBOffset += (ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_2)
                                                         >>ADC_RIGHT_ALIGNMENT);  
    /* Clear the ADC1 JEOC pending flag */
    ADC_ClearFlag(ADC1, ADC_FLAG_JEOC); 	
    ADC_SoftwareStartInjectedConvCmd(ADC1,ENABLE);
  }
  
  SVPWM_IcsInjectedConvConfig();  
}


/*******************************************************************************
* Function Name  : SVPWM_IcsInjectedConvConfig
* Description    : This function configure ADC1 for ICS current 
*                  reading and temperature and voltage feedbcak after a 
*                  calibration of the utilized ADC Channels for current reading
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVPWM_IcsInjectedConvConfig(void)
{
  /* ADC1 Injected conversions configuration */ 
  ADC_InjectedChannelConfig(ADC1, PHASE_A_ADC_CHANNEL, 1, 
                                                      SAMPLING_TIME_CK);
  ADC_InjectedChannelConfig(ADC1, BUS_VOLT_FDBK_CHANNEL, 
                                                   2, SAMPLING_TIME_CK);
  
  /* ADC1 Injected conversions trigger is TIM1 TRGO */ 
  ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_TRGO); 
  
  /* ADC1 Injected group of conversions end interrupt enabling */
  ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
}

/*******************************************************************************
* Function Name  : SVPWM_IcsPhaseCurrentValues
* Description    : This function computes current values of Phase A and Phase B 
*                 in q1.15 format starting from values acquired from the A/D 
*                 Converter peripheral.
* Input          : None
* Output         : Stat_Curr_a_b
* Return         : None
*******************************************************************************/
Curr_Components SVPWM_IcsGetPhaseCurrentValues(void)
{
  Curr_Components Local_Stator_Currents;
  s32 wAux;

      
 // Ia = (hPhaseAOffset)-(PHASE_A_ADC_CHANNEL vale)  
  wAux = ((ADC1->JDR1)<<1)-(s32)(hPhaseAOffset);          
 //Saturation of Ia 
  if (wAux < S16_MIN)
  {
    Local_Stator_Currents.qI_Component1= S16_MIN;
  }  
  else  if (wAux > S16_MAX)
        { 
          Local_Stator_Currents.qI_Component1= S16_MAX;
        }
        else
        {
          Local_Stator_Currents.qI_Component1= wAux;
        }
                     
 // Ib = (hPhaseBOffset)-(PHASE_B_ADC_CHANNEL value)
  wAux = ((ADC2->JDR1)<<1)-(s32)(hPhaseBOffset);
 // Saturation of Ib
  if (wAux < S16_MIN)
  {
    Local_Stator_Currents.qI_Component2= S16_MIN;
  }  
  else  if (wAux > S16_MAX)
        { 
          Local_Stator_Currents.qI_Component2= S16_MAX;
        }
        else
        {
          Local_Stator_Currents.qI_Component2= wAux;
        }
  
  return(Local_Stator_Currents); 
}

/*******************************************************************************
* Function Name  : SVPWM_IcsCalcDutyCycles
* Description    : Computes duty cycle values corresponding to the input value
		   and configures 
* Input          : Stat_Volt_alfa_beta
* Output         : None
* Return         : None
*******************************************************************************/

void SVPWM_IcsCalcDutyCycles (Volt_Components Stat_Volt_Input)
{
   u8 bSector;
   s32 wX, wY, wZ, wUAlpha, wUBeta;
   u16  hTimePhA=0, hTimePhB=0, hTimePhC=0;
    
   wUAlpha = Stat_Volt_Input.qV_Component1 * T_SQRT3 ;//park逆变换,?
   wUBeta = -(Stat_Volt_Input.qV_Component2 * T);

   wX = wUBeta;
   wY = (wUBeta + wUAlpha)/2;//clark逆变换
   wZ = (wUBeta - wUAlpha)/2;

  // Sector calculation from wX, wY, wZ
   if (wY<0)
   {
      if (wZ<0)
      {
        bSector = SECTOR_5;
      }
      else // wZ >= 0
        if (wX<=0)
        {
          bSector = SECTOR_4;
        }
        else // wX > 0
        {
          bSector = SECTOR_3;
        }
   }
   else // wY > 0
   {
     if (wZ>=0)
     {
       bSector = SECTOR_2;
     }
     else // wZ < 0
       if (wX<=0)
       {  
         bSector = SECTOR_6;
       }
       else // wX > 0
       {
         bSector = SECTOR_1;
       }
    }
   
   /* Duty cycles computation */
  
  switch(bSector)
  {  
    case SECTOR_1:  //时间区间是0-4-6-7-7-6-4-0
    case SECTOR_4:	//时间区间是0-1-3-7-7-3-1-0
                hTimePhA = (T/8) + ((((T + wX) - wZ)/2)/131072);
		hTimePhB = hTimePhA + wZ/131072;
		hTimePhC = hTimePhB - wX/131072;                                       
                break;
    case SECTOR_2:
    case SECTOR_5:  
                hTimePhA = (T/8) + ((((T + wY) - wZ)/2)/131072);
        	hTimePhB = hTimePhA + wZ/131072;
		hTimePhC = hTimePhA - wY/131072;
                break;

    case SECTOR_3:
    case SECTOR_6:
                hTimePhA = (T/8) + ((((T - wX) + wY)/2)/131072);
		hTimePhC = hTimePhA - wY/131072;
		hTimePhB = hTimePhC + wX/131072;
                break;
    default:
		break;
   }
  
  /* Load compare registers values */
   
  TIM1->CCR1 = hTimePhA;
  TIM1->CCR2 = hTimePhB;
  TIM1->CCR3 = hTimePhC;
}

/*******************************************************************************
* Function Name  : SVPWMEOCEvent
* Description    :  Routine to be performed inside the end of conversion ISR
* Input           : None
* Output         : None
* Return         : None
*******************************************************************************/
u8 SVPWMEOCEvent(void)
{
  // Store the Bus Voltage and temperature sampled values
 // h_ADCTemp = ADC_GetInjectedConversionValue(ADC2,ADC_InjectedChannel_2);
 // h_ADCBusvolt = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_2);
  return ((u8)(1));
}
#endif //ICS_SENSORS
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/  
