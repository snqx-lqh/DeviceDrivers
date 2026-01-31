/**
  ******************************************************************************
  * @file    bh1750.c
  * @author  liqinghua <liqinghuaxx@163.com>
  * @version V1.0.0
  * @date    2026-01-29
  * @brief   BH1750 环境光传感器驱动程序
  *
  * @copyright (c) 2026 liqinghua
  * 
  * Permission is hereby granted, free of charge, to any person obtaining a copy
  * of this software and associated documentation files (the "Software"), to deal
  * in the Software without restriction, including without limitation the rights
  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  * copies of the Software, and to permit persons to whom the Software is
  * furnished to do so, subject to the following conditions:

  * The above copyright notice and this permission notice shall be included in all
  * copies or substantial portions of the Software.

  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  * SOFTWARE.
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
 * @brief  初始化 BH1750 光照传感器
 * @param  addr: 传感器 I2C 地址（7位地址，例如 0x23 或 0x5C）
 * @param  mode: 测量模式（连续或单次）
 *   @arg  BH1750_CONT_H_RES   - 连续高分辨率模式（1 lx，~120ms）
 *   @arg  BH1750_CONT_H_RES2  - 连续高分辨率模式2（0.5 lx，~120ms）
 *   @arg  BH1750_CONT_L_RES   - 连续低分辨率模式（4 lx，~16ms）
 *   @arg  BH1750_ONE_TIME_H   - 单次高分辨率模式（1 lx，~120ms）
 *   @arg  BH1750_ONE_TIME_H2  - 单次高分辨率模式2（0.5 lx，~120ms）
 *   @arg  BH1750_ONE_TIME_L   - 单次低分辨率模式（4 lx，~16ms）
 * @retval 0: 初始化成功；1: 初始化失败（I2C 通信错误）
 * @note   初始化步骤：
 *         1. Power On - 唤醒传感器进入工作状态
 *         2. Reset - 清零数据寄存器，清除之前的测量结果
 *         3. 设置测量模式 - 配置连续或单次测量
 *         4. 等待首次测量完成 - 确保数据准备就绪
 *         
 *         硬件要求：
 *         - VCC 和 GND 上电后，需等待至少 1us 复位时间
 *         - 本函数内置 180ms 延时以等待首次测量完成
 * 
 * @example
 *         bh1750_init(0x23, BH1750_CONT_H_RES);  // 使用 ADDR=GND，连续高分辨率
 */
uint8_t bh1750_init(uint8_t addr, uint8_t mode)
{
    uint8_t ret;
    
    // 1. Power On（要先让设备进入工作模式，不然后续处理无效）
    ret = bh1750_send_cmd(addr, BH1750_POWER_ON);
    if (ret) return ret;
    
    // 2. Reset（清零数据寄存器，用于清除之前的测量结果）
    ret = bh1750_send_cmd(addr, BH1750_RESET);
    if (ret) return ret;
    
    // 3. 设置测量模式
    ret = bh1750_send_cmd(addr, mode);
    if (ret) return ret;
    
    // 4. 等待第一次测量完成（最大180ms）
    // 如果是单次模式，不需要在这里等，而是在 read 前等
    if ((mode == BH1750_CONT_H_RES) || (mode == BH1750_CONT_H_RES2)) {
        delay_ms(180);  // 等待首次测量完成
    } else if (mode == BH1750_CONT_L_RES) {
        delay_ms(24);   // L分辨率最大24ms
    }
    
    return 0;
}

/**
 * @brief  读取照度值（连续测量模式）
 * @param  addr: 传感器 I2C 地址
 * @param  mode: 当前测量模式（用于判断分辨率，H/H2 需要除以 2）
 * @param  lux: 指向照度输出变量的指针（单位: lx）
 * @retval 0: 读取成功；-1: 读取失败（I2C 通信错误）
 * 
 * @note   转换公式（参考数据手册第 7、11 页）：
 *         lux = raw_data / 1.2
 *         其中 raw_data 是从传感器读取的 16 位原始数据
 *         
 *         特殊情况：
 *         - 模式为 H_RES2 或 ONE_TIME_H2 时，需要再除以 2
 *         - 该函数适用于连续测量模式，读出的是最新完成的测量结果
 * 
 * @warning 调用本函数前需确保：
 *         1. 已调用 bh1750_init() 完成初始化
 *         2. 对于连续模式，已等待至少一个测量周期
 * 
 * @example
 *         float illuminance;
 *         if (bh1750_read_lux(0x23, BH1750_CONT_H_RES, &illuminance) == 0) {
 *             printf("Illuminance: %.1f lx\r\n", illuminance);
 *         }
 */
