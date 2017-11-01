/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : STM32x_svpwm_1shunt.c
* Author             : IMS Systems Lab
* Date First Issued  : Mar/08
* Description        : 1 shunt resistor current reading module
********************************************************************************
* History:
* 29/05/08 v2.0
* 02/07/08 v2.0.1
* 03/07/08 v2.0.2
* 11/07/08 v2.0.3
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

#ifdef SINGLE_SHUNT

/* Includes-------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "stm32f10x_svpwm_1shunt.h"
#include "MC_Globals.h"

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

#define REGULAR         ((u8)0)
#define BOUNDARY_1      ((u8)1)  // Two small, one big
#define BOUNDARY_2      ((u8)2)  // Two big, one small
#define BOUNDARY_3      ((u8)3)  // Three equal

#define PHASE_B_ADC_CHANNEL     ADC_Channel_12

#define ADC_PRE_EMPTION_PRIORITY 1
#define ADC_SUB_PRIORITY 0

#define BRK_PRE_EMPTION_PRIORITY 0
#define BRK_SUB_PRIORITY 0

#define TIM1_UP_PRE_EMPTION_PRIORITY 0
#define TIM1_UP_SUB_PRIORITY 0

#define LOW_SIDE_POLARITY  TIM_OCIdleState_Reset

// Direct address of the registers used by DMA
#define TIM1_CCR1_Address   0x40012C34
#define TIM1_CCR2_Address   0x40012C38
#define TIM1_CCR3_Address   0x40012C3C
#define TIM1_CCR4_Address   0x40012C40
#define TIM1_DMAR_Address   0x40012C4C

#define CCMR1_OC1PE_BB    0x4225830C
#define CCMR1_OC2PE_BB    0x4225832C
#define CCMR2_OC3PE_BB    0x4225838C

#define CH1NORMAL 0x0060
#define CH2NORMAL 0x6000
#define CH3NORMAL 0x0060
#define CH4NORMAL 0x7000
#define CH1TOGGLE 0x0030
#define CH2TOGGLE 0x3000
#define CH3TOGGLE 0x0030
#define CH4TOGGLE 0x3000

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u8  bSector;  
u16 hPhaseOffset=0;

u8 bInverted_pwm=INVERT_NONE,bInverted_pwm_new=INVERT_NONE;
u8  bStatorFluxPos, bStatorFluxPosOld;
DUTYVALUESTYPE dvDutyValues;
CURRENTSAMPLEDTYPE csCurrentSampled;
u8 bError=0;

s16 hCurrAOld,hCurrBOld,hCurrCOld;
s16 hDeltaA,hDeltaB,hDeltaC;
u8 bReadDelta;

u16 hPreloadCCMR1Disable;
u16 hPreloadCCMR1Set;
u16 hPreloadCCMR2Disable;
u16 hPreloadCCMR2Set;
   
u16 hCCDmaBuffCh1[4];
u16 hCCDmaBuffCh2[4];
u16 hCCDmaBuffCh3[4];
u16 hCCDmaBuffCh4[4];

u16 hCCRBuff[4];

u8 bStBd3 = 0;
u8 bDistEnab = 0;

/* Private function prototypes -----------------------------------------------*/

void SVPWM_InjectedConvConfig(void);

