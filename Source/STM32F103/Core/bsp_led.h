#ifndef _BSP_LED_H
#define _BSP_LED_H

#include "main.h"

#include "bsp_sys.h"

void bsp_led_init(void);

#define RCC_ALL_LED 	(RCC_APB2Periph_GPIOA \
						|RCC_APB2Periph_GPIOD )

#define LED0_GPIO_PORT GPIOA
#define LED0_GPIO_PIN  GPIO_Pin_8

#define LED1_GPIO_PORT GPIOD
#define LED1_GPIO_PIN  GPIO_Pin_2

#define LED0 PAout(8)
#define LED1 PDout(2)

void bsp_led_on(uint8_t _no);
void bsp_led_off(uint8_t _no);
void bsp_led_toggle(uint8_t _no);

#endif

