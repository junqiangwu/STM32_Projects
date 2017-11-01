/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : stm32f10x_MClib.h
* Author             : IMS Systems Lab 
* Date First Issued  : 21/11/07
* Description        : This file gathers the motor control header files which 
*                      are needed depending on configuration.
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
#ifndef __STM32F10xMCLIB_H
#define __STM32F10xMCLIB_H

/* Includes ------------------------------------------------------------------*/

#include "stm32f10x_MCconf.h"
#include "MC_type.h"
#include "stm32f10x_lib.h"

#if defined HALL_SENSORS 
#include "stm32f10x_hall.h"
#define GET_ELECTRICAL_ANGLE    HALL_GetElectricalAngle()
#define GET_SPEED_0_1HZ         HALL_GetSpeed()
#define GET_SPEED_DPP           HALL_GetRotorFreq()
#endif

#if defined ENCODER
#include "stm32f10x_encoder.h"
#define GET_ELECTRICAL_ANGLE    ENC_Get_Electrical_Angle()
#define GET_SPEED_0_1HZ         ENC_Get_Mechanical_Speed()
#define GET_SPEED_DPP    (s16)((ENC_Get_Mechanical_Speed()*_0_1HZ_2_128DPP)/128)
#endif

#if defined NO_SPEED_SENSORS 
#include "MC_State_Observer.h"
#include "MC_State_Observer_param.h"
#include "MC_State_Observer_interface.h"
#define GET_ELECTRICAL_ANGLE    STO_Get_Electrical_Angle()
#define GET_SPEED_0_1HZ         STO_Get_Speed_Hz()
#define GET_SPEED_DPP           STO_Get_Speed()
#endif

#if defined OBSERVER_GAIN_TUNING
#include "MC_State_Observer.h"
#include "MC_State_Observer_param.h"
#include "MC_State_Observer_interface.h"
#endif

#if defined VIEW_HALL_FEEDBACK
#include "stm32f10x_hall.h"
#endif

#if defined VIEW_ENCODER_FEEDBACK
#include "stm32f10x_encoder.h"
#endif

#ifdef ICS_SENSORS
#include "stm32f10x_svpwm_ics.h"
#define GET_PHASE_CURRENTS SVPWM_IcsGetPhaseCurrentValues
#define CALC_SVPWM SVPWM_IcsCalcDutyCycles
#endif

#ifdef THREE_SHUNT
#include "stm32f10x_svpwm_3shunt.h"
#define GET_PHASE_CURRENTS SVPWM_3ShuntGetPhaseCurrentValues
#define CALC_SVPWM SVPWM_3ShuntCalcDutyCycles
#endif

#ifdef SINGLE_SHUNT
#include "stm32f10x_svpwm_1shunt.h"
#define GET_PHASE_CURRENTS SVPWM_1ShuntGetPhaseCurrentValues
#define CALC_SVPWM SVPWM_1ShuntCalcDutyCycles
#endif

#ifdef DAC_FUNCTIONALITY
#include "stm32f10x_MCdac.h"
#endif

#include "MC_Clarke_Park.h"
#include "MC_FOC_Drive.h"
#include "MC_FOC_Methods.h"
#include "MC_PID_regulators.h"
#include "MC_Control_Param.h"
#include "stm32f10x_Timebase.h"
#include "MC_Display.h"
#include "MC_Keys.h"
#include "stm32_2.8_lcd.h"
#include "MC_PMSM_motor_param.h"
#include "MC_MotorControl_Layer.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __STM32F10xMCLIB_H */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
