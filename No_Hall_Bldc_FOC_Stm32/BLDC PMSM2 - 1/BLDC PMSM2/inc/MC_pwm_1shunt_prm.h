/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_pwm_1shunt_prm.h
* Author             : FOCSDK development tool ver. 1.0.0 by IMS Systems Lab
* Creation date      : Mon Aug 26 21:58:31 2013
* Description        : Contains the list of project specific parameters related
*                      to the single-shunt current reading.
********************************************************************************
* History:
* Mar/07 v1.0
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MC_PWM_1SHUNT_PRM_H
#define __MC_PWM_1SHUNT_PRM_H

/////////////////////// PWM Peripheral Input clock ////////////////////////////
#define CKTIM	((u32)72000000uL) 	/* Silicon running at 72MHz Resolution: 1Hz */

////////////////////// PWM Frequency ///////////////////////////////////

/****	 Pattern type is center aligned  ****/

	#define PWM_PRSC ((u8)0)

        /* Resolution: 1Hz */                            
	#define PWM_PERIOD ((u16) (CKTIM / (u32)(2 * PWM_FREQ *(PWM_PRSC+1)))) 
        
////////////////////////////// Deadtime Value /////////////////////////////////
	#define DEADTIME  (u16)((unsigned long long)CKTIM/2 \
          *(unsigned long long)DEADTIME_NS/1000000000uL) 

///////////////////////////// Current reading parameters //////////////////////

#define PHASE_A_ADC_CHANNEL     ADC_Channel_11
#define PHASE_A_GPIO_PORT       GPIOC
#define PHASE_A_GPIO_PIN        GPIO_Pin_1

#define PHASE_B_ADC_CHANNEL     ADC_Channel_12
#define PHASE_B_GPIO_PORT       GPIOC
#define PHASE_B_GPIO_PIN        GPIO_Pin_2

#define PHASE_C_ADC_CHANNEL     ADC_Channel_13
#define PHASE_C_GPIO_PORT       GPIOC
#define PHASE_C_GPIO_PIN        GPIO_Pin_3

/****************** Control related define ***************************/
//#define CURRENT_COMPENSATION // Enable the current compensation

#define SAMPLING_TIME_NS   200  //200ns
//#define SAMPLING_TIME_NS   700  //700ns
//#define SAMPLING_TIME_NS  1200  //1.2us
//#define SAMPLING_TIME_NS  2450  //2.45us

#if (SAMPLING_TIME_NS == 200)
#define SAMPLING_TIME_CK  ADC_SampleTime_1Cycles5
#elif (SAMPLING_TIME_NS == 700)
#define SAMPLING_TIME_CK  ADC_SampleTime_7Cycles5
#elif (SAMPLING_TIME_NS == 1200)
#define SAMPLING_TIME_CK  ADC_SampleTime_13Cycles5
#elif (SAMPLING_TIME_NS == 2450)
#define SAMPLING_TIME_CK  ADC_SampleTime_28Cycles5
#else
#warning "Sampling time is not a possible value"
#endif

#define TRISE_NS 2550

#define SAMPLING_TIME (u16)(((u16)(SAMPLING_TIME_NS) * 72uL)/1000uL) 
#define TRISE (u16)((((u16)(TRISE_NS)) * 72uL)/1000uL)
#define TDEAD (u16)((DEADTIME_NS * 72uL)/1000uL)

#define TMIN (((u16)(((DEADTIME_NS+((u16)(TRISE_NS))+((u16)(SAMPLING_TIME_NS)))*72uL)/1000ul))+1)
#define HTMIN (u16)(TMIN >> 1)
#define TSAMPLE SAMPLING_TIME
#define TAFTER ((u16)(((DEADTIME_NS+((u16)(TRISE_NS)))*72ul)/1000ul))
#define TBEFORE (((u16)(((((u16)(SAMPLING_TIME_NS)))*72ul)/1000ul))+1)

#if (TRISE_NS > SAMPLING_TIME_NS)
  #define MAX_TRTS (2 * TRISE)
#else
  #define MAX_TRTS (2 * SAMPLING_TIME)
#endif

/////////////////  Power Stage management Conversions setting ////////////////////////

#define TEMP_FDBK_CHANNEL                 ADC_Channel_10
#define TEMP_FDBK_CHANNEL_GPIO_PORT       GPIOC
#define TEMP_FDBK_CHANNEL_GPIO_PIN        GPIO_Pin_0

#define BUS_VOLT_FDBK_CHANNEL             ADC_Channel_3
#define BUS_VOLT_FDBK_CHANNEL_GPIO_PORT   GPIOA
#define BUS_VOLT_FDBK_CHANNEL_GPIO_PIN    GPIO_Pin_3

#endif  /*__MC_PWM_1SHUNT_PRM_H*/
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