/*******************************************************************************
* Function Name  : SVPWM_1ShuntInit
* Description    : It initializes PWM and ADC peripherals
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVPWM_1ShuntInit(void)
{ 
  ADC_InitTypeDef ADC_InitStructure;
  TIM_TimeBaseInitTypeDef TIM1_TimeBaseStructure;
  TIM_OCInitTypeDef TIM1_OCInitStructure;
  TIM_BDTRInitTypeDef TIM1_BDTRInitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;

  /* ADC1, ADC2, DMA, GPIO, TIM1 clocks enabling -----------------------------*/
  
  /* ADCCLK = PCLK2/6 */
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);

  /* Enable DMA clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  
  /* Enable GPIOA, GPIOC, GPIOE, AFIO clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB|
                         RCC_APB2Periph_GPIOC , ENABLE);
  /* Enable ADC1 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  /* Enable ADC2 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE); 
   
  /* Enable TIM1 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
     
  /* ADC1, ADC2, PWM pins configurations -------------------------------------*/
  GPIO_StructInit(&GPIO_InitStructure);
  /****** Configure PC.00,01,2,3,4 (ADC Channels [10..14]) as analog input ****/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
     
  GPIO_StructInit(&GPIO_InitStructure);
  /****** Configure PA.03 (ADC Channels [3]) as analog input ******/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_Init(GPIOB, &GPIO_InitStructure); 
  // After reset value of DMA buffers
  hCCDmaBuffCh1[0] = PWM_PERIOD-HTMIN;
  hCCDmaBuffCh1[1] = PWM_PERIOD-HTMIN;
  hCCDmaBuffCh1[2] = PWM_PERIOD >> 1;
  hCCDmaBuffCh1[3] = PWM_PERIOD >> 1;
  ;
  hCCDmaBuffCh2[0] = PWM_PERIOD-HTMIN;
  hCCDmaBuffCh2[1] = PWM_PERIOD-HTMIN;
  hCCDmaBuffCh2[2] = PWM_PERIOD >> 1;
  hCCDmaBuffCh2[3] = PWM_PERIOD >> 1;
  
  hCCDmaBuffCh3[0] = PWM_PERIOD-HTMIN;
  hCCDmaBuffCh3[1] = PWM_PERIOD-HTMIN;
  hCCDmaBuffCh3[2] = PWM_PERIOD >> 1;
  hCCDmaBuffCh3[3] = PWM_PERIOD >> 1;
  
  // Default Update DMA buffer Ch 1,2,3,4 after reset
  hCCRBuff[0] = PWM_PERIOD >> 1;
  hCCRBuff[1] = PWM_PERIOD >> 1;
  hCCRBuff[2] = PWM_PERIOD >> 1;
  hCCRBuff[3] = (PWM_PERIOD >> 1) - TBEFORE;
  
  // After reset value of dvDutyValues
  dvDutyValues.hTimePhA = PWM_PERIOD >> 1;
  dvDutyValues.hTimePhB = PWM_PERIOD >> 1;
  dvDutyValues.hTimePhC = PWM_PERIOD >> 1;
  
  /* TIM1 Channel 1 toggle mode */
  /* DMA Channel2 configuration ----------------------------------------------*/
  DMA_DeInit(DMA1_Channel2);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)TIM1_CCR1_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)(hCCDmaBuffCh1);
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 4;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel2, &DMA_InitStructure);
  /* Enable DMA Channel2 */
  //DMA_Cmd(DMA_Channel2, ENABLE);
  DMA_Cmd(DMA1_Channel2, DISABLE);
  
  /* TIM1 Channel 2 toggle mode */
  /* DMA Channel3 configuration ----------------------------------------------*/
  DMA_DeInit(DMA1_Channel3);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)TIM1_CCR2_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)(hCCDmaBuffCh2);
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 4;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel3, &DMA_InitStructure);
  /* Enable DMA Channel3 */
  //DMA_Cmd(DMA_Channel3, ENABLE);
  DMA_Cmd(DMA1_Channel3, DISABLE);

  /* TIM1 Channel 3 toggle mode */
  /* DMA Channel6 configuration ----------------------------------------------*/
  DMA_DeInit(DMA1_Channel6);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)TIM1_CCR3_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)(hCCDmaBuffCh3);
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 4;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel6, &DMA_InitStructure);
  /* Enable DMA Channel6 */
  DMA_Cmd(DMA1_Channel6, DISABLE);
  
  /* TIM1 Channel 4 PWM2 or toggle mode */
  /* DMA channel4 configuration */
  DMA_DeInit(DMA1_Channel4);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)TIM1_CCR4_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)(hCCDmaBuffCh4);
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 2;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel4, &DMA_InitStructure);
  
  DMA_Cmd(DMA1_Channel4, ENABLE);
  
  /* DMA TIM1 update configuration */
  DMA_DeInit(DMA1_Channel5);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)TIM1_DMAR_Address; 
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)(hCCRBuff);
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 4;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel5, &DMA_InitStructure);
  
  DMA_Cmd(DMA1_Channel5, ENABLE);
   
  /* TIM1 Peripheral Configuration -------------------------------------------*/
  /* TIM1 Registers reset */
  TIM_DeInit(TIM1);
  TIM_TimeBaseStructInit(&TIM1_TimeBaseStructure);
  /* Time Base configuration */
  TIM1_TimeBaseStructure.TIM_Prescaler = 0x0;
  TIM1_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned3;
  TIM1_TimeBaseStructure.TIM_Period = PWM_PERIOD;
  TIM1_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV2;
  
  // Initial condition is REP=0 to set the UPDATE only on the underflow
  TIM1_TimeBaseStructure.TIM_RepetitionCounter = REP_RATE;
  TIM_TimeBaseInit(TIM1, &TIM1_TimeBaseStructure);
  
  TIM_OCStructInit(&TIM1_OCInitStructure);
  /* Channel 1, 2,3 in PWM mode */
  TIM1_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
  TIM1_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
  TIM1_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;                  
  TIM1_OCInitStructure.TIM_Pulse = PWM_PERIOD >> 1; //dummy value
  TIM1_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 
  TIM1_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;         
  TIM1_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
  TIM1_OCInitStructure.TIM_OCNIdleState = LOW_SIDE_POLARITY;          
  
  TIM_OC1Init(TIM1, &TIM1_OCInitStructure); 
  TIM_OC3Init(TIM1, &TIM1_OCInitStructure);
  TIM_OC2Init(TIM1, &TIM1_OCInitStructure);

  /*Timer1 alternate function full remapping*/
  //GPIO_PinRemapConfig(GPIO_FullRemap_TIM1,ENABLE); 
  
  GPIO_StructInit(&GPIO_InitStructure);
  /* GPIOE Configuration: Channel 1, 1N, 2, 2N, 3, 3N and 4 Output */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10  ;
                                
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
  
  /* Lock GPIOE Pin9 and Pin11 Pin 13 (High sides) */
 // GPIO_PinLockConfig(GPIOE, GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13);

  GPIO_StructInit(&GPIO_InitStructure);
  /* GPIOE Configuration: BKIN pin */   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  TIM_OCStructInit(&TIM1_OCInitStructure);
  /* Channel 4 Configuration in OC */
  TIM1_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;  
  TIM1_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
  TIM1_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;                  
  TIM1_OCInitStructure.TIM_Pulse = PWM_PERIOD - TMIN - TBEFORE; 
  
  TIM1_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 
  TIM1_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;         
  TIM1_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
  TIM1_OCInitStructure.TIM_OCNIdleState = LOW_SIDE_POLARITY;            
  
  TIM_OC4Init(TIM1, &TIM1_OCInitStructure);
  
  /* Enables the TIM1 Preload on CC1 Register */
  TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
  /* Enables the TIM1 Preload on CC2 Register */
  TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
  /* Enables the TIM1 Preload on CC3 Register */
  TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
  /* Enables the TIM1 Preload on CC4 Register */
  TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
  
  /* Automatic Output enable, Break, dead time and lock configuration*/
 // TIM1_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
 // TIM1_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
 // TIM1_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1; 
 // TIM1_BDTRInitStructure.TIM_DeadTime = DEADTIME;
 // TIM1_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
 // TIM1_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;
 // TIM1_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;

 //TIM_BDTRConfig(TIM1, &TIM1_BDTRInitStructure);

  TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);
  
  // Clear Break Flag and enable interrupt
  TIM_ClearITPendingBit(TIM1, TIM_IT_Break);
  TIM_ITConfig(TIM1, TIM_IT_Break,ENABLE);
  
  /* TIM1 counter enable */
  TIM_Cmd(TIM1, ENABLE);
  

  // Disnable update interrupt
  TIM_ITConfig(TIM1, TIM_IT_Update, DISABLE);
  
  // Resynch to have the Update evend during Undeflow
  TIM_GenerateEvent(TIM1, TIM_EventSource_Update);
  
  // Enable DMA event
  TIM_DMACmd(TIM1, TIM_DMA_CC1, ENABLE);
  TIM_DMACmd(TIM1, TIM_DMA_CC2, ENABLE);
  TIM_DMACmd(TIM1, TIM_DMA_CC3, ENABLE);
  TIM_DMACmd(TIM1, TIM_DMA_Update,ENABLE);
  
  TIM_DMAConfig(TIM1, TIM_DMABase_CCR1, TIM_DMABurstLength_4Bytes);
  
  // Sets the disable preload vars for CCMR
  hPreloadCCMR1Disable = TIM1->CCMR1 & 0x8F8F;
  hPreloadCCMR2Disable = TIM1->CCMR2 & 0x8F8F;
   
  /* ADC1 registers reset ----------------------------------------------------*/
  ADC_DeInit(ADC1);
  /* ADC1 registers reset ----------------------------------------------------*/
  ADC_DeInit(ADC2);
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  /* Enable ADC2 */
  ADC_Cmd(ADC2, ENABLE);
  
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Left;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
  
  /* ADC2 Configuration ------------------------------------------------------*/
  ADC_StructInit(&ADC_InitStructure);  
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Left;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC2, &ADC_InitStructure);
  
  ADC_InjectedDiscModeCmd(ADC1,ENABLE);
  ADC_InjectedDiscModeCmd(ADC2,ENABLE);
  
  // Start calibration of ADC1
  ADC_StartCalibration(ADC1);
  // Start calibration of ADC2
  ADC_StartCalibration(ADC2);
  
  // Wait for the end of ADCs calibration 
  while (ADC_GetCalibrationStatus(ADC1) & ADC_GetCalibrationStatus(ADC2))
  {
  }
  
  SVPWM_1ShuntCurrentReadingCalibration();
    
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  NVIC_StructInit(&NVIC_InitStructure);
  /* Enable the ADC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = ADC_PRE_EMPTION_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = ADC_SUB_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the Update Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIM1_UP_PRE_EMPTION_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIM1_UP_SUB_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
    
  /* Enable the TIM1 BRK Interrupt */
  //NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_IRQChannel;
  //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = BRK_PRE_EMPTION_PRIORITY;
  //NVIC_InitStructure.NVIC_IRQChannelSubPriority = BRK_SUB_PRIORITY;
  //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  //NVIC_Init(&NVIC_InitStructure);
  
  // Default value of DutyValues
  dvDutyValues.hTimeSmp1 = (PWM_PERIOD >> 1) - TBEFORE;
  dvDutyValues.hTimeSmp2 = (PWM_PERIOD >> 1) + TAFTER;
} 

