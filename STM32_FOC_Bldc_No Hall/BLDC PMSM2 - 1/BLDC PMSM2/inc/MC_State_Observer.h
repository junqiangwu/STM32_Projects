/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_State_Observer.h
* Author             : IMS Systems Lab 
* Date First Issued  : 21/11/07
* Description        : Contains the prototypes of the PMSM Observer 
*                      related functions (module MC_State_Observer.c)
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
#ifndef __MC_STATE_OBSERVER_H
#define __MC_STATE_OBSERVER_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void STO_Calc_Rotor_Angle(Volt_Components,Curr_Components,s16);
void STO_Calc_Speed(void);
s16 STO_Get_Electrical_Angle(void);
s16 STO_Get_Speed(void);
bool STO_IsSpeed_Reliable(void);
void STO_Gains_Update(StateObserver_GainsUpdate*);
void STO_Init(void);
void STO_InitSpeedBuffer(void);
void STO_Gains_Init(StateObserver_Const*);
s16 STO_Get_wIalfa_est(void);
s16 STO_Get_wIbeta_est(void);
s16 STO_Get_wBemf_alfa_est(void);
s16 STO_Get_wBemf_beta_est(void);
#endif 
/* __MC_STATE_OBSERVER_H */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
