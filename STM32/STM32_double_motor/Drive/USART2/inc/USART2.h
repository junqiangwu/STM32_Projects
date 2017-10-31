#ifndef _USART1_H
#define _USART1_H
#include "stm32f10x.h"

#define USART1_MAX_RECV_LEN		200					//最大接收缓存字节数
#define MAX_CMD_ARG_LENGTH 9
#define MAX_CMD_INFO_LENGTH 32
#define MAX_CMD_LINE_LENGTH 32
#define MAX_CMD_LINE_CNT 5
#define MAX_ARGC 3
#define STR_BUFFER_LEN 32


typedef struct {
    char cmd_name[MAX_CMD_ARG_LENGTH];   //命令的名字
    char cmd_usage[MAX_CMD_INFO_LENGTH];   //命令的信息
    void (*cmd_func)(int acgc,char *argv[]); //命令执行函数
}cmd_struct;


#define CMD_ADD(cmd_name,cmd_usage,cmd_func) \
    {\
        cmd_name,\
        cmd_usage,\
        cmd_func\
    }


extern u8  USART1_RX_BUF[USART1_MAX_RECV_LEN]; 		//接收缓冲,最大USART2_MAX_RECV_LEN字节
extern u16 USART1_RX_STA;   						//接收数据状态

extern void cmd_init(void);
int cmd_parse(char *cmd_line,int *argc,char *argv[]);  //命令行解析
int cmd_exec(int argc,char *argv[]);    
    
extern void USART1_Config(void);
extern void uprintf(USART_TypeDef* USARTx, char *fmt, ...);
#endif