/*******************************************************************************
* Function Name  : SVPWM_1ShuntCurrentReadingCalibration
* Description    : Store zero current converted values for current reading 
                   network offset compensation in case of 1 shunt resistors 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVPWM_1ShuntCurrentReadingCalibration(void)
{
  static u16 bIndex;
  
  /* ADC1 Injected group of conversions end interrupt disabling */
  ADC_ITConfig(ADC1, ADC_IT_JEOC, DISABLE);
  
  hPhaseOffset=0;
  
  /* ADC1 Injected conversions trigger is given by software and enabled */ 
  ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_TRGO);  
  ADC_ExternalTrigInjectedConvCmd(ADC1,ENABLE); 
  
  /* ADC1 Injected conversions configuration */ 
  ADC_InjectedSequencerLengthConfig(ADC1,2);
  ADC_InjectedChannelConfig(ADC1, PHASE_B_ADC_CHANNEL, 1,SAMPLING_TIME_CK);
  ADC_InjectedChannelConfig(ADC1, PHASE_B_ADC_CHANNEL, 2,SAMPLING_TIME_CK);

  /* Clear the ADC1 JEOC pending flag */
  ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);  
    
  /* ADC Channel used for current reading are read 
     in order to get zero currents ADC values*/ 
  for(bIndex=0; bIndex <NB_CONVERSIONS; bIndex++)
  {
    while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_JEOC)) { }
    hPhaseOffset=ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_1);
    hPhaseOffset += (ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_1)>>3);
            
    /* Clear the ADC1 JEOC pending flag */
    ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);    
  }
  
  SVPWM_InjectedConvConfig();  
}

/*******************************************************************************
* Function Name  : SVPWM_InjectedConvConfig
* Description    : This function configure ADC1 for 1 shunt current 
*                  reading and ADC2  temperature and voltage feedbcak after a 
*                  calibration, it also setup the DMA and the default value of the 
*                 variables after the start command
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVPWM_InjectedConvConfig(void)
{  
  /* ADC2 Injected conversions configuration */ 
  ADC_InjectedSequencerLengthConfig(ADC2,2);
  ADC_InjectedChannelConfig(ADC2, TEMP_FDBK_CHANNEL, 1,SAMPLING_TIME_CK);
  ADC_InjectedChannelConfig(ADC2, BUS_VOLT_FDBK_CHANNEL, 2,SAMPLING_TIME_CK);
  
  /* ADC2 Injected conversions trigger is TIM1 TRGO */ 
  ADC_ExternalTrigInjectedConvConfig(ADC2, ADC_ExternalTrigInjecConv_T1_TRGO);
  ADC_ExternalTrigInjectedConvCmd(ADC2,ENABLE);
  
  /* Bus voltage protection initialization*/                            
  //ADC_AnalogWatchdogCmd(ADC2,ADC_AnalogWatchdog_SingleInjecEnable);
 // ADC_AnalogWatchdogSingleChannelConfig(ADC2,BUS_VOLT_FDBK_CHANNEL);
 // ADC_AnalogWatchdogThresholdsConfig(ADC2,OVERVOLTAGE_THRESHOLD>>3,0x00);
  
  /* ADC1 Injected group of conversions end and Analog Watchdog interrupts
                                                                     enabling */
  ADC_ITConfig(ADC2, ADC_IT_AWD, ENABLE);
  ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
  

  // Default value of DutyValues
  dvDutyValues.hTimeSmp1 = (PWM_PERIOD >> 1) - TBEFORE;
  dvDutyValues.hTimeSmp2 = (PWM_PERIOD >> 1) + TAFTER;
  
  // Default value of sampling point
  hCCDmaBuffCh4[0] = dvDutyValues.hTimeSmp2; // Second point 
  hCCDmaBuffCh4[1] = dvDutyValues.hTimeSmp2;
  hCCDmaBuffCh4[2] = dvDutyValues.hTimeSmp1; // First point
  hCCDmaBuffCh4[3] = dvDutyValues.hTimeSmp1;

  // Set TIM1 CCx start value
  TIM1->CCR1 = PWM_PERIOD >> 1;
  TIM1->CCR2 = PWM_PERIOD >> 1;
  TIM1->CCR3 = PWM_PERIOD >> 1;
  TIM1->CCR4 = (PWM_PERIOD >> 1) - TBEFORE;
  
  // Default Update DMA buffer Ch 1,2,3,4 after reset
  hCCRBuff[0] = PWM_PERIOD >> 1;
  hCCRBuff[1] = PWM_PERIOD >> 1;
  hCCRBuff[2] = PWM_PERIOD >> 1;
  hCCRBuff[3] = (PWM_PERIOD >> 1) - TBEFORE;
  
  TIM_DMACmd(TIM1, TIM_DMA_CC4, ENABLE);
  
  // After start value of DMA buffers
  hCCDmaBuffCh1[0] = PWM_PERIOD-HTMIN;
  hCCDmaBuffCh1[1] = PWM_PERIOD-HTMIN;
  hCCDmaBuffCh1[2] = PWM_PERIOD >> 1;
  hCCDmaBuffCh1[3] = PWM_PERIOD >> 1;
  
  hCCDmaBuffCh2[0] = PWM_PERIOD-HTMIN;
  hCCDmaBuffCh2[1] = PWM_PERIOD-HTMIN;
  hCCDmaBuffCh2[2] = PWM_PERIOD >> 1;
  hCCDmaBuffCh2[3] = PWM_PERIOD >> 1;
  
  hCCDmaBuffCh3[0] = PWM_PERIOD-HTMIN;
  hCCDmaBuffCh3[1] = PWM_PERIOD-HTMIN;
  hCCDmaBuffCh3[2] = PWM_PERIOD >> 1;
  hCCDmaBuffCh3[3] = PWM_PERIOD >> 1;
  
  // After start value of dvDutyValues
  dvDutyValues.hTimePhA = PWM_PERIOD >> 1;
  dvDutyValues.hTimePhB = PWM_PERIOD >> 1;
  dvDutyValues.hTimePhC = PWM_PERIOD >> 1;
  
  // Set the default previous value of Phase A,B,C current
  hCurrAOld=0;
  hCurrBOld=0;
  hCurrCOld=0;
  
  hDeltaA = 0;
  hDeltaB = 0;
  hDeltaC = 0;
  bReadDelta = 0;
  bStatorFluxPosOld = REGULAR;
  bStatorFluxPos = REGULAR;
}

