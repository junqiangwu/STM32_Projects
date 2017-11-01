#include "string.h"
#include "stdio.h"
#include "USART2.h"
#include "PID.h"
#include "stdarg.h"
#include "misc.h"
#include "stm32f10x.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "USART_Function.h"

static char cmd_line[MAX_CMD_LINE_LENGTH + 1];
static char *cmd_argv[MAX_ARGC];


//串口接收缓存区 	
u8 USART1_RX_BUF[USART1_MAX_RECV_LEN]; 				//接收缓冲,最大USART2_MAX_RECV_LEN个字节.
u16 USART1_RX_STA=0;  


static cmd_struct cmd_tbl[] = {
    /*
     * 若需添加命令，需要在此加上：
     * CMD_ADD("命令名","命令使用方法（可为空格,但不能不加双引号）",对应命令的执行函数名)
     * 注意最后一个不需要逗号，前面的都需要逗号
     */
    CMD_ADD("p","Print all command and usage",Stop), 
    CMD_ADD("s","Print all command and usage",Start),
  	CMD_ADD("setp","set p of speed mode",set_speed_p),
    CMD_ADD("seti","set p of speed mode",set_speed_i),
    CMD_ADD("setd","set p of speed mode",set_speed_d), 
    CMD_ADD("test","set d of speed mode",set_test), 
    CMD_ADD("setp2","set p of speed mode",set_speed_p_2),
    CMD_ADD("seti2","set p of speed mode",set_speed_i_2),
    CMD_ADD("setd2","set p of speed mode",set_speed_d_2),
	//添加stop
	
};


void USART1_Config(void)
{        int i;    
    
    USART_InitTypeDef USART_InitStruct;
    GPIO_InitTypeDef	GPIO_InitStruct;
    USART_ClockInitTypeDef USART_ClockInitStruct;
    
       GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
     GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
     GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
     GPIO_Init(GPIOA, &GPIO_InitStruct);

     GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
     GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
     GPIO_Init(GPIOA,&GPIO_InitStruct); 
     
      
 	USART_InitStruct.USART_BaudRate            = 115200;	  					
	USART_InitStruct.USART_StopBits            = USART_StopBits_1;	 
	USART_InitStruct.USART_WordLength          = USART_WordLength_8b; 
	USART_InitStruct.USART_Parity              = USART_Parity_No;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

    USART_ClockInitStruct.USART_Clock=USART_Clock_Disable;
    USART_ClockInitStruct.USART_CPHA=USART_CPHA_2Edge;
    USART_ClockInitStruct.USART_CPOL=USART_CPOL_Low;
    USART_ClockInitStruct.USART_LastBit=USART_LastBit_Disable;
    USART_ClockInit(USART1,&USART_ClockInitStruct);

	USART_Init(USART1, &USART_InitStruct);
  	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);

    for(i = 0;i < MAX_ARGC;i++){
        cmd_argv[i] = (char *)malloc(MAX_CMD_ARG_LENGTH + 1);
    }
    

}


void USART1_IRQHandler(void){

    static u32 cmd_line_index = 0,cmd_line_length = 0;
    
    int cmd_argc,i;
    int erro_n;
    u8 c_recv;

    if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET){     
        
        USART_ClearITPendingBit(USART1,USART_IT_RXNE);
        
        c_recv = USART_ReceiveData(USART1);	// 缓存数据在c_recv
        
        if(c_recv == '\n'){  //接受完成  一次指令
			uprintf(USART1,"done");
            if(cmd_line_index == 0){   // 打印 Done
                return;
            }
            
            cmd_line[cmd_line_length++] = (char)c_recv;   // 保存 \n
            
            erro_n = cmd_parse(cmd_line,&cmd_argc,cmd_argv);  //解析命令  argc 存储参数的个数   set p 50  三个参数
                                                                    // argv 为每一个参数（字符串数组）首地址
            if(erro_n < 0){           //返回解析信息
                //打印函数执行错误信息
                if(erro_n == -3){
                cmd_line_index = 0;
                cmd_line_length = 0;
                memset(cmd_line,0,MAX_CMD_LINE_LENGTH);
                return;
                }else if(erro_n == -2){
                    uprintf(USART1,"\nthe param is too long");
                }else if(erro_n == -1){
                    uprintf(USART1,"\ntoo many param");
                }
                cmd_line_index = 0;
                cmd_line_length = 0;
                memset(cmd_line,0,MAX_CMD_LINE_LENGTH + 1);
                return;
            }        
            
            erro_n = cmd_exec(cmd_argc,cmd_argv);   //解析完成   执行命令
            
            if(erro_n < 0){
                //打印函数执行错误信息
                if(erro_n == -2){
                  	  uprintf(USART1,"\r\nnot find commmand:%s\n",cmd_argv[0]);
                }
                cmd_line_index = 0;
                cmd_line_length = 0;
                memset(cmd_line,0,MAX_CMD_LINE_LENGTH + 1);
                return;
            }
            cmd_line_index = 0;
            cmd_line_length = 0;
            memset(cmd_line,0,MAX_CMD_LINE_LENGTH + 1);
        }    
        
        else{ // 没有接受完数据
        
            if(cmd_line_index == MAX_CMD_LINE_LENGTH){  // 32
                //打印命令行太长的信息
                cmd_line_index = 0;
                cmd_line_length = 0;
                return;
            }
            
            for(i = 0;i < cmd_line_length - cmd_line_index;i++){
                cmd_line[cmd_line_length - i] = cmd_line[cmd_line_length - i -1];
            }
            
            cmd_line[cmd_line_index] = (char)c_recv;   // c_recv = USART_ReceiveData(USART1);	// 缓存数据在c_recv
			cmd_line_index++;
            cmd_line_length++;
        }			
    }
}