int bh1750_read_lux(uint8_t addr, uint8_t mode, float *lux)
{
    uint8_t ret;
    uint8_t raw_data[2];
    uint16_t raw;
    
    ret = bh1750_read_raw(addr, raw_data);
    if (ret) return -1;
    
    raw = (raw_data[0] << 8) | raw_data[1];
    
    // 使用 float 防止整数溢出
    *lux = (float)raw / 1.2f;
	if(mode == BH1750_CONT_H_RES2 || mode == BH1750_ONE_TIME_H2){
		*lux = *lux/2;
	}
    
    return 0;
}

/**
 * @brief  单次模式读取照度值（低功耗推荐方案）
 * @param  addr: 传感器 I2C 地址
 * @param  mode: 测量模式
 *   @arg  BH1750_ONE_TIME_H   - 单次高分辨率模式
 *   @arg  BH1750_ONE_TIME_H2  - 单次高分辨率模式2
 *   @arg  BH1750_ONE_TIME_L   - 单次低分辨率模式
 * @param  lux: 指向照度输出变量的指针（单位: lx）
 * @retval 0: 读取成功；-1: 读取失败（I2C 通信错误）
 * 
 * @note   工作原理：
 *         1. 唤醒传感器（Power On）
 *         2. 发送单次测量命令
 *         3. 等待测量完成（最大 180ms）
 *         4. 读取数据后，传感器自动进入 Power Down 状态
 *         
 *         优点（相比连续模式）：
 *         - 功耗更低：测量完成后自动进入 Power Down（0.01μA）
 *         - 适合电池供电设备和间歇式测量
 *         - 避免持续功耗开销
 * 
 * @warning 注意事项：
 *         - 每次读取都会完整执行一个测量周期（最多 180ms）
 *         - 函数会阻塞等待测量完成，不适合高频率数据采集
 *         - 若需高频数据采集，应使用连续模式
 * 
 * @example
 *         float illuminance;
 *         if (bh1750_read_lux_single(0x23, BH1750_ONE_TIME_H, &illuminance) == 0) {
 *             printf("Illuminance: %.1f lx\r\n", illuminance);
 *             // 此时传感器已自动进入 Power Down，功耗极低
 *         }
 */
int bh1750_read_lux_single(uint8_t addr,uint8_t mode, float *lux)
{
    uint8_t ret;
    
    // 1. Power On
    ret = bh1750_send_cmd(addr, BH1750_POWER_ON);
    if (ret) return -1;
    
    // 2. 发送单次测量命令
    ret = bh1750_send_cmd(addr, mode);
    if (ret) return -1;
    
    // 3. 等待测量完成（最大180ms）
    delay_ms(180);
    
    // 4. 读取数据（传感器会自动进入 Power Down）
    return bh1750_read_lux(addr, mode, lux);
}

