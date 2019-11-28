#include "mytime.h"
#include "sys.h"
	

void Time_Init(u16 yfp,u16 tz)  //yfp 预分频     tz 填充值
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitType;
    NVIC_InitTypeDef NVIC_InitType;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
    
    TIM_TimeBaseInitType.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInitType.TIM_CounterMode=TIM_CounterMode_Up;
    TIM_TimeBaseInitType.TIM_Period=tz;
    TIM_TimeBaseInitType.TIM_Prescaler=yfp;
     
    TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitType);
    
    TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM4, //TIM2
		TIM_IT_Update ,
		ENABLE  //使能
                );
	NVIC_InitType.NVIC_IRQChannel = TIM4_IRQn;  //TIM3中断
	NVIC_InitType.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitType.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitType.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitType);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM4, ENABLE);  //使能TIMx外设

}


