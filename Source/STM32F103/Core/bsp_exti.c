#include "bsp_exti.h"

/*
void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0)!= RESET)
	{

		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}
*/

void bsp_exti_init()
{
    //打开时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    //引脚初始化
    {
        GPIO_InitTypeDef GPIO_InitStruct;

        GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;

        GPIO_Init(GPIOA,&GPIO_InitStruct);
    }
    //中断线初始化
    {
        EXTI_InitTypeDef EXTI_InitStructure;

        EXTI_InitStructure.EXTI_Line    = EXTI_Line0;
        EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
        EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;

        EXTI_Init(&EXTI_InitStructure);

        //初始化中断线
        GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
    }
    //NVIC初始化
    {
        NVIC_InitTypeDef NVIC_InitStructure;

        NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//使能外部中断通道
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;	//抢占优先级5，
        NVIC_InitStructure.NVIC_IRQChannelSubPriority =        0;	//子优先级0
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道

        NVIC_Init(&NVIC_InitStructure);
    }
}
