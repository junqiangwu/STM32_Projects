

#include "TYPESTRUCT.h"
//#include "stm32f10x.h"

#ifndef RAM
#define RAM

#ifdef root								//仅在main.c中定义，其他.c中不要#define之
#define ramExtern 	 					//在main.c中加上"#define root", 则 ramExtern 被定义为空，
#else				
#define ramExtern extern			//在其他的.c文件中没有"#define root",也包含本文件时， ramExtern 被定义为"extern”，避免多重定义的问题。
#endif			

#define TBUF_SIZE   4096           /*** Must be one of these powers of 2 (2,4,8,16,32,64,128) ***/  //????
#define RBUF_SIZE   4096           /*** Must be one of these powers of 2 (2,4,8,16,32,64,128) ***/
#define N     6
#define ADN		60

ramExtern  NumToArrayBufDef N2ABuf;
ramExtern  MotorParam_TypeDef MotorA;

ramExtern U8	uct_i;//临时变量
ramExtern U8	uct_j;
ramExtern U8	uct_k;
ramExtern U32	uit_i;
ramExtern U8	ucKeyPressTime;
ramExtern U8	ucKeyDat;
ramExtern U8	ucPreKey;
ramExtern U8	ucKeyDebouce;
ramExtern U8	ucKeyNew;
ramExtern U8	ucKeyOld;
ramExtern U8	ucT10Ms;
ramExtern U8	ucT4Ms;
ramExtern U8	ucIICDly;
ramExtern U8	ucT8Ms;
ramExtern U16	ucT1S;
ramExtern U8	ucT10S;
ramExtern U16   usOZTimeS;
ramExtern U16  usPWMSpd;

ramExtern U8	ucT100ms;

ramExtern U8	ucLcdBuffer[8][128];//LCD 显示Buf
ramExtern S16	usSetSpd;
ramExtern U16	usSetSpdBk;
ramExtern U16	 BEMF_Cnt;
ramExtern U16	 BEMF_ADC_Cnt;

ramExtern U16	 ADC_Value[ADN][3];

ramExtern U16	usCurSpd;

ramExtern U8  ReceivedFA;		
//ramExtern U8  MyAddr=0x01; 	                  //下位机地址
//ramExtern U8  ReceiverAddr=0x01;				  //上位机地址
//ramExtern U8  SenderAdr=0x01;					  //发送的地址
ramExtern U8  CS485;
ramExtern U8  MsgCount_485;
ramExtern U8  CS;
ramExtern U8  NoteByte;
ramExtern U8  PureByteCount;
ramExtern U8  ReceiverDatBody[10];				//接收数据暂存数组
ramExtern U16  Pluse;
ramExtern S32 My_PWM;
ramExtern U16 Hall;
ramExtern U16 aim_speed;

ramExtern U32  Period;

ramExtern U32 motor_statue;
ramExtern S16	speed_1;

ramExtern U32	check_run;
ramExtern U8	ucMExt;
ramExtern U8	ucMotorStep;
ramExtern U16  VoltBEMF[4];
ramExtern U8	ucMotorAD;
ramExtern U16	ucSendTime;
ramExtern U8	ucST;
ramExtern U8	num;
ramExtern U8   ucstep[8];
ramExtern S32   siPwmAdd;
//温度控制PID 参数
ramExtern S32  siLast_e;//
ramExtern S32  siThis_e;//
ramExtern S32  siLast_delta_e;//
ramExtern S32  siThis_delta_e;//
ramExtern S32  siDoub_delta_e;//
ramExtern S32  siPwidth;//
ramExtern U8 	ucPID_P;//
ramExtern U8 	ucPID_I;//
ramExtern U8 	ucPID_D;//


//串口接收数据定义
ramExtern volatile U8 UART1_ucTBuf [TBUF_SIZE];//发送buf 
ramExtern volatile U8 UART1_ucRBuf [RBUF_SIZE];//接收BUF	
ramExtern volatile U16 UART1_ucTIn  ;
ramExtern volatile U16 UART1_ucTOut ;	
ramExtern volatile U16 UART1_ucRIn  ;
ramExtern volatile U16 UART1_ucROut ;
ramExtern volatile U8 UART1_ucTRestart ;  //发送标志位
ramExtern volatile U16 ADCConvertedValue_2[N];

