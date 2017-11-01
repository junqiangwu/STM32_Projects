#include "ADC.h"
#include "stm32f10x.h"
#include "USART2.h"
#include "main.h"

vu16 AD_Buf[4]; //用来存放ADC转换结果，也是DMA的目标地址
extern vu16 I_Thrut[4];  // 电压值

#define ADC1_DR_Address     ((u32)0x4001244C)

//循环采样   

void Get_Adc_Average(void)
{ 
    u8 count; 
     ADC_SoftwareStartConvCmd(ADC1, ENABLE);
     while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)); //等待转换结束	
     for ( count=0;count<4;count++)
        {
            I_Thrut[count]=(u16)(AD_Buf[count]* 330 / 4096);
        } 
}

void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

   
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOC|RCC_APB2Periph_ADC1, ENABLE );	  //使能ADC1通道时钟
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PA0/1/2/3  模拟输入口                        
     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		
	 GPIO_Init(GPIOA, &GPIO_InitStructure);	
    
                           
     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		
	 GPIO_Init(GPIOC, &GPIO_InitStructure);	
    
	 ADC_DeInit(ADC1);  //复位ADC1

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC工作模式:ADC1和ADC2工作在独立模式
    ADC_InitStructure.ADC_ScanConvMode =ENABLE; //模数转换工作在扫描模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //模数转换工作在连续转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; 
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel =4; //顺序进行规则转换的ADC通道的数目
    ADC_Init(ADC1, &ADC_InitStructure); //根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器  
    
        
 //设置指定ADC的规则组通道，设置它们的转化顺序和采样时间
    //ADC1,ADC通道x,规则采样顺序值为y,采样时间为239.5周期
  
   ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_239Cycles5 );
   ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 2, ADC_SampleTime_239Cycles5 );
   ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 3, ADC_SampleTime_239Cycles5 );
   ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 4, ADC_SampleTime_239Cycles5 );   //V_BUS
    
 // 开启ADC的DMA支持（要实现DMA功能，还需独立配置DMA通道等参数）
    ADC_DMACmd(ADC1, ENABLE);   
    
//     ADC_InjectedSequencerLengthConfig(ADC1, 1);
//     ADC_InjectedChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_7Cycles5);//CURRENT  
//     ADC_InjectedChannelConfig(ADC1,ADC_Channel_3,2,ADC_SampleTime_7Cycles5);//CURRENT  
//     ADC_InjectedChannelConfig(ADC1,ADC_Channel_4,1,ADC_SampleTime_7Cycles5);//CURRENT  
//     ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_CC4);//这个可以使用time1的CCR4事件来触发采样
//     ADC_ExternalTrigInjectedConvCmd(ADC1,ENABLE);
//     ADC_SoftwareStartInjectedConvCmd(ADC1, ENABLE);
//     
//     ADC_ITConfig(ADC1,ADC_IT_JEOC,ENABLE);//ENABLE INJECTED INTERRUPT

	ADC_Cmd(ADC1, ENABLE);	
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
}	




 void DMA_Configuration(void)
{      
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_DeInit(DMA1_Channel1); //将DMA的通道1寄存器重设为缺省值
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address; //DMA外设ADC基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_Buf; //DMA内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //内存作为数据传输的目的地
    DMA_InitStructure.DMA_BufferSize = 4; //DMA通道的DMA缓存的大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设地址寄存器不变
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //内存地址寄存器递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //数据宽度为16位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //数据宽度为16位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //工作在循环缓存模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA通道 x拥有高优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //DMA通道x没有设置为内存到内存传输
    DMA_Init(DMA1_Channel1, &DMA_InitStructure); //根据DMA_InitStruct中指定的参数初始化DMA的通道
    DMA_Cmd(DMA1_Channel1, ENABLE); //启动DMA通道
}

extern u8 ERROR_flag;
 void ADC1_2_IRQHandler(void)
 {

 }






