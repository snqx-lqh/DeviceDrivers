#ifndef _BSP_TIMER_ADVANCE_H
#define _BSP_TIMER_ADVANCE_H

#include "main.h"

void bsp_timer1_init(uint16_t arr,uint16_t psc);
void bsp_timer1_pwm_init(uint16_t arr,uint16_t psc);
void bsp_timer1_spwm_init(uint16_t arr,uint16_t psc);

#endif