//Key Commamd define
//#define  AssistKey							
#define  SetKey								1//--PA7//确认键
#define  FunKey								2//--PC4
#define  LeftKey 							4//--PC5							0x10//--k6
#define  RightKey							8//--PB0
#define  UpKey								0x10//--PB1
#define  DownKey							0x20//--PB10
#define  OtherKey							0xff
#define  NoKey								0

#define  KeyPressLong						188			// 按键长按计时8ms*255=2s
#define  KeyContinue						42			// 连续按键计时8ms*42=333ms
#define  KeyDebouce2						4			// 按键去抖计时8*4=64ms

#define     NOP()           				__nop();__nop(); __nop(); __nop(); __nop(); __nop(); __nop();__nop();__nop();
#define     Delay1Us()      				NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();

#define LCDDispOn  							0x3f
#define LCDBaseAddrX  						0xb8
#define LCDBaseAddrY  						0x40
#define LCDBaseAddrZ 						0xc0
#define LCDR								1
#define LCDL								0

#define Hall_Mode 0
#define Hall_Mode_High 0
#define  BLDC_TIMER_NUM         TIM1

#define kp 6
#define ki	12
#define kd  0
#define ClockDir 0//顺时针时候为1






/**** 此处定义FLAG可位选址 ****/
ramExtern TYPE_BYTE  tmpbyte;//byte to bit 共同体

ramExtern TYPE_BYTE		tmpdata;
ramExtern TYPE_BYTE		FLAG0;
ramExtern TYPE_BYTE		FLAG1;
ramExtern TYPE_BYTE		FLAG2;
ramExtern TYPE_BYTE		FLAG3;
ramExtern TYPE_BYTE		FLAG4;
ramExtern TYPE_BYTE		FLAG5;
ramExtern TYPE_BYTE		FLAG6;
ramExtern TYPE_BYTE		FLAG7;
ramExtern TYPE_BYTE		FLAG8;
ramExtern TYPE_BYTE		FLAG9;	
ramExtern TYPE_BYTE		FLAGA;
ramExtern TYPE_BYTE		FLAGB;
ramExtern TYPE_BYTE		FLAGC;
ramExtern TYPE_BYTE		FLAGD;
ramExtern TYPE_BYTE		FLAGE;
ramExtern TYPE_BYTE		FLAGF;
ramExtern TYPE_BYTE		FLAGG;
ramExtern TYPE_BYTE		FLAGH;
ramExtern TYPE_BYTE		FLAGI;
ramExtern TYPE_BYTE		FLAGJ;
ramExtern TYPE_BYTE		FLAGK;
ramExtern TYPE_BYTE		FLAGL;
ramExtern TYPE_BYTE		FLAGM;
ramExtern TYPE_BYTE		FLAGN;
ramExtern TYPE_BYTE		FLAGO;
ramExtern TYPE_BYTE		FLAGP;
ramExtern TYPE_BYTE		FLAGQ;
ramExtern TYPE_BYTE		FLAGR;
ramExtern TYPE_BYTE		FLAGS;
ramExtern TYPE_BYTE		FLAGT;
ramExtern TYPE_BYTE		FLAGU;
ramExtern TYPE_BYTE		FLAGV;
ramExtern TYPE_BYTE		FLAGW;
ramExtern TYPE_BYTE		FLAGX;
ramExtern TYPE_BYTE		FLAGY;
ramExtern TYPE_BYTE		FLAGZ;

ramExtern TYPE_DWORD	FLAGERRA1;
ramExtern TYPE_DWORD	FLAGERRA2;


#define		fgDispMode	       	FLAG0.bit.b0     //反冲洗使能
#define		fgKeyDeal       	FLAG0.bit.b1	  	 //按键处理标志
#define		fgBusy				FLAG0.bit.b2	   	 //按键正在处理标志
#define		fgRun	    		FLAG0.bit.b3	   	// 初始化E 方错误提示
#define		fgSendA     		FLAG0.bit.b4	   	//串口发送标志
#define		fgSendABK      		FLAG0.bit.b5   		//低电压标志
#define		fgSendB         FLAG0.bit.b6	   //恢复有错误职位
#define		fgSendC        	FLAG0.bit.b7	   //水质使能标志位

