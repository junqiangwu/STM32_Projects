#ifndef _USART1_H
#define _USART1_H
#include "stm32f10x.h"

#define USART1_MAX_RECV_LEN		200					//最大接收缓存字节数
extern u8  USART1_RX_BUF[USART1_MAX_RECV_LEN]; 		//接收缓冲,最大USART2_MAX_RECV_LEN字节
extern u16 USART1_RX_STA;   						//接收数据状态

extern void USART1_GPIOConfig(void);
extern void USART1_Config(void);
extern void uprintf(USART_TypeDef* USARTx, char *fmt, ...);
#endif
