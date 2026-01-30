#include "bsp_key.h"

#include "bsp_delay.h"

//10MS延时函数
#define KEY_DELAY_10MS() delay_ms(10)

void bsp_key_init()
{
    //打开时钟
    RCC_APB2PeriphClockCmd(RCC_ALL_KEY,ENABLE);

    //初始化IO口
    {
        GPIO_InitTypeDef GPIO_InitStruct;

        GPIO_InitStruct.GPIO_Pin  = KEY_UP_GPIO_PIN;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;

        GPIO_Init(KEY_UP_GPIO_PORT,&GPIO_InitStruct);

        GPIO_InitStruct.GPIO_Pin  = KEY_0_GPIO_PIN;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_Init(KEY_0_GPIO_PORT,&GPIO_InitStruct);

        GPIO_InitStruct.GPIO_Pin  = KEY_1_GPIO_PIN;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_Init(KEY_1_GPIO_PORT,&GPIO_InitStruct);
    }
}

/**
  * @brief          返回按键按下的值
  * @param[out]     mode:模式0就是单次扫描，1就是重复检测 delay_count:根据周期时间确定延时的时间,有值就是定时器延时，为0就是普通延时
  * @retval         uint8_t 按键值
  */
uint8_t key_scan(uint8_t mode,uint8_t delay_count)
{
    static uint8_t keyCount = 0;
    static uint8_t keyState = 0;
    if(mode == 1)  keyState = 0;
    if ((KEY_UP == 1 || KEY_0 == 0 || KEY_1 == 0)  && keyState == 0) //按键按下
    {
        if(delay_count == 0)
        {
            KEY_DELAY_10MS();
            keyState = 1;
            if(KEY_UP == 1) return KEY_UP_VALUE;
            else if(KEY_0 == 0) return KEY_0_VALUE;
            else if(KEY_1 == 0) return KEY_1_VALUE;
        } else
        {
            keyCount++;
            if (keyCount > delay_count )
            {
                keyState = 1;
                keyCount = 0;
                if(KEY_UP == 1) return KEY_UP_VALUE;
                else if(KEY_0 == 0) return KEY_0_VALUE;
                else if(KEY_1 == 0) return KEY_1_VALUE;
            }
        }
    }
    else if (KEY_UP == 0 && KEY_0 == 1 && KEY_1 == 1 && keyState == 1) //当所有按键都处于抬起状态，状态刷新
    {
        keyCount = 0;
        keyState = 0;
    }
    return 0;
}

