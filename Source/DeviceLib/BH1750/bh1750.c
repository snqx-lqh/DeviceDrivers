/**
  ******************************************************************************
  * @file    bh1750.c
  * @author  
  * @version V1.0.0
  * @date    2026-01-29
  * @brief   BH1750 环境光传感器驱动程序
  *
  * @details
  *          本模块实现BH1750环境光强度传感器的驱动函数。
  *          BH1750是一个I2C接口的16位环境光传感器，能够测量环境光强度。
  *          
  *          主要功能：
  *          - 传感器初始化
  *          - 发送命令到传感器
  *          - 读取原始ADC数据
  *          - 将原始数据转换为照度值（单位：lux）
  *
  *          支持工作模式：
  *          - 连续H精度模式（分辨率1 lux/LSB）
  *          - 典型工作电流：250μA
  *          - 待机电流：1μA
  *
  * @warning 使用前需要确保I2C总线正确初始化
  *
  ******************************************************************************
  */
#include "bh1750.h"

/********************User modification area begin********************/
//只需要将自己的IIC处理程序替换进去即可
#include "main.h"
#include "bsp_soft_i2c.h"
#include "bsp_delay.h"

/* 静态函数：发送命令 */
static uint8_t bh1750_send_cmd(uint8_t addr, uint8_t cmd)
{
    // 注意：确保 addr 是写地址（7位地址左移1位，最低位为0）
	// 我的 写函数中自动处理了 7位地址左移 所以这里直接把 7位地址传进来就行了。
    return soft_i2c_write(SOFT_I2C1, addr, &cmd, 1);
}

/* 静态函数：读取原始数据 */
static uint8_t bh1750_read_raw(uint8_t addr, uint8_t raw_data[2])
{
    return soft_i2c_read(SOFT_I2C1, addr, raw_data, 2);
}
/********************User modification area end ********************/

/**
 * @brief  修改测量时间（灵敏度调整）
 * @param  addr: 传感器地址
 * @param  mt_val: MTreg值（31-254，默认69）
 *                 >69 提高灵敏度（测量时间延长）
 *                 <69 降低灵敏度（测量时间缩短）
 * @retval 0:成功，1:失败
 * @note   数据手册第11页：用于补偿光学窗口透过率
 */
uint8_t bh1750_set_mtreg(uint8_t addr, uint8_t mt_val)
{
    uint8_t ret;
    uint8_t high = BH1750_MT_H | ((mt_val >> 5) & 0x07);  // MT[7:5]
    uint8_t low  = BH1750_MT_L | (mt_val & 0x1F);         // MT[4:0]
    
    ret = bh1750_send_cmd(addr, high);
    if (ret) return ret;
    ret = bh1750_send_cmd(addr, low);
    return ret;
}

/**
 * @brief  初始化 
 * @param  addr: 传感器地址（7位地址左移后的写地址，如 0x46）
 * @param  mode: 测量模式
 *   @arg  BH1750_CONT_H_RES:  连续高分辨率（120ms，1lx）
 *   @arg  BH1750_CONT_L_RES:  连续低分辨率（16ms，4lx）
 *   @arg  BH1750_ONE_TIME_H:  单次高分辨率（测量后自动断电）
 * @retval 0:成功，1:失败
 * @note   数据手册要求：VCC和DVI上电后，需等待至少1us复位时间
 *         本函数内置180ms延时等待首次测量完成
 */
uint8_t bh1750_init_ex(uint8_t addr, uint8_t mode)
{
    uint8_t ret;
    
    // Step 1: Power On（必须先发 Power On，否则 RESET 无效）
    ret = bh1750_send_cmd(addr, BH1750_POWER_ON);
    if (ret) return ret;
    
    // Step 2: Reset（清零数据寄存器，数据手册第5页）
    ret = bh1750_send_cmd(addr, BH1750_RESET);
    if (ret) return ret;
    
    // Step 3: 设置测量模式
    ret = bh1750_send_cmd(addr, mode);
    if (ret) return ret;
    
    // Step 4: 关键！等待第一次测量完成（最大180ms，数据手册第7页）
    // 如果是单次模式，不需要在这里等，而是在 read 前等
    if ((mode == BH1750_CONT_H_RES) || (mode == BH1750_CONT_H_RES2)) {
        delay_ms(180);  // 等待首次测量完成
    } else if (mode == BH1750_CONT_L_RES) {
        delay_ms(24);   // L分辨率最大24ms
    }
    
    return 0;
}

/**
 * @brief  读取照度（支持 MTreg 补偿计算）
 * @param  addr: 传感器地址
 * @param  lux: 输出照度值指针
 * @param  mtreg: 当前使用的 MTreg 值（默认69）
 *                如果未修改过，传入 69
 * @retval 0:成功，-1:失败
 * @note   转换公式：lux = raw / 1.2 * (69 / mtreg)
 *         数据手册第7页、第11页
 */
int bh1750_read_lux_ex(uint8_t addr, float *lux, uint8_t mtreg)
{
    uint8_t ret;
    uint8_t raw_data[2];
    uint16_t raw;
    
    ret = bh1750_read_raw(addr, raw_data);
    if (ret) return -1;
    
    raw = (raw_data[0] << 8) | raw_data[1];
    
    // 根据 MTreg 计算实际照度（数据手册第11页公式）
    // 默认 mtreg=69 时，系数为 1/1.2 ≈ 0.833
    // 使用 float 防止整数溢出
    *lux = (float)raw / 1.2f * (69.0f / (float)mtreg);
    
    return 0;
}

/**
 * @brief  单次模式测量（低功耗推荐）
 * @param  addr: 传感器地址
 * @param  lux: 输出照度值指针
 * @param  mtreg: MTreg值（默认69）
 * @retval 0:成功，-1:失败
 * @note   测量完成后传感器自动进入 Power Down（0.01uA）
 *         适合电池供电场景
 */
int bh1750_read_lux_single(uint8_t addr, float *lux, uint8_t mtreg)
{
    uint8_t ret;
    
    // Step 1: Power On
    ret = bh1750_send_cmd(addr, BH1750_POWER_ON);
    if (ret) return -1;
    
    // Step 2: 发送单次测量命令
    ret = bh1750_send_cmd(addr, BH1750_ONE_TIME_H);
    if (ret) return -1;
    
    // Step 3: 等待测量完成（最大180ms）
    delay_ms(180);
    
    // Step 4: 读取数据（传感器会自动进入 Power Down）
    return bh1750_read_lux_ex(addr, lux, mtreg);
}

void bh1750_init(uint8_t addr)
{
    bh1750_init_ex(addr, BH1750_CONT_H_RES);
}

int bh1750_read_lux(uint8_t addr, float *lux)
{
    return bh1750_read_lux_ex(addr, lux, 69);  // 默认 MTreg=69
}
