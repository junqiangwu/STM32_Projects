/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : stm32f10x_encoder.h
* Author             : IMS Systems Lab 
* Date First Issued  : 21/11/07
* Description        : This file contains the software implementation for the
*                      encoder position and speed reading.
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
#ifndef __STM32F10x_ENCODER_H
#define __STM32F10x_ENCODER_H

/* Includes ------------------------------------------------------------------*/
#include "MC_encoder_param.h"

/* Exported functions ------------------------------------------------------- */
void ENC_Init(void);
s16 ENC_Get_Electrical_Angle(void);
s16 ENC_Get_Mechanical_Angle(void);
void ENC_Clear_Speed_Buffer(void);
void ENC_ResetEncoder(void);
s16 ENC_Get_Mechanical_Speed(void);
void ENC_Calc_Average_Speed(void);
bool ENC_ErrorOnFeedback(void);
void ENC_Start_Up(void);

#endif  /*__STM32F10x_ENCODER_H*/
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