/*******************FLAG1*******************/   
/*显示标志位改变显示模块时候更改*/ 
//#define  	Led1		   	FLAG1.byte	    //
#define		fgsendNum	  		FLAG1.bit.b0     //led4 开关变量
#define		fgSendABKaa  			FLAG1.bit.b1	    // 主机错误标志
#define		fgSendD  		FLAG1.bit.b2	    //pump 联网标志
#define		fgSendE 			FLAG1.bit.b3	    //补水联网标志
#define		fgSendF   		FLAG1.bit.b4  	 //正在恢复标志位
#define		fg2S    	FLAG1.bit.b5	    //仅有一个放冲洗沙缸使能时候标志
#define		fgAdConv  			FLAG1.bit.b6	    //数据刷新用
//#define		fgLedAlm2     		FLAG1.bit.b7	    //LED8 使能标志位




      
/*******************FLAG2*******************/
/*显示标志位改变显示模块时候更改*/
//...............6a 6b 6c 6d 6e 6f 6g / 
//#define  	Led2   			FLAG2.byte	    //
//#define		fgLedReqBaw      	FLAG2.bit.b0  //num2     
//#define		fgLedAlm      	FLAG2.bit.b1	//num1      
//#define		fgLedBackWash   FLAG2.bit.b2	//num4	        	
//#define		number_2D      FLAG2.bit.b3	       	
//#define		number_2E      FLAG2.bit.b4       	
//#define		number_2F      FLAG2.bit.b5	     
//#define		number_2G      FLAG2.bit.b6	     
/*******************FLAG3*******************/	
/*显示标志位改变显示模块时候更改*/
//...............5a 5b 5c 5d 5e 5f 5g 5h
//#define  	Display_get5   FLAG3.byte	    
#define		fgLcdCS1			FLAG3.bit.b0    //LCD 片选
#define	    fgSwGndON       	FLAG3.bit.b1    //补水水满状态位
#define		fgT2S 	   			FLAG3.bit.b2    //温湿度2秒标志位
#define	    fgRTC1S 			FLAG3.bit.b3    //RTC 一秒获取实时时间
#define		fgRTCSet	   		FLAG3.bit.b4    //设定RTC时间
#define		fgMenuRef      		FLAG3.bit.b5	//秒刷新标志位
#define		fgIICDly        	FLAG3.bit.b6	//IIC 周期性延时标志位
#define		fgIICRW         	FLAG3.bit.b7    //读和写标志位

//#define		fgrade5dff	   FLAG3.bit.b0 	   //
/*******************FLAG4*******************/  

#define		fgBranch			FLAG4.bit.b0	    //程序轮换执行标志位
#define		fgT10Ms     		FLAG4.bit.b1	    //周期10ms标志位
#define		fgSWRHErrS    		FLAG4.bit.b2	    //
#define		fgLedDebug      	FLAG4.bit.b3	    //程序运行时自动闪烁，在中断处理
#define		fgFilterinit    	FLAG4.bit.b4	    //开机时候反冲洗标志位
#define		fgStartRun    		FLAG4.bit.b5   	    //运行标志位
#define		fgIICErrS    		FLAG4.bit.b6	    //IIC错误标志保存标志位
#define		fgCanErrS	   		FLAG4.bit.b7	    //can 错误保存标志位


#ifdef root//def    root	

