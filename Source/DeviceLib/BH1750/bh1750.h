#ifndef _BH_1750_H
#define _BH_1750_H

#include <stdint.h>

#include <stdint.h>

// I2C 地址（根据 ADDR 引脚电平）
#define BH1750_ADDR_L   0x23  // ADDR 接 GND 时的地址
#define BH1750_ADDR_H   0x5C  // ADDR 接 VCC 时的地址 

// 基本控制命令
#define BH1750_POWER_DOWN     0x00  // 关闭电源，进入低功耗模式（0.01uA）
#define BH1750_POWER_ON       0x01  // 打开电源
#define BH1750_RESET          0x07  // 仅复位数据寄存器，（清零测量结果，不影响 MTreg）

// 连续测量模式（测量完成后保持在该模式，不需要重复发送命令）
#define BH1750_CONT_H_RES     0x10  // 连续高分辨率（分辨率: 1 lx，测量时间: ~120ms 标准 MTreg 值 69 时）
#define BH1750_CONT_H_RES2    0x11  // 连续高分辨率模式2（分辨率: 0.5 lx，测量时间: ~120ms 标准 MTreg 值 69 时）
#define BH1750_CONT_L_RES     0x13  // 连续低分辨率（分辨率: 4 lx，测量时间: ~16ms，功耗更低）

// 单次测量模式（测量完成后自动进入 Power Down 状态，适合低功耗应用）
#define BH1750_ONE_TIME_H     0x20  // 单次高分辨率模式（分辨率: 1 lx，测量时间: ~120ms）
#define BH1750_ONE_TIME_H2    0x21  // 单次高分辨率模式2（分辨率: 0.5 lx，测量时间: ~120ms）
#define BH1750_ONE_TIME_L     0x23  // 单次低分辨率模式（分辨率: 4 lx，测量时间: ~16ms）

// MTreg 命令前缀（参考 BH1750 数据手册第 11 页）
// 用于调节测量时间（积分时间），范围: 31~254，默认: 69
// 调节时间会同步调节灵敏度
#define BH1750_MT_H           0x40  // MTreg 高 3 位命令前缀（01000_MT[7:5]）
#define BH1750_MT_L           0x60  // MTreg 低 5 位命令前缀（011_MT[4:0]）

/* API */
uint8_t bh1750_init(uint8_t addr, uint8_t mode);
int bh1750_read_lux(uint8_t addr, uint8_t mode, float *lux);
int bh1750_read_lux_single(uint8_t addr,uint8_t mode, float *lux);
uint8_t bh1750_set_mtreg(uint8_t addr, uint8_t mt_val);
int bh1750_read_lux_ex(uint8_t addr, uint8_t mode, float *lux, uint8_t mtreg);
int bh1750_read_lux_single_ex(uint8_t addr, uint8_t mode, float *lux, uint8_t mtreg);

#endif

