/**
  ************************************* Copyright ****************************** 
  *
  *                 (C) Copyright 2024,LQH,China, GCU.
  *                            All Rights Reserved
  *                              
  *                                  By LQH
  *                     
  *    
  * FileName   : bsp_timer_advance.c   		
  * Author     : 少年潜行(snqx-lgh)			
  * Date       : 2024-01-04         
  * Description: CSDN主页： https://blog.csdn.net/wan1234512?type=blog
			     Gitee地址：https://gitee.com/snqx-lqh   
				 该文件将包含高级定时器的定时配置，PWM输出配置，互补死区PWM配置，输入捕获配置

 * 下面注释摘自野火说明
 * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
 * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
 * 另外三个成员是通用定时器和高级定时器才有.
 *-----------------------------------------------------------------------------
 *typedef struct
 *{ TIM_Prescaler            都有
 *	TIM_CounterMode			 TIMx,x[6,7]没有，其他都有
 *  TIM_Period               都有
 *  TIM_ClockDivision        TIMx,x[6,7]没有，其他都有
 *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]才有
 *}TIM_TimeBaseInitTypeDef; 
 *-----------------------------------------------------------------------------
  ******************************************************************************
 */


#include "bsp_timer_advance.h"


/*
void TIM1_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
	{

		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );   //清除TIMx更新中断标志
	}
}
*/

void bsp_timer1_init(uint16_t arr,uint16_t psc)
{
	//时钟初始化
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //时钟使能
    //定时器初始化
    {
        TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

        TIM_TimeBaseStructure.TIM_Period    =   arr-1;                      //设置自动重装载值
        TIM_TimeBaseStructure.TIM_Prescaler =   psc-1;                    //设置预分频值
        TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //设置时钟分割:TDTS = Tck_tim
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式

        TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
    }
    //NVIC初始化
    {
        NVIC_InitTypeDef NVIC_InitStructure;

        NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;              //TIM1中断
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;    //先占优先级0级
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;           //从优先级3级
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              //IRQ通道被使能

        NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
    }
    //定时器使能以及开启中断
    {
        TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE ); //使能指定的TIM1中断,允许更新中断
        TIM_Cmd(TIM1, ENABLE);                    //使能TIMx
    }
}

void bsp_timer1_pwm_init(uint16_t arr,uint16_t psc)
{
	//时钟初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,  ENABLE);   //
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //使能GPIO外设时钟使能
	//GPIO Init
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		//设置该引脚为复用输出功能,
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	//定时器基础配置
	{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		TIM_TimeBaseStructure.TIM_Period =     arr-1;  
		TIM_TimeBaseStructure.TIM_Prescaler =  psc-1; 
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割
		TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;  //TIM向上计数模式
		TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); 
	}
    //定时器输出比较功能配置
	{
		TIM_OCInitTypeDef  TIM_OCInitStructure;
		
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
		TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
		
		TIM_OC1Init(TIM1, &TIM_OCInitStructure);           //CH1初始化
		TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1预装载使能
	}
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	TIM_ARRPreloadConfig(TIM1, ENABLE); //使能TIMx在ARR上的预装载寄存器
    TIM_Cmd(TIM1, ENABLE);  //使能TIM1
}

void bsp_timer1_spwm_init(uint16_t arr,uint16_t psc)
{
	//时钟初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,  ENABLE);   //
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //使能GPIO外设时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   //使能GPIO外设时钟使能
	//GPIO Init
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		//设置该引脚为复用输出功能,
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; 
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		//刹车通道
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; 
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		// BKIN引脚默认先输出低电平
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);	
		
	}
	//定时器基础配置
	{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		TIM_TimeBaseStructure.TIM_Period =     arr-1;  
		TIM_TimeBaseStructure.TIM_Prescaler =  psc-1; 
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割
		TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;  //TIM向上计数模式
		TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); 
	}
    //定时器输出比较功能配置
	{
		TIM_OCInitTypeDef  TIM_OCInitStructure;
		
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
		TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; // 互补输出使能
		TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
		TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;// 互补输出通道电平极性配置
		TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;// 输出通道空闲电平极性配置
		TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;// 互补输出通道空闲电平极性配置
		
		TIM_OC1Init(TIM1, &TIM_OCInitStructure);           //CH1初始化
		TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1预装载使能
	}
	{
		// 有关刹车和死区结构体的成员具体可参考BDTR寄存器的描述
		TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
		TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
		TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
		TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
		// 输出比较信号死区时间配置，具体如何计算可参考 BDTR:UTG[7:0]的描述
		// 这里配置的死区时间为152ns
		TIM_BDTRInitStructure.TIM_DeadTime = 11;
		TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
		// 当BKIN引脚检测到高电平的时候，输出比较信号被禁止，就好像是刹车一样
		TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
		TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
		TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
		
	}
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	TIM_ARRPreloadConfig(TIM1, ENABLE); //使能TIMx在ARR上的预装载寄存器
    TIM_Cmd(TIM1, ENABLE);  //使能TIM1
}