U8 const Eng_A[5]={ 0x7E,0x11,0x11,0x11,0x7E};
U8 const Eng_B[5]={ 0x7F,0x49,0x49,0x49,0x3E};
U8 const Eng_C[5]={ 0x3E,0x41,0x41,0x41,0x22};
U8 const Eng_D[5]={ 0x7F,0x41,0x41,0x22,0x1C};
U8 const Eng_E[5]={ 0x7F,0x49,0x49,0x49,0x41};
U8 const Eng_F[5]={ 0x7F,0x09,0x09,0x09,0x01};
U8 const Eng_G[5]={ 0x3E,0x41,0x49,0x49,0x7A};
U8 const Eng_H[5]={ 0x7F,0x08,0x08,0x08,0x7F};
U8 const Eng_I[5]={ 0x00,0x41,0x7F,0x41,0x00};
U8 const Eng_J[5]={ 0x21,0x41,0x3F,0x01,0x01};
U8 const Eng_K[5]={ 0x7F,0x08,0x14,0x22,0x41};
U8 const Eng_L[5]={ 0x7F,0x40,0x40,0x40,0x40};
U8 const Eng_M[5]={ 0x7F,0x02,0x0C,0x02,0x7F};
U8 const Eng_N[5]={ 0x7F,0x04,0x08,0x10,0x7F};
U8 const Eng_O[5]={ 0x3E,0x41,0x41,0x41,0x3E};
U8 const Eng_P[5]={ 0x7F,0x09,0x09,0x09,0x06};
U8 const Eng_Q[5]={ 0x3E,0x41,0x51,0x21,0x5E};
U8 const Eng_R[5]={ 0x7F,0x09,0x19,0x29,0x46};
U8 const Eng_S[5]={ 0x46,0x49,0x49,0x49,0x31};
U8 const Eng_T[5]={ 0x01,0x01,0x7F,0x01,0x01};
U8 const Eng_U[5]={ 0x3F,0x40,0x40,0x40,0x3F};
U8 const Eng_V[5]={ 0x1F,0x20,0x40,0x20,0x1F};
U8 const Eng_W[5]={ 0x3F,0x40,0x38,0x40,0x3F};
U8 const Eng_X[5]={ 0x63,0x14,0x08,0x14,0x63};
U8 const Eng_Y[5]={ 0x07,0x08,0x70,0x08,0x07};
U8 const Eng_Z[5]={ 0x61,0x51,0x49,0x45,0x43};


U8 const Eng_a[5]={ 0x20,0x54,0x54,0x54,0x78};
U8 const Eng_b[5]={ 0x7F,0x48,0x44,0x44,0x38};
U8 const Eng_c[5]={ 0x38,0x44,0x44,0x44,0x20};
U8 const Eng_d[5]={ 0x38,0x44,0x44,0x48,0x7F};
U8 const Eng_e[5]={ 0x38,0x54,0x54,0x54,0x18};
U8 const Eng_f[5]={ 0x08,0x7E,0x09,0x01,0x02};
U8 const Eng_g[5]={ 0x08,0x54,0x54,0x54,0x7C};//{ 0x06,0x49,0x49,0x49,0x3F};
U8 const Eng_h[5]={ 0x7F,0x08,0x04,0x04,0x78};
U8 const Eng_i[5]={ 0x00,0x44,0x7D,0x40,0x00};
U8 const Eng_j[5]={ 0x20,0x40,0x44,0x7D,0x00};
U8 const Eng_k[5]={ 0x7F,0x10,0x28,0x44,0x00};
U8 const Eng_l[5]={ 0x00,0x41,0x7F,0x40,0x00};
U8 const Eng_m[5]={ 0x7C,0x04,0x18,0x04,0x78};
U8 const Eng_n[5]={ 0x7C,0x08,0x04,0x04,0x78};
U8 const Eng_o[5]={ 0x38,0x44,0x44,0x44,0x38};
U8 const Eng_p[5]={ 0x7C,0x14,0x14,0x14,0x08};
U8 const Eng_q[5]={ 0x08,0x14,0x14,0x18,0x7C};
U8 const Eng_r[5]={ 0x7C,0x08,0x04,0x04,0x08};
U8 const Eng_s[5]={ 0x48,0x54,0x54,0x54,0x20};
U8 const Eng_t[5]={ 0x04,0x3F,0x44,0x40,0x20};
U8 const Eng_u[5]={ 0x3C,0x40,0x40,0x20,0x7C};
U8 const Eng_v[5]={ 0x1C,0x20,0x40,0x20,0x1C};
U8 const Eng_w[5]={ 0x3C,0x40,0x38,0x40,0x3C};
U8 const Eng_x[5]={ 0x44,0x28,0x10,0x28,0x44};
U8 const Eng_y[5]={ 0x0C,0x50,0x50,0x50,0x3C};
U8 const Eng_z[5]={ 0x44,0x64,0x54,0x4C,0x44};


