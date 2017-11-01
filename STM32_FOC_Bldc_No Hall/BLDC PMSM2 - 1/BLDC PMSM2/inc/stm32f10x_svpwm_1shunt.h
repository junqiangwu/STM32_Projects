/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : stm32f10x_svpwm_1shunt.h
* Author             : IMS Systems Lab
* Date First Issued  : 29/05/08
* Description        : This file contains declaration of functions exported by 
*                      module stm32x_svpwm_1shunt.c
********************************************************************************
* History:
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
#ifndef __STM32F10x_SVPWM_1SHUNT_H
#define __STM32F10x_SVPWM_1SHUNT_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "stm32f10x_MClib.h"
#include "MC_pwm_1shunt_prm.h"
#include "MC_const.h"
#include "MC_Control_Param.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

#define INVERT_NONE 0
#define INVERT_A 1
#define INVERT_B 2
#define INVERT_C 3

typedef struct
{
	u8 sampCur1;
	u8 sampCur2;
} CURRENTSAMPLEDTYPE;

#define SAMP_NO 0
#define SAMP_IA 1
#define SAMP_IB 2
#define SAMP_IC 3
#define SAMP_NIA 4
#define SAMP_NIB 5
#define SAMP_NIC 6
#define SAMP_OLDA 7
#define SAMP_OLDB 8
#define SAMP_OLDC 9

typedef struct 
{
	u16 hTimePhA;
	u16 hTimePhB;
	u16 hTimePhC;
	u16 hTimeSmp1;
	u16 hTimeSmp2;
} DUTYVALUESTYPE;

#define MAX(a,b) ((a)>(b))?(a):(b)

#define DMABURSTMIN_A 16
#define DMABURSTMIN_B 23
#define DMABURSTMIN_C 35

/* DMA ENABLE mask */
#define CCR_ENABLE_Set          ((u32)0x00000001)
#define CCR_ENABLE_Reset        ((u32)0xFFFFFFFE)

#define MINTIMCNTUPHAND ((u16)(60))
#define MAXTIMCNTUPHAND ((u16)(220))

#define MIDTIMCNTUPHAND ((MAXTIMCNTUPHAND+MINTIMCNTUPHAND) >> 1)

/* Exported functions ------------------------------------------------------- */
void SVPWM_1ShuntCalcDutyCycles (Volt_Components Stat_Volt_Input);
Curr_Components SVPWM_1ShuntGetPhaseCurrentValues(void);

void SVPWM_1ShuntInit(void);
void SVPWM_1ShuntCurrentReadingCalibration(void);
void SVPWM_1ShuntAdvCurrentReading(FunctionalState cmd);

void SVPWMUpdateEvent(void);
u8 SVPWMEOCEvent(void);

#endif /* __STM32F10x_SVPWM_1SHUNT_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
