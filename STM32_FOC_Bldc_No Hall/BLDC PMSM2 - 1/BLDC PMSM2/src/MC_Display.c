/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_Display.c
* Author             : IMS Systems Lab 
* Date First Issued  : 21/11/07
* Description        : This file contains the software implementation of the
*                      display routines
********************************************************************************
* History:
* 21/11/07 v1.0
* 29/05/08 v2.0
* 14/07/08 v2.0.1
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
/* Standard include ----------------------------------------------------------*/
#include "stm32f10x_lib.h"  

/* Include of other module interface headers ---------------------------------*/
/* Local includes ------------------------------------------------------------*/

#include "stm32f10x_MClib.h"
#include "MC_Globals.h"

extern u8 bMenu_index;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BLINKING_TIME   5  // 5 * timebase_display_5 ms

#define VISUALIZATION_1   (u8)1
#define VISUALIZATION_2   (u8)2
#define VISUALIZATION_3   (u8)3
#define VISUALIZATION_4   (u8)4
#define VISUALIZATION_5   (u8)5
#define VISUALIZATION_6   (u8)6
#define VISUALIZATION_7   (u8)7
#define VISUALIZATION_8   (u8)8
#define VISUALIZATION_9   (u8)9
#define VISUALIZATION_10  (u8)10
#ifdef FLUX_WEAKENING
#define VISUALIZATION_11  (u8)11
#endif

#define CHAR_0            (u8)0 //First character of the line starting from the left
#define CHAR_1            (u8)1 
#define CHAR_2            (u8)2
#define CHAR_3            (u8)3
#define CHAR_4            (u8)4
#define CHAR_5            (u8)5
#define CHAR_6            (u8)6
#define CHAR_7            (u8)7
#define CHAR_8            (u8)8
#define CHAR_9            (u8)9
#define CHAR_10           (u8)10
#define CHAR_11           (u8)11
#define CHAR_12           (u8)12
#define CHAR_13           (u8)13
#define CHAR_14           (u8)14
#define CHAR_15           (u8)15
#define CHAR_16           (u8)16
#define CHAR_17           (u8)17

#ifdef OBSERVER_GAIN_TUNING 
#define CHAR_18           (u8)18
#endif

#ifdef DAC_FUNCTIONALITY
#define CHAR_19           (u8)19
#endif