int cmd_parse(char *cmd_line,int *argc,char *argv[]){    //cmd_line 数组名 (地址)
    char c_temp;   
    int i = 0,arg_index = 0;
    int arg_cnt = 0;
    
    c_temp = cmd_line[i++];   // c_temp 接收到的数据
    
    while(c_temp != '\r'){    // 没有结束
        
        if(c_temp == ' '){      // 1 空格接受完命令  后面是参数
            
            if(arg_index == 0){     //如果命令或者参数字符串第一个是空格，则忽略  重新循环
                c_temp = cmd_line[i++];
                continue;         
            }
            
                     //空格为参数或者命令的分隔符
            if(arg_cnt == MAX_ARGC){   //如果参数个数过多,则返回  -1    参数长度  \=  参数个数
                return -1;
            }
            
            argv[arg_cnt][arg_index] = 0;
            arg_cnt++;//字数
            arg_index = 0;
            c_temp = cmd_line[i++];
            continue;
        }                            //  1
                
        if(arg_index == MAX_CMD_ARG_LENGTH){   //如果单个参数长度过长，则报错返回
            return -2;
        }
        
        argv[arg_cnt][arg_index++] = c_temp;   // 有一个空格传入  即argv[1][arg_index++] 
                                                   // arg_cnt为第几个参数   arg_index为某个参数的第几位
        c_temp = cmd_line[i++];
    }
    
      
    if(arg_cnt == 0 && arg_index == 0){  //如果命令或者参数是空的，则返回
        return -3;
    }
    
    //最后一个参数的结束没有在上面的while循环中解析到
    argv[arg_cnt++][arg_index] = 0;
    *argc = arg_cnt;   // argc 存储参数的个数   set p 50  三个参数
    return 0;
}



int cmd_exec(int argc,char *argv[]){
    int cmd_index = 0;
    u32 cmd_num;
 
    cmd_num = sizeof(cmd_tbl)/sizeof(cmd_tbl[0]);

    if(argc == 0){  //如果参数是空的，则返回
        return -1;
    }
    
    for(cmd_index = 0;cmd_index < cmd_num;cmd_index++){   //查找命令
        if(strcmp((char *)(cmd_tbl[cmd_index].cmd_name),(char *)argv[0]) == 0){  //如果找到了命令，则执行命令相对应的函数
            uprintf(USART1,"\n");
            cmd_tbl[cmd_index].cmd_func(argc,argv);
            return 0;
        }
    }
    return -2;
}

void uprintf(USART_TypeDef* USARTx, char *fmt, ...)
 {
	char buffer[128];
	u8 i = 0;
     
	va_list arg_ptr;
	va_start(arg_ptr, fmt);  
	vsnprintf(buffer, 128, fmt, arg_ptr);
     
	USART_ClearFlag(USARTx,USART_FLAG_TXE);
	while ((i < 128) && buffer[i])
	{
		if(buffer[i] == '\n'){
			
			USART_SendData(USARTx,(u8)'\r');
			while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET); 
			USART_SendData(USARTx,(u8)buffer[i++]);
		}else{
	    USART_SendData(USARTx, (u8) buffer[i++]);
		}
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET); 
	}
	va_end(arg_ptr);
 }
