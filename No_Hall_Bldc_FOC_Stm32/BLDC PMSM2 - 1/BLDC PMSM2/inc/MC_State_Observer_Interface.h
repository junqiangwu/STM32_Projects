/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_State_Observer_Interface.h
* Author             : IMS Systems Lab 
* Date First Issued  : 21/11/07
* Description        : Contains the prototypes of the PMSM State Observer 
*                      related functions (module MC_State_Observer_Interface.c)
*
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
#ifndef __MC_STATE_OBSERVER_INTERFACE_H
#define __MC_STATE_OBSERVER_INTERFACE_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
s16 STO_Get_Mechanical_Angle(void);
s16 STO_Get_Speed_Hz(void);
void STO_Start_Up(void);
bool STO_Check_Speed_Reliability(void);
void STO_StateObserverInterface_Init(void);
#ifdef OBSERVER_GAIN_TUNING
void STO_Obs_Gains_Update(void);
#endif
#endif 
/* __MC_STATE_OBSERVER_INTERFACE_H */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
