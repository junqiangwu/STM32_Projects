/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : stm32f10x_MCdac.h
* Author             : IMS Systems Lab 
* Date First Issued  : 21/11/07
* Description        : It contains prototypes and definitions necessary for 
*                      handling DAC functionality from LCD display and joystick
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
#ifndef __STM32F10x_MCDAC_H
#define __STM32F10x_MCDAC_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define I_A           (u8)(1)               
#define I_B           (u8)(2)
#define I_ALPHA       (u8)(3)
#define I_BETA        (u8)(4)
#define I_Q           (u8)(5)
#define I_D           (u8)(6)
#define I_Q_REF       (u8)(7)
#define I_D_REF       (u8)(8)
#define V_Q           (u8)(9)
#define V_D           (u8)(10)
#define V_ALPHA       (u8)(11)
#define V_BETA        (u8)(12)
#define SENS_ANGLE    (u8)(13)
#define SENS_SPEED    (u8)(14)
#define LO_ANGLE      (u8)(15)
#define LO_SPEED      (u8)(16)
#define LO_I_A        (u8)(17)
#define LO_I_B        (u8)(18)
#define LO_BEMF_A     (u8)(19)
#define LO_BEMF_B     (u8)(20)
#define USER_1        (u8)(21)
#define USER_2        (u8)(22)
#define DAC_CH1       (u8)(1)
#define DAC_CH2       (u8)(2)

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void MCDAC_Init(void);
void MCDAC_Update_Output(void);
void MCDAC_Update_Value(u8,s16);
void MCDAC_Output_Choice(s8,u8);
u8 *MCDAC_Output_Var_Name(u8);

#endif 
/* __STM32F10x_MCDAC_H */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
