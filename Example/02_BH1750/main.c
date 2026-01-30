#include "main.h"
 
#include "bsp_delay.h"
#include "bsp_usart.h"
#include "bsp_sys.h" 
 
#include "bsp_soft_i2c.h"  

#include "bh1750.h"
 
float lux; 
 
int main()
{
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	//关闭JTAG调试
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO ,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	//常用外设初始化
	bsp_usart1_init(115200);
	delay_init();
	
	soft_i2c_init(SOFT_I2C1);
	
	bh1750_init(BH1750_WRITE_ADDR_L);
 
	while(1)
	{ 
		/* 高分辨率模式下，每次测量间隔至少 120ms */
        delay_ms(1000);  // 请替换为您的延时函数
        
        if (bh1750_read_lux(BH1750_WRITE_ADDR_L, &lux) == 0)
		{
			printf("Light = %.2f lux\r\n", lux);
		}
	}
}