U8 const sign_duC[5]={ 0x01,0x3C,0x42,0x42,0x24};
U8 const sign_duF[5]={ 0x01,0x7E,0x0A,0x0A,0x02};
U8 const sign_sanjiao[4]={ 0x7F,0x3E,0x1C,0x08};

U8 const sign_heng[5]={ 0x08,0x08,0x08,0x08,0x08};//sign_heng[3]={ 0x08,0x08,0x08};
U8 const sign_duanheng[3]={ 0x08,0x08,0x08};
//U8 const sign_diheng[5]={ 0x10,0x10,0x10,0x10,0x10};
U8 const sign_xiegang[3]={ 0x60,0x18,0x06};  //显示的个数跟数组一样也是为3
U8 const sign_zuokuo[3]={ 0x1C,0x22,0x41};
U8 const sign_youkuo[3]={ 0x41,0x22,0x1C};
U8 const sign_maohao[3]={ 0x00,0x6C,0x6C};//{ 0x00,0x36,0x36};
U8 const sign_dot[2]={ 0x60,0x60};
U8 const sign_bigdot16x3[6]={0x00,0x00,0x00,0x0E,0x0E,0x0E};
U8 const sign_zheng[5]={ 0x08,0x08,0x3E,0x08,0x08};
U8 const sign_fu[5]={ 0x08,0x08,0x08,0x08,0x08};
U8 const sign_jiankuo[5]={ 0x00,0x41,0x22,0x14,0x08};
U8 const sign_bolang[5]={ 0x08,0x04,0x08,0x10,0x08};
U8 const sign_xinghao[5]={ 0x2A,0x1C,0x3E,0x1C,0x2A};
U8 const sign_shenglve[5]={ 0x08,0x00,0x08,0x00,0x08};
U8 const sign_baifenhao[5]={ 0x43,0x33,0x08,0x66,0x61};
U8 const sign_zonghe[5]={ 0x81,0xC3,0xA5,0x99,0xC3};
U8 const sign_jinhao[5]={0x14,0x7F,0x14,0x7F,0x14};
U8 const  sign_jiahao[5]={0x08,0x08,0x7F,0x08,0x08};
U8 const sign_gantanhao[5]={0x00,0x00,0x5F,0x00,0x00};
U8 const  sign_shangpie[2]={0x0B,0x07};
U8 const SysADPsw[4]={1,9,8,3};


U8 const sign_tanhao[4]={0x00,0x5E,0x5E,0x00};
U8 const sign_and[5]={0x36,0x49,0x55,0x22,0x50};


U8 const number8x5[10][5]={ 
{ 0x3E,0x51,0x49,0x45,0x3E},
{ 0x00,0x42,0x7F,0x40,0x00},
{ 0x42,0x61,0x51,0x49,0x46},
{ 0x21,0x41,0x45,0x4B,0x31},
{ 0x18,0x14,0x12,0x7F,0x10},
{ 0x27,0x45,0x45,0x45,0x39},
{ 0x3C,0x4A,0x49,0x49,0x30},
{ 0x01,0x71,0x09,0x05,0x03},
{ 0x36,0x49,0x49,0x49,0x36},
{ 0x06,0x49,0x49,0x29,0x1E}
};

U8 const blank4[4]={ 0x00,0x00,0x00,0x00};
U8 const blank6[6]={ 0x00,0x00,0x00,0x00,0x00,0x00};
U8 const black6[6]={ 0xff,0xff,0xff,0xff,0xff,0xff};

