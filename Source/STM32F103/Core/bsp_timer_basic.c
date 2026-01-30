/**
  ************************************* Copyright ****************************** 
  *
  *                 (C) Copyright 2024,LQH,China, GCU.
  *                            All Rights Reserved
  *                              
  *                                  By LQH
  *                     
  *    
  * FileName   : bsp_timer_basic.c   		
  * Author     : 少年潜行(snqx-lgh)			
  * Date       : 2024-01-04         
  * Description: CSDN主页： https://blog.csdn.net/wan1234512?type=blog
			     Gitee地址：https://gitee.com/snqx-lqh
				 这个是基本定时器的使用，在STM32F103中，只有TIM6和TIM7是基本定时器,
				 本文件初始化定时器6作为示例
 
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


#include "bsp_timer_basic.h"

