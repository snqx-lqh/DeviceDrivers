#include "main.h"
 
#include "bsp_delay.h"
#include "bsp_usart.h"
#include "bsp_sys.h" 
 
 
 
#include "control.h" 
 
#include "bsp_soft_i2c.h" 
 
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
	app_run_main();
	
	while(1)
	{ 
		printf("HELLO 1\r\n");
		delay_ms(1000);
	}
}

