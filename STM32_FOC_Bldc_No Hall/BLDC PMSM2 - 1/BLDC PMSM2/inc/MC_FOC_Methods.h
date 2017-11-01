/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name  	     : MC_FOC_Methods.h
* Author             : IMS Systems Lab 
* Date First Issued  : 29/05/08
* Description        : Contains the prototypes for the PMSM FOC methods module 
*                      related functions.
********************************************************************************
* History:
* 29/05/08 v2.0
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
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
#ifndef __MC_FOC_METHODS_H
#define __MC_FOC_METHODS_H 

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
Curr_Components FOC_FluxRegulator (Curr_Components,Volt_Components,s16);
void FOC_FluxRegulator_Init(PID_Struct_t *, s16);
s16 FOC_FluxRegulator_Update(s16, s16);
s16 FOC_MTPA(s16 hIqRef);
void FOC_MTPA_Init(MTPA_Const MTPA_InitStructure_in, s16 hIdDemag_in);
void FOC_FF_CurrReg_Init(s32 wConstant1Q, s32 wConstant1D, s32 wConstant2);
Volt_Components FOC_FF_CurrReg(Curr_Components, Volt_Components, s16 speed,s16 vbus);

#endif /* MC_FOC_METHODS_H */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
