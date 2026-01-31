#include "main.h"
 
#include "bsp_delay.h"
#include "bsp_usart.h"
#include "bsp_sys.h" 
 
#include "bsp_soft_i2c.h"  

#include "bh1750.h"
 
float lux; 
 
void exp_continuous_measure()
{
	bh1750_init(BH1750_ADDR_L,BH1750_CONT_H_RES);
	bh1750_set_mtreg(BH1750_ADDR_L,69);
	while(1)
	{
		if (bh1750_read_lux(BH1750_ADDR_L, BH1750_CONT_H_RES, &lux) == 0)
		{
			printf("continuous Light = %.2f lux\r\n", lux);
		}
		/* 高分辨率模式下，每次测量间隔至少 120ms */
        delay_ms(1000);  // 请替换为您的延时函数
	}
}	

void exp_continuous_measure_ex()
{
	bh1750_init(BH1750_ADDR_L,BH1750_CONT_H_RES);
	bh1750_set_mtreg(BH1750_ADDR_L,69*2);
	while(1)
	{
		if (bh1750_read_lux_ex(BH1750_ADDR_L,BH1750_CONT_H_RES, &lux, 69*2) == 0)
		{
			printf("continuous ex Light = %.2f lux\r\n", lux);
		}
		/* 高分辨率模式下，每次测量间隔至少 120ms */
        delay_ms(1000);  // 请替换为您的延时函数
	}
}	

void exp_single_measure()
{
	bh1750_set_mtreg(BH1750_ADDR_L,69);
	while(1)
	{ 
		if (bh1750_read_lux_single(BH1750_ADDR_L, BH1750_ONE_TIME_H, &lux) == 0)
		{
			printf("single Light = %.2f lux\r\n", lux);
		}
		/* 高分辨率模式下，每次测量间隔至少 120ms */
        delay_ms(1000);  // 请替换为您的延时函数
	}
}	
 
void exp_single_measure_ex()
{
	bh1750_set_mtreg(BH1750_ADDR_L,69*2);
	while(1)
	{ 
		if (bh1750_read_lux_single_ex(BH1750_ADDR_L,BH1750_ONE_TIME_H, &lux, 69*2) == 0)
		{
			printf("single ex Light = %.2f lux\r\n", lux);
		}
		/* 高分辨率模式下，每次测量间隔至少 120ms */
        delay_ms(1000);  // 请替换为您的延时函数
	}
}	

int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
 
	//常用外设初始化
	bsp_usart1_init(115200);
	delay_init();
	printf("\r\nBH1750 Measure Example\r\n");
	soft_i2c_init(SOFT_I2C1);
		
	exp_continuous_measure(); //连续测量
	//exp_single_measure(); // 单次测量
	//exp_continuous_measure_ex(); // 修改灵敏度
	//exp_single_measure_ex();
	while(1)
	{ 

	}
}

