/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_hall_prm.h
* Author             : FOCSDK development tool ver. 1.0.0 by IMS Systems Lab
* Creation date      : Mon Aug 26 21:58:31 2013
* Description        : Contains the list of project specific parameters related
*                      to the Hall sensors speed feedback.
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
#ifndef __HALL_PRM_H
#define __HALL_PRM_H
/* Includes ------------------------------------------------------------------*/
#include "STM32F10x_MCconf.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* APPLICATION SPECIFIC DEFINE -----------------------------------------------*/
/* Define here the 16-bit timer chosen to handle hall sensors feedback  */
/* Timer 2 is the mandatory selection when using STM32MC-KIT  */
//#define TIMER2_HANDLES_HALL
#define TIMER3_HANDLES_HALL
//#define TIMER4_HANDLES_HALL

/* HALL SENSORS PLACEMENT ----------------------------------------------------*/
#define	DEGREES_120 0
#define	DEGREES_60 1

/* Define here the mechanical position of the sensors with reference to an 
                                                             electrical cycle */ 
#define HALL_SENSORS_PLACEMENT DEGREES_120

/* Define here in degrees the electrical phase shift between the low to high
transition of signal H1 and the maximum of the Bemf induced on phase A */

#define	HALL_PHASE_SHIFT (s16) 300

/* APPLICATION SPEED DOMAIN AND ERROR/RANGE CHECKING -------------------------*/

/* Define here the rotor mechanical frequency above which speed feedback is not 
realistic in the application: this allows discriminating glitches for instance 
*/
#define	HALL_MAX_SPEED_FDBK_RPM ((u32)30000)

/* Define here the returned value if measured speed is > MAX_SPEED_FDBK_RPM
It could be 0 or FFFF depending on upper layer software management */
#define HALL_MAX_SPEED               ((u16)5000) // Unit is 0.1Hz
// With digit-per-PWM unit (here 2*PI rad = 0xFFFF):
#define HALL_MAX_PSEUDO_SPEED        ((s16)-32768)

/* Define here the rotor mechanical frequency below which speed feedback is not 
realistic in the application: this allows to discriminate too low freq for 
instance */
#define	HALL_MIN_SPEED_FDBK_RPM ((u16)60)

/* Max TIM prescaler ratio defining the lowest expected speed feedback */
#define HALL_MAX_RATIO		((u16)800u)

/* Number of consecutive timer overflows without capture: this can indicate
that informations are lost or that speed is decreasing very sharply */
/* This is needed to implement hall sensors time-out. This duration depends on hall sensor
timer pre-scaler, which is variable; the time-out will be higher at low speed*/
#ifdef FLUX_TORQUE_PIDs_TUNING
#define HALL_MAX_OVERFLOWS       ((u16)4)
#else
#define HALL_MAX_OVERFLOWS       ((u16)2)
#endif

/* ROLLING AVERAGE DEPTH -----------------------------------------------------*/
#ifdef FLUX_TORQUE_PIDs_TUNING
#define HALL_SPEED_FIFO_SIZE 	((u8)1)
#else
#define HALL_SPEED_FIFO_SIZE 	((u8)6)
#endif

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __HALL_PRM_H */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
