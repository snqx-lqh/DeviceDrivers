#ifndef _BH_1750_H
#define _BH_1750_H

#include <stdint.h>

#include <stdint.h>

// I2C 地址（7位地址左移后的值，根据 ADDR 引脚电平）
#define BH1750_ADDR_L   0x23  // ADDR 接 GND: 0100011 -> 左移后 0x46 & 0xFE = 0x46? 
                              // 注意：你的 soft_i2c 库如果要求 8位地址（含 R/W），则：
                              // 写地址 = 0x46 (0x23<<1), 读地址 = 0x47
                              // 如果使用 7位地址由库内部左移，则保持 0x23
                              // 这里假设你的库使用 8位地址格式（写地址）：
#define BH1750_WRITE_ADDR_L  0x23   
#define BH1750_WRITE_ADDR_H  0x5C  

// 命令定义（数据手册第5页）
#define BH1750_POWER_DOWN     0x00
#define BH1750_POWER_ON       0x01
#define BH1750_RESET          0x07  // 仅复位数据寄存器，不是阈值
#define BH1750_CONT_H_RES     0x10  // 连续 H 分辨率（1 lx）
#define BH1750_CONT_H_RES2    0x11  // 连续 H 分辨率模式2（0.5 lx）
#define BH1750_CONT_L_RES     0x13  // 连续 L 分辨率（4 lx，16ms）
#define BH1750_ONE_TIME_H     0x20  // 单次 H 分辨率（测量后自动断电）
#define BH1750_ONE_TIME_L     0x23  // 单次 L 分辨率

// MTreg 命令前缀（数据手册第11页）
#define BH1750_MT_H           0x40  // 01000_MT[7,6,5]
#define BH1750_MT_L           0x60  // 011_MT[4,3,2,1,0]

/* API */
void bh1750_init(uint8_t addr);
int  bh1750_read_lux(uint8_t addr, float *lux);

#endif