/*******************************************************************************
* Function Name  : SVPWM_1ShuntCalcDutyCycles
* Description    :  Implementation of the single shunt algorithm to setup the 
TIM1 register and DMA buffers values for the next PWM period.
* Input          : Stat_Volt_alfa_beta
* Output         : None
* Return         : None
*******************************************************************************/
void SVPWM_1ShuntCalcDutyCycles (Volt_Components Stat_Volt_Input)
{
    s32 wX, wY, wZ, wUAlpha, wUBeta;
    s16 hDeltaDuty[2];
    u16 hDutyV[4]; // the 4th element is the swap tmp
    
/*******************************************************************************
* Function Name  : SVPWM_1ShuntGetDuty
* Description    : Computes the three duty cycle values corresponding to the input value
                        using space vector modulation techinque
* Input          : Stat_Volt_alfa_beta
* Output         : None
* Return         : None
*******************************************************************************/
    //SVPWM_1ShuntGetDuty(Stat_Volt_Input);
    wUAlpha = Stat_Volt_Input.qV_Component1 * T_SQRT3 ;
    wUBeta = -(Stat_Volt_Input.qV_Component2 * T);
  
    wX = wUBeta;
    wY = (wUBeta + wUAlpha)/2;
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
      case SECTOR_1:
          dvDutyValues.hTimePhA = (T/8) + ((((T + wX) - wZ)/2)/131072);
          dvDutyValues.hTimePhB = dvDutyValues.hTimePhA + wZ/131072;
          dvDutyValues.hTimePhC = dvDutyValues.hTimePhB - wX/131072;
                  break;
      case SECTOR_2:
          dvDutyValues.hTimePhA = (T/8) + ((((T + wY) - wZ)/2)/131072);
          dvDutyValues.hTimePhB = dvDutyValues.hTimePhA + wZ/131072;
          dvDutyValues.hTimePhC = dvDutyValues.hTimePhA - wY/131072;
          break;
      case SECTOR_3:
          dvDutyValues.hTimePhA = (T/8) + ((((T - wX) + wY)/2)/131072);
          dvDutyValues.hTimePhC = dvDutyValues.hTimePhA - wY/131072;
          dvDutyValues.hTimePhB = dvDutyValues.hTimePhC + wX/131072;
          break;
      case SECTOR_4:
          dvDutyValues.hTimePhA = (T/8) + ((((T + wX) - wZ)/2)/131072);
          dvDutyValues.hTimePhB = dvDutyValues.hTimePhA + wZ/131072;
          dvDutyValues.hTimePhC = dvDutyValues.hTimePhB - wX/131072;
          break;  
      case SECTOR_5:
          dvDutyValues.hTimePhA = (T/8) + ((((T + wY) - wZ)/2)/131072);
          dvDutyValues.hTimePhB = dvDutyValues.hTimePhA + wZ/131072;
          dvDutyValues.hTimePhC = dvDutyValues.hTimePhA - wY/131072;
              break;
      case SECTOR_6:
          dvDutyValues.hTimePhA = (T/8) + ((((T - wX) + wY)/2)/131072);
          dvDutyValues.hTimePhC = dvDutyValues.hTimePhA - wY/131072;
          dvDutyValues.hTimePhB = dvDutyValues.hTimePhC + wX/131072;
          break;
      default:
          break;
    }
    
    if (bDistEnab == 1)
    {
      bStatorFluxPosOld = bStatorFluxPos;
    
/*******************************************************************************
* Function Name  : SVPWM_1GetStatorFluxPos
* Description    :  Compute the stator vector position 
                        REGULAR if stator vector lies in regular region
                        BOUNDARY_1 if stator vector lies in boundary region 1 (two small, one big)
                        BOUNDARY_2 if stator vector lies in boundary region 2 (two big, one small)
                        BOUNDARY_3 if stator vector lies in boundary region 3 (three equal)
* Input           : None
* Output         : None
* Return         : None
*******************************************************************************/
      //bStatorFluxPos = SVPWM_1GetStatorFluxPos(); 
      hDutyV[0] = dvDutyValues.hTimePhA;
      hDutyV[1] = dvDutyValues.hTimePhB;
      hDutyV[2] = dvDutyValues.hTimePhC;
      
      // Sort ascendant
      if (hDutyV[0] > hDutyV[1])
      {
        // Swap [0] [1]
        hDutyV[3] = hDutyV[0];
        hDutyV[0] = hDutyV[1];
        hDutyV[1] = hDutyV[3];
      }
      if (hDutyV[0] > hDutyV[2])
      {
        // Swap [0] [2]
        hDutyV[3] = hDutyV[0];
        hDutyV[0] = hDutyV[2];
        hDutyV[2] = hDutyV[3];
      }
      if (hDutyV[1] > hDutyV[2])
      {
        // Swap [1] [2]
        hDutyV[3] = hDutyV[1];
        hDutyV[1] = hDutyV[2];
        hDutyV[2] = hDutyV[3];
      }
      
      // Compute delta duty
      hDeltaDuty[0] = (s16)(hDutyV[1]) - (s16)(hDutyV[0]);
      hDeltaDuty[1] = (s16)(hDutyV[2]) - (s16)(hDutyV[1]);
      
      // Check region
      if ((hDeltaDuty[1]>TMIN) && (hDeltaDuty[0]<=TMIN))
        bStatorFluxPos = BOUNDARY_2;
      else if ((hDeltaDuty[1]<=TMIN) && (hDeltaDuty[0]>TMIN))
        bStatorFluxPos = BOUNDARY_1;
      else if ((hDeltaDuty[1]>TMIN) && (hDeltaDuty[0]>TMIN))
        bStatorFluxPos = REGULAR;    
      else
        bStatorFluxPos = BOUNDARY_3;
    
/*******************************************************************************
* Function Name  : SVPWM_1PWMDutyAdj
* Description    :  Compute the PWM channel that must be distorted and updates
                    the value od duty cycle registers
                                
* Input           : None
* Output         : None
* Return         : None
*******************************************************************************/    
      //SVPWM_1PWMDutyAdj();
      if (bStatorFluxPos == REGULAR)
      {
              bInverted_pwm_new = INVERT_NONE;
      }
      else if (bStatorFluxPos == BOUNDARY_1) // Adjust the lower
      {
        switch (bSector)
        {
                case SECTOR_5:
                case SECTOR_6:
                        bInverted_pwm_new = INVERT_A;
                        dvDutyValues.hTimePhA -=HTMIN;
                        break;
                case SECTOR_2:
                case SECTOR_1:
                        bInverted_pwm_new = INVERT_B;
                        dvDutyValues.hTimePhB -=HTMIN;
                        break;
                case SECTOR_4:
                case SECTOR_3:
                        bInverted_pwm_new = INVERT_C;
                        dvDutyValues.hTimePhC -=HTMIN;
                        break;
        }
      }
      else if (bStatorFluxPos == BOUNDARY_2) // Adjust the middler
      {
        switch (bSector)
        {
                case SECTOR_4:
                case SECTOR_5: // Inverto sempre B
                        bInverted_pwm_new = INVERT_B;
                        dvDutyValues.hTimePhB -=HTMIN;
                        break;
                case SECTOR_2:
                case SECTOR_3: // Inverto sempre A
                        bInverted_pwm_new = INVERT_A;
                        dvDutyValues.hTimePhA -=HTMIN;
                        break;
                case SECTOR_6:
                case SECTOR_1: // Inverto sempre C
                        bInverted_pwm_new = INVERT_C;
                        dvDutyValues.hTimePhC -=HTMIN;
                        break;
        }
      }
      else if (bStatorFluxPos == BOUNDARY_3)
      {
        if (bStBd3 == 0)
        {
          bInverted_pwm_new = INVERT_A;
          dvDutyValues.hTimePhA -=HTMIN;
          bStBd3 = 1;
        } 
        else
        {
          bInverted_pwm_new = INVERT_B;
          dvDutyValues.hTimePhB -=HTMIN;
          bStBd3 = 0;
        }
      }
      
      if (bInverted_pwm_new != INVERT_NONE)
      {
        // Check for negative values of duty register
        if (dvDutyValues.hTimePhA > 0xEFFF)
          dvDutyValues.hTimePhA = DMABURSTMIN_A;
        if (dvDutyValues.hTimePhB > 0xEFFF)
          dvDutyValues.hTimePhB = DMABURSTMIN_B;
        if (dvDutyValues.hTimePhC > 0xEFFF)
          dvDutyValues.hTimePhC = DMABURSTMIN_C;
        
        // Duty adjust to avoid commutation inside Update Handler
        if ((dvDutyValues.hTimePhA > MINTIMCNTUPHAND) && (dvDutyValues.hTimePhA < MIDTIMCNTUPHAND))
            dvDutyValues.hTimePhA = MINTIMCNTUPHAND;
        if ((dvDutyValues.hTimePhA >= MIDTIMCNTUPHAND) && (dvDutyValues.hTimePhA < MAXTIMCNTUPHAND))
            dvDutyValues.hTimePhA = MAXTIMCNTUPHAND;     
        if ((dvDutyValues.hTimePhB > MINTIMCNTUPHAND) && (dvDutyValues.hTimePhB < MIDTIMCNTUPHAND))
            dvDutyValues.hTimePhB = MINTIMCNTUPHAND;
        if ((dvDutyValues.hTimePhB >= MIDTIMCNTUPHAND) && (dvDutyValues.hTimePhB < MAXTIMCNTUPHAND))
            dvDutyValues.hTimePhB = MAXTIMCNTUPHAND;
        if ((dvDutyValues.hTimePhC > MINTIMCNTUPHAND) && (dvDutyValues.hTimePhC < MIDTIMCNTUPHAND))
            dvDutyValues.hTimePhC = MINTIMCNTUPHAND;
        if ((dvDutyValues.hTimePhC >= MIDTIMCNTUPHAND) && (dvDutyValues.hTimePhC < MAXTIMCNTUPHAND))
            dvDutyValues.hTimePhC = MAXTIMCNTUPHAND;
      }
    
    /*******************************************************************************
* Function Name  : SVPWM_1PWMSetSamplingPoints
* Description    :  Compute the sampling point and the related phase sampled 	
* Input           : None
* Output         : None
* Return         : None
*******************************************************************************/
      //SVPWM_1PWMSetSamplingPoints();
      // Reset error state & sampling before
      bError = 0;
      
      if (bStatorFluxPos == REGULAR) // Regual zone
      {
        // First point
        if ((hDutyV[1] - hDutyV[0] - TDEAD)> MAX_TRTS)
        {
          dvDutyValues.hTimeSmp1 = (hDutyV[0] + hDutyV[1] + TDEAD) >> 1;
        }
        else
        {
          dvDutyValues.hTimeSmp1 = hDutyV[1] - TBEFORE;
        }
        // Second point
        if ((hDutyV[2] - hDutyV[1] - TDEAD)> MAX_TRTS)
        {
          dvDutyValues.hTimeSmp2 = (hDutyV[1] + hDutyV[2] + TDEAD) >> 1;
        }
        else
        {
          dvDutyValues.hTimeSmp2 = hDutyV[2] - TBEFORE;
        }
      }
      else 
      {
        // Adjust hDuty
        hDutyV[0] = dvDutyValues.hTimePhA;
        hDutyV[1] = dvDutyValues.hTimePhB;
        hDutyV[2] = dvDutyValues.hTimePhC;
        
        // Sort ascendant
        if (hDutyV[0] > hDutyV[1])
        {
          // Swap [0] [1]
          hDutyV[3] = hDutyV[0];
          hDutyV[0] = hDutyV[1];
          hDutyV[1] = hDutyV[3];
        }
        if (hDutyV[0] > hDutyV[2])
        {
          // Swap [0] [2]
          hDutyV[3] = hDutyV[0];
          hDutyV[0] = hDutyV[2];
          hDutyV[2] = hDutyV[3];
        }
        if (hDutyV[1] > hDutyV[2])
        {
          // Swap [1] [2]
          hDutyV[3] = hDutyV[1];
          hDutyV[1] = hDutyV[2];
          hDutyV[2] = hDutyV[3];
        }
      }
      
      if (bStatorFluxPos == BOUNDARY_1) // Two small, one big
      {   
        // Check after the distortion for sampling space
        if ((hDutyV[1] - hDutyV[0])< TMIN)
        {
          // After the distortion the first sampling point can't be performed
          // It is necessary to swtch to Boudary 3
          
          // Restore the distorted duty
          if (bInverted_pwm_new == INVERT_A);
            dvDutyValues.hTimePhA +=HTMIN;
          if (bInverted_pwm_new == INVERT_B);
            dvDutyValues.hTimePhB +=HTMIN;
          if (bInverted_pwm_new == INVERT_C);
            dvDutyValues.hTimePhC +=HTMIN;
          
          // Switch to Boudary 3
          bStatorFluxPos = BOUNDARY_3;        
          if (bStBd3 == 0)
          {
            bInverted_pwm_new = INVERT_A;
            dvDutyValues.hTimePhA -=HTMIN;
            bStBd3 = 1;
          } 
          else
          {
            bInverted_pwm_new = INVERT_B;
            dvDutyValues.hTimePhB -=HTMIN;
            bStBd3 = 0;
          }
        }
        
        // First point
        if ((hDutyV[1] - hDutyV[0] - TDEAD)> MAX_TRTS)
        {
          dvDutyValues.hTimeSmp1 = (hDutyV[0] + hDutyV[1] + TDEAD) >> 1;
        }
        else
        {
          dvDutyValues.hTimeSmp1 = hDutyV[1] - TBEFORE;
        }
        // Second point
        dvDutyValues.hTimeSmp2 = PWM_PERIOD - HTMIN + TSAMPLE;
      }
      
      if (bStatorFluxPos == BOUNDARY_2) // Two big, one small
      {
        // First point
        if ((hDutyV[2] - hDutyV[1] - TDEAD)>= MAX_TRTS)
        {
          dvDutyValues.hTimeSmp1 = (hDutyV[1] + hDutyV[2] + TDEAD) >> 1;
        }
        else
        {
          dvDutyValues.hTimeSmp1 = hDutyV[2] - TBEFORE;
        }
        // Second point
        dvDutyValues.hTimeSmp2 = PWM_PERIOD - HTMIN + TSAMPLE;
      }
      
      if (bStatorFluxPos == BOUNDARY_3) // 
      {
        // First point
        dvDutyValues.hTimeSmp1 = hDutyV[0]-TBEFORE; // Dummy trigger
        // Second point
        dvDutyValues.hTimeSmp2 = PWM_PERIOD - HTMIN + TSAMPLE;
      }
    }
    else
    {
      bInverted_pwm_new = INVERT_NONE;
      bStatorFluxPos = REGULAR;
    }
        
    // Update DMA buffer Ch 1,2,3,4 (These value are required before update event)
    // This buffer is updated using DMA burst
    hCCRBuff[0] = dvDutyValues.hTimePhA;
    hCCRBuff[1] = dvDutyValues.hTimePhB;
    hCCRBuff[2] = dvDutyValues.hTimePhC;
    hCCRBuff[3] = dvDutyValues.hTimeSmp1;
    