/**
 * @brief  设置测量时间寄存器（MTreg）以调节传感器灵敏度
 * @param  addr: 传感器 I2C 地址
 * @param  mt_val: MTreg 值（范围: 31~254，默认: 69）
 *                 < 69: 降低灵敏度，缩短测量时间（用于高亮环境）
 *                 = 69: 标准设置（默认工厂配置）
 *                 > 69: 提高灵敏度，延长测量时间（用于低亮环境或光学窗口遮挡）
 * @retval 0: 设置成功；1: 设置失败（I2C 通信错误）
 * 
 * @note   MTreg 原理（参考数据手册第 11 页）：
 *         - 用于调节光子积分时间，影响测量精度和响应速度
 *         - 一般用于补偿光学窗口的透过率变化
 *         - 改变 MTreg 值同时会改变光学灵敏度，需要相应调整照度计算系数
 *         
 *         命令格式：
 *         - 高字节（MTreg[7:5]）: 0x40 | (mt_val >> 5)
 *         - 低字节（MTreg[4:0]）: 0x60 | (mt_val & 0x1F)
 *         
 *         照度补偿公式：
 *         实际照度 = 原始照度 × (69 / MTreg)
 *         
 * @example
 *         // 提高灵敏度 1.5 倍（用于光线较弱的环境）
 *         bh1750_set_mtreg(0x23, 103);  // 69 * 1.5 ≈ 103
 *         
 *         // 降低灵敏度 1.5 倍（用于光线非常强的环境）
 *         bh1750_set_mtreg(0x23, 46);  // 69 / 1.5 ≈ 46
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
 * @brief  读取照度值（考虑 MTreg 补偿的连续测量模式）
 * @param  addr: 传感器 I2C 地址
 * @param  mode: 当前测量模式
 * @param  lux: 指向照度输出变量的指针（单位: lx）
 * @param  mtreg: 传感器当前设置的 MTreg 值（默认 69）
 *                如果之前调用过 bh1750_set_mtreg() 修改，应传入修改后的值
 * @retval 0: 读取成功；-1: 读取失败（I2C 通信错误）
 * 
 * @note   照度转换公式（参考数据手册第 7、11 页）：
 *         基础公式: lux = raw / 1.2
 *         补偿公式: lux = raw / 1.2 × (69 / mtreg)
 *         
 *         其中：
 *         - raw 是从传感器读取的 16 位原始数据
 *         - 69 是 BH1750 的默认 MTreg 值
 *         - mtreg 是传感器当前的 MTreg 设置值
 *         
 *         补偿原理：
 *         - 当 mtreg > 69 时，灵敏度增加，读数会偏高，需要乘以系数 < 1 进行补偿
 *         - 当 mtreg < 69 时，灵敏度降低，读数会偏低，需要乘以系数 > 1 进行补偿
 * 
 * @warning 使用前提：
 *         1. 已调用 bh1750_init() 完成初始化
 *         2. 已调用 bh1750_set_mtreg() 修改 MTreg（如果使用非默认值）
 *         3. 所传入的 mtreg 参数必须与实际设置相符
 * 
 * @example
 *         // 先将 MTreg 设为 100（提高灵敏度）
 *         bh1750_set_mtreg(0x23, 100);
 *         delay_ms(180);
 *         
 *         // 再读取照度，传入实际的 MTreg 值 100
 *         float illuminance;
 *         if (bh1750_read_lux_ex(0x23, BH1750_CONT_H_RES, &illuminance, 100) == 0) {
 *             printf("Illuminance: %.1f lx\r\n", illuminance);
 *         }
 */
int bh1750_read_lux_ex(uint8_t addr, uint8_t mode, float *lux, uint8_t mtreg)
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
    
	if(mode == BH1750_CONT_H_RES2 || mode == BH1750_ONE_TIME_H2){
		*lux = *lux/2;
	}
	
    return 0;
}

/**
 * @brief  单次模式读取照度值（考虑 MTreg 补偿的低功耗方案）
 * @param  addr: 传感器 I2C 地址
 * @param  mode: 测量模式（单次模式）
 * @param  lux: 指向照度输出变量的指针（单位: lx）
 * @param  mtreg: 传感器当前设置的 MTreg 值（默认 69）
 * @retval 0: 读取成功；-1: 读取失败（I2C 通信错误）
 * 
 * @note   功能说明：
 *         - 结合单次测量模式的低功耗优势
 *         - 结合 MTreg 补偿的精度优势
 *         - 最适合需要精确读数且要求低功耗的应用
 *         
 *         工作流程：
 *         1. 唤醒传感器（Power On）
 *         2. 发送单次测量命令
 *         3. 等待测量完成（最大 180ms）
 *         4. 读取数据并使用 MTreg 补偿公式计算照度
 *         5. 传感器自动进入 Power Down 状态
 *         
 * @warning 使用前提：
 *         1. 已调用 bh1750_init() 完成初始化
 *         2. 已调用 bh1750_set_mtreg() 修改 MTreg（如果使用非默认值）
 *         3. 所传入的 mtreg 参数必须与实际设置相符
 * 
 * @example
 *         // 先将 MTreg 设为 100（提高灵敏度）
 *         bh1750_set_mtreg(0x23, 100);
 *         
 *         // 单次测量读取，传入实际的 MTreg 值 100
 *         float illuminance;
 *         if (bh1750_read_lux_single_ex(0x23, BH1750_ONE_TIME_H, &illuminance, 100) == 0) {
 *             printf("Illuminance: %.1f lx\r\n", illuminance);
 *         }
 */
int bh1750_read_lux_single_ex(uint8_t addr, uint8_t mode, float *lux, uint8_t mtreg)
{
    uint8_t ret;
    
    // 1. Power On
    ret = bh1750_send_cmd(addr, BH1750_POWER_ON);
    if (ret) return -1;
    
    // 2. 发送单次测量命令
    ret = bh1750_send_cmd(addr, mode);
    if (ret) return -1;
    
    // 3. 等待测量完成（最大180ms）
    delay_ms(180);
    
    // Step 4: 读取数据（传感器会自动进入 Power Down）
    return bh1750_read_lux_ex(addr, mode, lux, mtreg);
}
 

 
