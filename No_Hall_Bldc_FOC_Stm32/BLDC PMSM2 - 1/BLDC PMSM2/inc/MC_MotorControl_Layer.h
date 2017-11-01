/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_MotorControl_Layer.h
* Author             : IMS Systems Lab
* Date First Issued  : 21/11/07
* Description        : Export of public functions of Motor control layer 
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
#ifndef __MC_MOTORCONTROLLAYER_H
#define __MC_MOTORCONTROLLAYER_H

/* Includes ------------------------------------------------------------------*/
#include "MC_type.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
//Not to be modified
#define OVERVOLTAGE_THRESHOLD  (u16)(OVERVOLTAGE_THRESHOLD_V*\
                                                 (BUS_ADC_CONV_RATIO*32768/3.3))
#define UNDERVOLTAGE_THRESHOLD (u16)(UNDERVOLTAGE_THRESHOLD_V*\
                                                 (BUS_ADC_CONV_RATIO*32768/3.3))
/* Exported variables --------------------------------------------------------*/

extern u16 h_ADCBusvolt;
extern u16 h_ADCTemp;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void MCL_Init(void);
void MCL_ChkPowerStage(void);
bool MCL_ClearFault(void);
void MCL_SetFault(u16);
bool MCL_Chk_OverTemp(void);
BusV_t MCL_Chk_BusVolt(void);
u16 MCL_Compute_BusVolt(void);
u8 MCL_Compute_Temp(void);
void MCL_Calc_BusVolt(void);
s16 MCL_Get_BusVolt(void);
void MCL_Init_Arrays(void);
#ifdef BRAKE_RESISTOR
void MCL_Brake_Init(void);
void MCL_Set_Brake_On(void);
void MCL_Set_Brake_Off(void);
#endif
#endif //__MC_MOTORCONTROLLAYER_H
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