/*******************************************************************************
* Function Name  : SVPWM_1ShuntNoPreloadAdj
* Description    :  Set the preload variables for PWM mode Ch 1,2,3,4	
* Input           : None
* Output         : None
* Return         : None
*******************************************************************************/
    //SVPWM_1ShuntNoPreloadAdj();
    // Set the preload vars for PWM mode Ch 1,2,3,4 (these value are required 
    // inside update event handler
    switch (bInverted_pwm_new)
    {
    case INVERT_A:
      // Preloads for CCMR
      hPreloadCCMR1Set = hPreloadCCMR1Disable | CH1TOGGLE | CH2NORMAL;
      hPreloadCCMR2Set = hPreloadCCMR2Disable | CH3NORMAL | CH4TOGGLE;
      break;
    case INVERT_B:
      // Preloads for CCMR
      hPreloadCCMR1Set = hPreloadCCMR1Disable | CH1NORMAL | CH2TOGGLE;
      hPreloadCCMR2Set = hPreloadCCMR2Disable | CH3NORMAL | CH4TOGGLE;
      break;
    case INVERT_C:
      // Preloads for CCMR
      hPreloadCCMR1Set = hPreloadCCMR1Disable | CH1NORMAL | CH2NORMAL;
      hPreloadCCMR2Set = hPreloadCCMR2Disable | CH3TOGGLE | CH4TOGGLE;
      break;
    default:
      // Preloads for CCMR
      hPreloadCCMR1Set = hPreloadCCMR1Disable | CH1NORMAL | CH2NORMAL;
      hPreloadCCMR2Set = hPreloadCCMR2Disable | CH3NORMAL | CH4NORMAL;
      break;
    }
    
    // Limit for update event
    
    // The following instruction can be executed after Update handler
    // before the get phase current (Second EOC)
    
    // Set the current sampled
     if (bStatorFluxPos == REGULAR) // Regual zone
    {  
      switch (bSector)
      {
      case SECTOR_1: // Fisrt after C, Second after B
          csCurrentSampled.sampCur1 = SAMP_NIC;
          csCurrentSampled.sampCur2 = SAMP_IA;
          break;
      case SECTOR_2: // Fisrt after C, Second after A 
          csCurrentSampled.sampCur1 = SAMP_NIC;
          csCurrentSampled.sampCur2 = SAMP_IB;
          break;
      case SECTOR_3: // Fisrt after A, Second after C
          csCurrentSampled.sampCur1 = SAMP_NIA;
          csCurrentSampled.sampCur2 = SAMP_IB;
          break;
      case SECTOR_4: // Fisrt after A, Second after B
          csCurrentSampled.sampCur1 = SAMP_NIA;
          csCurrentSampled.sampCur2 = SAMP_IC;
          break;
      case SECTOR_5: // Fisrt after B, Second after A
          csCurrentSampled.sampCur1 = SAMP_NIB;
          csCurrentSampled.sampCur2 = SAMP_IC;
          break;
      case SECTOR_6: // Fisrt after B, Second after C
          csCurrentSampled.sampCur1 = SAMP_NIB;
          csCurrentSampled.sampCur2 = SAMP_IA;
          break;
      }
    }
    
    if (bStatorFluxPos == BOUNDARY_1) // Two small, one big
    {
      switch (bSector)
      {
      case SECTOR_1:    // Phase B is adjusted
      case SECTOR_2:    
          csCurrentSampled.sampCur1 = SAMP_NIC;
          csCurrentSampled.sampCur2 = SAMP_IB;
          break;

      case SECTOR_3:    // Phase C is adjusted 
      case SECTOR_4:    
          csCurrentSampled.sampCur1 = SAMP_NIA;
          csCurrentSampled.sampCur2 = SAMP_IC;
          break;

      case SECTOR_5:   // Phase A is adjusted 
      case SECTOR_6:    
          csCurrentSampled.sampCur1 = SAMP_NIB;
          csCurrentSampled.sampCur2 = SAMP_IA;
          break;
      }
    }
    
    if (bStatorFluxPos == BOUNDARY_2) // Two big, one small
    {
      switch (bSector)
      {
      case SECTOR_2: // Phase A is adjusted
      case SECTOR_3:
          csCurrentSampled.sampCur1 = SAMP_IB;
          csCurrentSampled.sampCur2 = SAMP_IA;
          break;     
      case SECTOR_4: // Phase B is adjusted
      case SECTOR_5:
          csCurrentSampled.sampCur1 = SAMP_IC;
          csCurrentSampled.sampCur2 = SAMP_IB;
          break;  
      case SECTOR_6: // Phase C is adjusted
      case SECTOR_1:
          csCurrentSampled.sampCur1 = SAMP_IA;
          csCurrentSampled.sampCur2 = SAMP_IC;
          break;    
      }
    }
    
    if (bStatorFluxPos == BOUNDARY_3)  
    {
      if (bInverted_pwm_new == INVERT_A)
      {
        csCurrentSampled.sampCur1 = SAMP_OLDB;
        csCurrentSampled.sampCur2 = SAMP_IA;
      }
      if (bInverted_pwm_new == INVERT_B)
      {
        csCurrentSampled.sampCur1 = SAMP_OLDA;
        csCurrentSampled.sampCur2 = SAMP_IB;
      }
    }
    
	#ifdef CURRENT_COMPENSATION
	    // Check for distortion compensation entering Boudary2
	    if ((bStatorFluxPosOld == REGULAR) && (bStatorFluxPos == BOUNDARY_2) && (State == RUN))
	    {
	      bReadDelta = 1;
	    }
	    else
	    {
	      bReadDelta = 0;
	    }
	#endif
    
    // Deleting Delta if Stator Pos is no more BOUDARY_2
    if ((bStatorFluxPosOld == BOUNDARY_2) && (bStatorFluxPos != bStatorFluxPosOld) 
        && (State == RUN))
    {
      hDeltaA = 0;
      hDeltaB = 0;
      hDeltaC = 0;
    }
    
    // Limit for the Get Phase current (Second EOC Handler)
}

