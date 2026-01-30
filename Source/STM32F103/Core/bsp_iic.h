#ifndef _BSP_IIC_H
#define _BSP_IIC_H

#include "bsp_sys.h"

/*  IIC_SCL时钟端口、引脚定义 */
#define IIC_SCL_PORT 			GPIOB
#define IIC_SCL_PIN 			(GPIO_Pin_6)
#define IIC_SCL_PORT_RCC		RCC_APB2Periph_GPIOB

/*  IIC_SDA时钟端口、引脚定义 */
#define IIC_SDA_PORT 			GPIOB
#define IIC_SDA_PIN 			(GPIO_Pin_7)
#define IIC_SDA_PORT_RCC		RCC_APB2Periph_GPIOB

//IO方向设置
#define SDA_IN()  {GPIOC->CRL&=0X0FFFFFFF;GPIOC->CRH|=(uint32_t)8<<7;}	    //PB7输入模式
#define SDA_OUT() {GPIOC->CRH&=0X0FFFFFFF;GPIOC->CRH|=(uint32_t)3<<7;} 	//PB7输出模式

//IO操作
#define IIC_SCL   PBout(6) //SCL
#define IIC_SDA   PBout(7) //SDA
#define READ_SDA  PBin(7)  //输入SDA

#define IIC_SCL_0 IIC_SCL=0
#define IIC_SCL_1 IIC_SCL=1
#define IIC_SDA_0 IIC_SDA=0
#define IIC_SDA_1 IIC_SDA=1

//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);

#endif
