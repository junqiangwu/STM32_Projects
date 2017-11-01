/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : stm32f10x_svpwm_ics.h
* Author             : IMS Systems Lab
* Date First Issued  : 30/05/07
* Description        : This file contains declaration of functions exported by 
*                      module stm32x_svpwm_ics.c
********************************************************************************
* History:
* 21/11/07 v1.0
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
#ifndef __STM32F10x_SVPWM_ICS_H
#define __STM32F10x_SVPWM_ICS_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "stm32f10x_MClib.h"
#include "MC_pwm_ics_prm.h"
#include "MC_const.h"
#include "MC_Control_Param.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void SVPWM_IcsInit(void);
Curr_Components SVPWM_IcsGetPhaseCurrentValues(void);
void SVPWM_IcsCalcDutyCycles (Volt_Components Stat_Volt_Input);
void SVPWM_IcsCurrentReadingCalibration(void);
u8 SVPWMEOCEvent(void);

#endif /* __STM32F10x_SVPWM_ICS_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