/*******************************************************************************
* Function Name  : SVPWM_1ShuntGetPhaseCurrentValues
* Description    : This function computes current values of Phase A and Phase B 
*                 in q1.15 format starting from values acquired from the A/D 
*                 Converter peripheral.
* Input          : None
* Output         : Stat_Curr_a_b
* Return         : None
*******************************************************************************/
Curr_Components SVPWM_1ShuntGetPhaseCurrentValues(void)
{
    Curr_Components Local_Stator_Currents;
    s32 wAux;
    s16 hCurrA = 0, hCurrB = 0, hCurrC = 0;
    u8 bCurrASamp = 0, bCurrBSamp = 0, bCurrCSamp = 0;

    
    if (csCurrentSampled.sampCur1 == SAMP_OLDA)
    {
      hCurrA = hCurrAOld;
      bCurrASamp = 1;
    }
    
    if (csCurrentSampled.sampCur1 == SAMP_OLDB)
    {
      hCurrB = hCurrBOld;
      bCurrBSamp = 1;
    }
    
    // First sampling point
    wAux =  (s32)(ADC1->JDR2 << 1) - (s32)(hPhaseOffset);
    
    switch (csCurrentSampled.sampCur1)
    {
    case SAMP_IA:
    case SAMP_IB:
    case SAMP_IC:
            break;
    case SAMP_NIA:
    case SAMP_NIB:
    case SAMP_NIC:
            wAux = -wAux; 
            break;
    default:
            wAux = 0;
    }
    
    // Check saturation
    if (wAux < S16_MIN)
    {
            wAux = S16_MIN;
    }  
    else  if (wAux > S16_MAX)
    { 
            wAux = S16_MAX;
    }
    else
    {
            wAux = (s16)(wAux);
    }
    
    switch (csCurrentSampled.sampCur1)
    {
    case SAMP_IA:
    case SAMP_NIA:
            hCurrA = (s16)(wAux);
            bCurrASamp = 1;
            break;
    case SAMP_IB:
    case SAMP_NIB:
            hCurrB = (s16)(wAux);
            bCurrBSamp = 1;
            break;
    case SAMP_IC:
    case SAMP_NIC:
            hCurrC = (s16)(wAux);
            bCurrCSamp = 1;
            break;
    }
    
    // Second sampling point
    wAux = (s32)(ADC1->JDR1 << 1) - (s32)(hPhaseOffset);
    
    switch (csCurrentSampled.sampCur2)
    {
    case SAMP_IA:
    case SAMP_IB:
    case SAMP_IC:
            break;
    case SAMP_NIA:
    case SAMP_NIB:
    case SAMP_NIC:
            wAux = -wAux; 
            break;
    default:
            wAux = 0;
    }
    
    // Check saturation
    if (wAux < S16_MIN)
    {
            wAux = S16_MIN;
    }  
    else  if (wAux > S16_MAX)
    { 
            wAux = S16_MAX;
    }
    else
    {
            wAux = (s16)(wAux);
    }
    
    switch (csCurrentSampled.sampCur2)
    {
    case SAMP_IA:
    case SAMP_NIA:
            hCurrA = (s16)(wAux);
            bCurrASamp = 1;
            break;
    case SAMP_IB:
    case SAMP_NIB:
            hCurrB = (s16)(wAux);
            bCurrBSamp = 1;
            break;
    case SAMP_IC:
    case SAMP_NIC:
            hCurrC = (s16)(wAux);
            bCurrCSamp = 1;
            break;
    }
    
    // Computation of the third value
    if (bCurrASamp == 0)
            hCurrA = -hCurrB -hCurrC;
    if (bCurrBSamp == 0)
            hCurrB = -hCurrA -hCurrC;
    if (bCurrCSamp == 0)
            hCurrC = -hCurrA -hCurrB;
    
    // hCurrA, hCurrB, hCurrC values are the sampled values
    
	#ifdef CURRENT_COMPENSATION
	    if (bReadDelta == 1)  
	    {
	      hDeltaA = hCurrAOld - hCurrA;
	      hDeltaB = hCurrBOld - hCurrB;
	      hDeltaC = hCurrCOld - hCurrC;
	    }
      
	    if (bStatorFluxPos == BOUNDARY_2)
	    {
	      hCurrA += hDeltaA;
	      hCurrB += hDeltaB;
	      hCurrC += hDeltaC;
	    }
	#endif
    
    hCurrAOld = hCurrA;
    hCurrBOld = hCurrB;
    hCurrCOld = hCurrC;

    Local_Stator_Currents.qI_Component1 = hCurrA;
    Local_Stator_Currents.qI_Component2 = hCurrB;
    
    return(Local_Stator_Currents); 
}

