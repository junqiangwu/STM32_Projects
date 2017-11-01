/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_type.h
* Author             : IMS Systems Lab 
* Date First Issued  : 21/11/07
* Description        : This header file provides structure type definitions that 
*                      are used throughout this motor control library.
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
#ifndef __MC_TYPE_H
#define __MC_TYPE_H

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

typedef struct 
{
  s16 qI_Component1;
  s16 qI_Component2;
} Curr_Components;

typedef struct 
{
  s16 qV_Component1;
  s16 qV_Component2;
} Volt_Components;

typedef struct
{
  s16 hCos;
  s16 hSin;
} Trig_Components;

typedef struct
{
 s16 hC1;
 s16 hC2;
 s16 hC3;
 s16 hC4;
 s16 hC5;
 s16 hC6;
 s16 hF1;
 s16 hF2;
 s16 hF3;
 s16 PLL_P;
 s16 PLL_I;
 s32 wMotorMaxSpeed_dpp;
 u16 hPercentageFactor;
} StateObserver_Const; 

typedef struct
{
  s16 PLL_P;
  s16 PLL_I;
  s16 hC2;
  s16 hC4;
} StateObserver_GainsUpdate;

typedef struct
{
 s16 hsegdiv;
 s32 wangc[8];
 s32 wofst[8];
} MTPA_Const;

typedef struct 
{  
  s16 hKp_Gain;
  u16 hKp_Divisor;
  s16 hKi_Gain;
  u16 hKi_Divisor;  
  s16 hLower_Limit_Output;     //Lower Limit for Output limitation
  s16 hUpper_Limit_Output;     //Lower Limit for Output limitation
  s32 wLower_Limit_Integral;   //Lower Limit for Integral term limitation
  s32 wUpper_Limit_Integral;   //Lower Limit for Integral term limitation
  s32 wIntegral;
  // Actually used only if DIFFERENTIAL_TERM_ENABLED is enabled in
  //stm32f10x_MCconf.h
  s16 hKd_Gain;
  u16 hKd_Divisor;
  s32 wPreviousError;
} PID_Struct_t;

typedef enum 
{
IDLE, INIT, START, RUN, STOP, BRAKE, WAIT, FAULT
} SystStatus_t;

typedef enum 
{
NO_FAULT, OVER_VOLT, UNDER_VOLT
} BusV_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MC_TYPE_H */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
