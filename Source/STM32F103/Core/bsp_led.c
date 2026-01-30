#include "bsp_led.h"

void bsp_led_init()
{
    //打开时钟
    RCC_APB2PeriphClockCmd(RCC_ALL_LED,ENABLE);
    //初始化IO口
    {
        GPIO_InitTypeDef GPIO_InitStruct;

        GPIO_InitStruct.GPIO_Pin  = LED0_GPIO_PIN;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStruct.GPIO_Speed= GPIO_Speed_50MHz;

        GPIO_Init(LED0_GPIO_PORT,&GPIO_InitStruct);

        GPIO_InitStruct.GPIO_Pin  = LED1_GPIO_PIN;
        GPIO_Init(LED1_GPIO_PORT,&GPIO_InitStruct);
    }
}

void bsp_led_on(uint8_t _no)
{
    if (_no == 0)
    {
        LED0_GPIO_PORT->BRR = LED0_GPIO_PIN;
    }
    else if (_no == 1)
    {
        LED1_GPIO_PORT->BRR = LED1_GPIO_PIN;
    }
}

void bsp_led_off(uint8_t _no)
{
    if (_no == 0)
    {
        LED0_GPIO_PORT->BSRR = LED0_GPIO_PIN;
    }
    else if (_no == 1)
    {
        LED1_GPIO_PORT->BSRR = LED1_GPIO_PIN;
    }
}

void bsp_led_toggle(uint8_t _no)
{
    if (_no == 0)
    {
        LED0_GPIO_PORT->ODR ^= LED0_GPIO_PIN;
    }
    else if (_no == 1)
    {
        LED1_GPIO_PORT->ODR ^= LED1_GPIO_PIN;
    }
}