/*******************************************************************************
* Function Name  : SVPWM_1ShuntAdvCurrentReading
* Description    :  It is used to enable or disable the current reading.
* Input          : cmd (ENABLE or DISABLE)
* Output         : None
* Return         : None
*******************************************************************************/
void SVPWM_1ShuntAdvCurrentReading(FunctionalState cmd)
{
  if (cmd == ENABLE)
  {
    // Enable UPDATE ISR
    // Clear Update Flag
    TIM_ClearFlag(TIM1, TIM_FLAG_Update);
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
    
    // Distortion for single shunt enabling
    bDistEnab = 1;
  }
  else
  {
    // Disable UPDATE ISR
    TIM_ITConfig(TIM1, TIM_IT_Update, DISABLE);

    // Sync ADC trigger with Update
    ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_TRGO);

	// Enabling the Injectec conversion for ADC1
  	ADC_ExternalTrigInjectedConvCmd(ADC1,ENABLE);
    
    // Clear pending bit and Enable the EOC ISR
    ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);
    ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
    
    // Distortion for single shunt disabling
    bDistEnab = 0;
    
    // Disabling the last setting of PWM Mode and Duty Register
    hPreloadCCMR1Set = hPreloadCCMR1Disable | CH1NORMAL | CH2NORMAL;
    hPreloadCCMR2Set = hPreloadCCMR2Disable | CH3NORMAL | CH4NORMAL;
    TIM1->CCMR1 = hPreloadCCMR1Set; // Switch to Normal 
    TIM1->CCMR2 = hPreloadCCMR2Set; // Switch to Normal
    
    // Disabling all DMA previous setting
    TIM_DMACmd(TIM1, TIM_DMA_CC1, DISABLE);
    TIM_DMACmd(TIM1, TIM_DMA_CC2, DISABLE);
    TIM_DMACmd(TIM1, TIM_DMA_CC3, DISABLE);
    
    // Set all duty to 50%
    TIM1->CCR1 = PWM_PERIOD >> 1;
    TIM1->CCR2 = PWM_PERIOD >> 1;
    TIM1->CCR3 = PWM_PERIOD >> 1;    
  }
}

/*******************************************************************************
* Function Name  : SVPWMEOCEvent
* Description    :  Routine to be performed inside the end of conversion ISR
			store the first sampled value and compute the bus voltage and temperature
			sensor sampling  and disable the ext. adc triggering.
* Input           : None
* Output         : Return false after first EOC, return true after second EOC
* Return         : None
*******************************************************************************/
u8 SVPWMEOCEvent(void)
{
  //if (bDistEnab == 1)
 // {
    // Diabling the Injectec conversion for ADC1
 //   ADC_ExternalTrigInjectedConvCmd(ADC1,DISABLE);
 // }
  
  // Store the Bus Voltage and temperature sampled values
  //h_ADCTemp = ADC_GetInjectedConversionValue(ADC2,ADC_InjectedChannel_1);
 // h_ADCBusvolt = ADC_GetInjectedConversionValue(ADC2,ADC_InjectedChannel_2);
  
  return ((u8)(1));
}

