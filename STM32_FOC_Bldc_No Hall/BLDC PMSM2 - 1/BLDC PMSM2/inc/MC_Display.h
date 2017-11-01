/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_Display.h
* Author             : IMS Systems Lab
* Date First Issued  : 21/11/07
* Description        : Contains the prototypes of the LCD display module related
*                      routines.
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
#ifndef __DISPLAY_H
#define __DISPLAY_H


/* Exported constants --------------------------------------------------------*/

#define CONTROL_MODE_MENU_1    (u8) 0
#define REF_SPEED_MENU         (u8) 1

#define P_SPEED_MENU           (u8) 2
#define I_SPEED_MENU           (u8) 3
#define D_SPEED_MENU           (u8) 4

#define P_TORQUE_MENU          (u8) 5
#define I_TORQUE_MENU          (u8) 6
#define D_TORQUE_MENU          (u8) 7

#define P_FLUX_MENU            (u8) 8
#define I_FLUX_MENU            (u8) 9
#define D_FLUX_MENU            (u8) 10

#define POWER_STAGE_MENU       (u8) 11

#define CONTROL_MODE_MENU_6    (u8) 12
#define IQ_REF_MENU            (u8) 13
#define ID_REF_MENU            (u8) 14

#define FAULT_MENU             (u8) 15

#define WAIT_MENU              (u8) 16

#ifdef OBSERVER_GAIN_TUNING
#define K1_MENU				   (u8) 17
#define K2_MENU				   (u8) 18
#define P_PLL_MENU			   (u8) 19
#define I_PLL_MENU			   (u8) 20
#endif

#ifdef DAC_FUNCTIONALITY
#define DAC_PB0_MENU		           (u8) 21
#define DAC_PB1_MENU                       (u8) 22
#endif

#ifdef FLUX_WEAKENING
#define P_VOLT_MENU             (u8)23
#define I_VOLT_MENU             (u8)24
#define TARGET_VOLT_MENU        (u8)25
#endif

/* Exported functions ------------------------------------------------------- */
void Display_LCD(void);
void Display_Welcome_Message(void);

#endif  /*__DISPLAY_H*/
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
