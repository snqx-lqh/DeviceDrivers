#ifndef __SPI_H
#define __SPI_H

#include "main.h"
#include "bsp_sys.h"

/*
时钟极性，CPOL=0，串行同步时钟的空闲状态为低电平
		  CPOL=1，串行同步时钟的空闲状态为高电平
时钟相位，CPHA=0，在串行同步时钟的第一个跳变沿（上升或下降）数据被采样
		  CPHA=1，在串行同步时钟的第二个跳变沿（上升或下降）数据被采样
*/

extern SPI_InitTypeDef  SPI1_InitStructure;
extern SPI_InitTypeDef  SPI2_InitStructure;

void SPI1_Init(void);			 //初始化SPI口
void SPI1_SetSpeed(uint8_t SpeedSet); //设置SPI速度
uint8_t SPI1_ReadWriteByte(uint8_t TxData);//SPI总线读写一个字节

void SPI2_Init(void);			 //初始化SPI口
void SPI2_SetSpeed(uint8_t SpeedSet); //设置SPI速度
uint8_t SPI2_ReadWriteByte(uint8_t TxData);//SPI总线读写一个字节

#endif