/*******************************************************************************
* Function Name  : SVPWMUpdateEvent
* Description    :  Routine to be performed inside the update event ISR. 
                    It is used to set the PWM output mode of the four channels 
                    (Toggle or PWM), enable or disable the DMA event for each 
                    channel, update the DMA buffers, update the DMA lenght and 
                    finally it re-enables the external ADC triggering.	
* Input           : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVPWMUpdateEvent(void)
{
  if (bInverted_pwm_new != bInverted_pwm)  
  {
    // Critical point start
    
    // Update CCMR (OC Mode)
    TIM1->CCMR1 = hPreloadCCMR1Disable; // Switch to Frozen
    TIM1->CCMR1 = hPreloadCCMR1Set; // Switch to Normal or Toggle
    
    TIM1->CCMR2 = hPreloadCCMR2Disable; // Switch to Frozen
    TIM1->CCMR2 = hPreloadCCMR2Set; // Switch to Normal or Toggle
    
    // Disable DMA (in this period is not inverted)
    switch (bInverted_pwm)
    {
    case INVERT_A:
      //TIM1_DMACmd(TIM1_DMA_CC1, DISABLE);
      TIM1->DIER &= ~TIM_DMA_CC1;
      DMA1_Channel2->CCR &= CCR_ENABLE_Reset;
      TIM1->CCR1 = hCCRBuff[0];
      break;
    case INVERT_B:
      //TIM1_DMACmd(TIM1_DMA_CC2, DISABLE);
      TIM1->DIER &= ~TIM_DMA_CC2;
      DMA1_Channel3->CCR &= CCR_ENABLE_Reset;
      TIM1->CCR2 = hCCRBuff[1];
      break;
    case INVERT_C:
      //TIM1_DMACmd(TIM1_DMA_CC3, DISABLE);
      TIM1->DIER &= ~TIM_DMA_CC3;
      DMA1_Channel6->CCR &= CCR_ENABLE_Reset;
      TIM1->CCR3 = hCCRBuff[2];
      break;
    default:
      break;
    }
    
    // Enable DMA (in this period channel is toggled)
    switch (bInverted_pwm_new)
    {
    case INVERT_A:
      DMA1_Channel2->CCR &= CCR_ENABLE_Reset;
      DMA1_Channel2->CNDTR = 4;
      DMA1_Channel2->CCR |= CCR_ENABLE_Set;
      
      //TIM1_DMACmd(TIM1_DMA_CC1, ENABLE); 
      TIM1->DIER |= TIM_DMA_CC1;  
      
      if (bInverted_pwm_new == INVERT_A)
      {
        if (hCCRBuff[0] <= MINTIMCNTUPHAND)
        {
          TIM1->CCMR1 = TIM1->CCMR1 & 0xFF8F; // Switch to Frozen Ch1
          TIM1->CCMR1 = TIM1->CCMR1 | 0x0040; // Force Low Ch1
          TIM1->CCMR1 = TIM1->CCMR1 & 0xFF8F; // Switch to Frozen Ch1
          TIM1->CCMR1 = TIM1->CCMR1 | 0x0030; // Switch to Toggle Ch1
          
          //TIM1_GenerateEvent(TIM1_EventSource_CC1);
          TIM1->EGR |= TIM_EventSource_CC1;
        }
      }
      break;
      
    case INVERT_B:
      DMA1_Channel3->CCR &= CCR_ENABLE_Reset;
      DMA1_Channel3->CNDTR = 4;
      DMA1_Channel3->CCR |= CCR_ENABLE_Set;
      
      //TIM1_DMACmd(TIM1_DMA_CC2, ENABLE); 
      TIM1->DIER |= TIM_DMA_CC2;
      
      if (bInverted_pwm_new == INVERT_B)
      {
        if (hCCRBuff[1] <= MINTIMCNTUPHAND)
        {
          TIM1->CCMR1 = TIM1->CCMR1 & 0x8FFF; // Switch to Frozen Ch2
          TIM1->CCMR1 = TIM1->CCMR1 | 0x4000; // Force Low Ch2
          TIM1->CCMR1 = TIM1->CCMR1 & 0x8FFF; // Switch to Frozen Ch2
          TIM1->CCMR1 = TIM1->CCMR1 | 0x3000; // Switch to Toggle Ch2
          
          //TIM1_GenerateEvent(TIM1_EventSource_CC2);
          TIM1->EGR |= TIM_EventSource_CC2;
        }
      }
      break;
      
    case INVERT_C:
      DMA1_Channel6->CCR &= CCR_ENABLE_Reset;
      DMA1_Channel6->CNDTR = 4;
      DMA1_Channel6->CCR |= CCR_ENABLE_Set;
      
      //TIM1_DMACmd(TIM1_DMA_CC3, ENABLE);
      TIM1->DIER |= TIM_DMA_CC3;
      
      if (bInverted_pwm_new == INVERT_C)
      {
        if (hCCRBuff[2] <= MINTIMCNTUPHAND)
        {
          TIM1->CCMR2 = TIM1->CCMR2 & 0xFF8F; // Switch to Frozen Ch3
          TIM1->CCMR2 = TIM1->CCMR2 | 0x0040; // Force Low Ch3
          TIM1->CCMR2 = TIM1->CCMR2 & 0xFF8F; // Switch to Frozen Ch3
          TIM1->CCMR2 = TIM1->CCMR2 | 0x0030; // Switch to Toggle Ch3
          
          //TIM1_GenerateEvent(TIM1_EventSource_CC3);
          TIM1->EGR |= TIM_EventSource_CC3;
        }
      }
      break;
      
    default:
      break;
    }
    
    // Critical point stop
    
    // Adjust the DMA lenght for Ch4
    //DMA_Cmd(DMA_Channel4, DISABLE);
    DMA1_Channel4->CCR &= CCR_ENABLE_Reset;
    if (bInverted_pwm_new == INVERT_NONE)
    { 
      // Length 2
      DMA1_Channel4->CNDTR = 2;
    }
    else
    {
      // Length 4
      DMA1_Channel4->CNDTR = 4;
    }
    //DMA_Cmd(DMA_Channel4, ENABLE);
    DMA1_Channel4->CCR |= CCR_ENABLE_Set;
  }
  
  switch (bInverted_pwm_new)
  {
  case INVERT_A: 
    if (hCCRBuff[0] <= DMABURSTMIN_A)
    {
      // Reset the status
      TIM1->CCMR1 = TIM1->CCMR1 & 0xFF8F; // Switch to Frozen Ch1
      TIM1->CCMR1 = TIM1->CCMR1 | 0x0040; // Force Low Ch1
      TIM1->CCMR1 = TIM1->CCMR1 & 0xFF8F; // Switch to Frozen Ch1
      TIM1->CCMR1 = TIM1->CCMR1 | 0x0030; // Switch to Toggle Ch1
      
      //TIM1_DMACmd(TIM1_DMA_CC1, ENABLE); 
      TIM1->DIER |= TIM_DMA_CC1;
      DMA1_Channel2->CCR &= CCR_ENABLE_Reset;
      DMA1_Channel2->CNDTR = 4;
      DMA1_Channel2->CCR |= CCR_ENABLE_Set;

      TIM_GenerateEvent(TIM1, TIM_EventSource_CC1);
      
      if (dvDutyValues.hTimePhA < DMABURSTMIN_A)
        dvDutyValues.hTimePhA = DMABURSTMIN_A;
    }
    break;  
      
  case INVERT_B:
    if (hCCRBuff[1] <= DMABURSTMIN_B)
    {
      // Reset the status
      TIM1->CCMR1 = TIM1->CCMR1 & 0x8FFF; // Switch to Frozen Ch2
      TIM1->CCMR1 = TIM1->CCMR1 | 0x4000; // Force Low Ch2
      TIM1->CCMR1 = TIM1->CCMR1 & 0x8FFF; // Switch to Frozen Ch2
      TIM1->CCMR1 = TIM1->CCMR1 | 0x3000; // Switch to Toggle Ch2
      
      //TIM1_DMACmd(TIM1_DMA_CC2, ENABLE); 
      TIM1->DIER |= TIM_DMA_CC2;
      DMA1_Channel3->CCR &= CCR_ENABLE_Reset;
      DMA1_Channel3->CNDTR = 4;
      DMA1_Channel3->CCR |= CCR_ENABLE_Set;

      TIM_GenerateEvent(TIM1, TIM_EventSource_CC2);
      
      if (dvDutyValues.hTimePhB < DMABURSTMIN_B)
        dvDutyValues.hTimePhB = DMABURSTMIN_B;
    }
    break;
  case INVERT_C: 
    if (hCCRBuff[2] <= DMABURSTMIN_C)
    {
      // Reset the status
      TIM1->CCMR2 = TIM1->CCMR2 & 0xFF8F; // Switch to Frozen Ch3
      TIM1->CCMR2 = TIM1->CCMR2 | 0x0040; // Force Low Ch3
      TIM1->CCMR2 = TIM1->CCMR2 & 0xFF8F; // Switch to Frozen Ch3
      TIM1->CCMR2 = TIM1->CCMR2 | 0x0030; // Switch to Toggle Ch3
      
      //TIM1_DMACmd(TIM1_DMA_CC3, ENABLE); 
      TIM1->DIER |= TIM_DMA_CC3;
      DMA1_Channel6->CCR &= CCR_ENABLE_Reset;
      DMA1_Channel6->CNDTR = 4;
      DMA1_Channel6->CCR |= CCR_ENABLE_Set;

      TIM_GenerateEvent(TIM1, TIM_EventSource_CC3);
      
      if (dvDutyValues.hTimePhC < DMABURSTMIN_C)
        dvDutyValues.hTimePhC = DMABURSTMIN_C;
    }
    break;
  }

  // Update remaining DMA buffer
  hCCDmaBuffCh4[0] = dvDutyValues.hTimeSmp2; // Second point 
  hCCDmaBuffCh4[1] = dvDutyValues.hTimeSmp2;
  hCCDmaBuffCh4[2] = dvDutyValues.hTimeSmp1; // First point
  hCCDmaBuffCh4[3] = dvDutyValues.hTimeSmp1;
  
  hCCDmaBuffCh1[2] = dvDutyValues.hTimePhA;
  hCCDmaBuffCh1[3] = dvDutyValues.hTimePhA;
  
  hCCDmaBuffCh2[2] = dvDutyValues.hTimePhB;
  hCCDmaBuffCh2[3] = dvDutyValues.hTimePhB;
  
  hCCDmaBuffCh3[2] = dvDutyValues.hTimePhC;
  hCCDmaBuffCh3[3] = dvDutyValues.hTimePhC;
  
  bInverted_pwm = bInverted_pwm_new;
  
  /* ADC1 Injected conversions trigger is TIM1 TRGO */ 
  ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_CC4); 
  
  // Enabling the Injectec conversion for ADC1
  ADC_ExternalTrigInjectedConvCmd(ADC1,ENABLE);
}


#endif

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/  