U8 const blank8[16]={ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
#else

ramExtern unsigned char const Eng_A[5];
ramExtern unsigned char const Eng_B[5];
ramExtern unsigned char const Eng_C[5];
ramExtern unsigned char const Eng_D[5];
ramExtern unsigned char const Eng_E[5];
ramExtern unsigned char const Eng_F[5];
ramExtern unsigned char const Eng_G[5];
ramExtern unsigned char const Eng_H[5];
ramExtern unsigned char const Eng_I[5];
ramExtern unsigned char const Eng_J[5];
ramExtern unsigned char const Eng_K[5];
ramExtern unsigned char const Eng_L[5];
ramExtern unsigned char const Eng_M[5];
ramExtern unsigned char const Eng_N[5];
ramExtern unsigned char const Eng_O[5];
ramExtern unsigned char const Eng_P[5];
ramExtern unsigned char const Eng_Q[5];
ramExtern unsigned char const Eng_R[5];
ramExtern unsigned char const Eng_S[5];
ramExtern unsigned char const Eng_T[5];
ramExtern unsigned char const Eng_U[5];
ramExtern unsigned char const Eng_V[5];
ramExtern unsigned char const Eng_W[5];
ramExtern unsigned char const Eng_X[5];
ramExtern unsigned char const Eng_Y[5];
ramExtern unsigned char const Eng_Z[5];


ramExtern unsigned char const Eng_a[5];
ramExtern unsigned char const Eng_b[5];
ramExtern unsigned char const Eng_c[5];
ramExtern unsigned char const Eng_d[5];
ramExtern unsigned char const Eng_e[5];
ramExtern unsigned char const Eng_f[5];
ramExtern unsigned char const Eng_g[5];
ramExtern unsigned char const Eng_h[5];
ramExtern unsigned char const Eng_i[5];
ramExtern unsigned char const Eng_j[5];
ramExtern unsigned char const Eng_k[5];
ramExtern unsigned char const Eng_l[5];
ramExtern unsigned char const Eng_m[5];
ramExtern unsigned char const Eng_n[5];
ramExtern unsigned char const Eng_o[5];
ramExtern unsigned char const Eng_p[5];
ramExtern unsigned char const Eng_q[5];
ramExtern unsigned char const Eng_r[5];
ramExtern unsigned char const Eng_s[5];
ramExtern unsigned char const Eng_t[5];
ramExtern unsigned char const Eng_u[5];
ramExtern unsigned char const Eng_v[5];
ramExtern unsigned char const Eng_w[5];
ramExtern unsigned char const Eng_x[5];
ramExtern unsigned char const Eng_y[5];
ramExtern unsigned char const Eng_z[5];


ramExtern unsigned char const sign_duC[5];
ramExtern unsigned char const sign_duF[5];
ramExtern unsigned char const sign_sanjiao[4];
ramExtern unsigned char const sign_heng[5];//sign_heng[3];	
ramExtern unsigned char const sign_duanheng[3];
ramExtern unsigned char const sign_diheng[5];
ramExtern unsigned char const sign_xiegang[3];
ramExtern unsigned char const sign_zuokuo[3];
ramExtern unsigned char const sign_youkuo[3];
ramExtern unsigned char const sign_maohao[3];
ramExtern unsigned char const sign_dot[2];
ramExtern unsigned char const sign_bigdot16x3[6];
ramExtern unsigned char const sign_zheng[5];
ramExtern unsigned char const sign_fu[5];
ramExtern unsigned char const sign_jiankuo[5];
ramExtern unsigned char const sign_bolang[5];
ramExtern unsigned char const sign_xinghao[5];
ramExtern unsigned char const sign_shenglve[5];
ramExtern unsigned char const sign_baifenhao[5];
ramExtern unsigned char const sign_tanhao[4];
ramExtern unsigned char const sign_jinhao[5];
ramExtern unsigned char const sign_gantanhao[5];
ramExtern unsigned char const sign_and[5];
ramExtern unsigned char const sign_jiahao[5];
ramExtern unsigned char const sign_shangpie[2];
ramExtern unsigned char const SysADPsw[4];




ramExtern unsigned char const sign_zonghe[5];

ramExtern unsigned char const number8x5[10][5];

ramExtern unsigned char const blank4[4];
ramExtern unsigned char const blank6[6];
ramExtern unsigned char const black6[6];

#endif 

#endif


