#ifndef _BSP_KEY_H
#define _BSP_KEY_H

#include "main.h"

#include "bsp_sys.h"

#define RCC_ALL_KEY 	(RCC_APB2Periph_GPIOA \
						|RCC_APB2Periph_GPIOC )

#define KEY_UP_GPIO_PORT GPIOA
#define KEY_UP_GPIO_PIN  GPIO_Pin_0

#define KEY_0_GPIO_PORT  GPIOC
#define KEY_0_GPIO_PIN   GPIO_Pin_5

#define KEY_1_GPIO_PORT  GPIOA
#define KEY_1_GPIO_PIN   GPIO_Pin_15

#define KEY_UP PAin(0)
#define KEY_0  PCin(5)
#define KEY_1  PAin(15)

#define KEY_UP_VALUE 1
#define KEY_0_VALUE  2
#define KEY_1_VALUE  3

void bsp_key_init(void);
uint8_t key_scan(uint8_t mode,uint8_t delay_count);

#endif

