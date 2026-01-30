#ifndef _BSP_TIMER_GENERAL_H
#define _BSP_TIMER_GENERAL_H

#include "main.h"

void bsp_timer3_init(uint16_t arr,uint16_t psc);
void bsp_timer3_pwm_init(uint16_t arr,uint16_t psc);

void bsp_timer2_cap_init(u16 arr,u16 psc);

extern u8   TIM2CH1_CAPTURE_STA;		//输入捕获状态
extern u16	TIM2CH1_CAPTURE_VAL;	//输入捕获值



#endif
