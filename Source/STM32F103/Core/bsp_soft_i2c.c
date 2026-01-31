/**
  ******************************************************************************
  * @file    bsp_soft_i2c.c
  * @author  liqinghua <liqinghuaxx@163.com>
  * @version V1.0.0
  * @date    2026-01-29
  * @brief   软件I2C驱动模块
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
  *          本模块实现基于GPIO的软件I2C通信，支持多个I2C总线。
  *          支持的功能：
  *          - I2C初始化
  *          - I2C总线控制（起始、停止、应答等）
  *          - 单字节读写
  *          - 多字节读写
  *
  * @warning 引脚初始化时必须使用开漏输出模式（GPIO_Mode_Out_OD）
  */  
#include "bsp_soft_i2c.h"

/****************** user port area start ****************/
#include "bsp_delay.h"
#include "main.h"

/**
  * @brief  I2C延时函数（微秒级）
  * @note   延时时间用于控制I2C总线的时钟频率
  * @param  无
  * @retval 无
  */
void soft_i2c_delay_us()
{
	delay_us(4);
}

/**
  * @brief  I2C模块初始化
  * @note   注意：引脚初始化一定要是开漏模式（GPIO_Mode_Out_OD）
  *         SCL: PB6, SDA: PB7
  * @param  soft_i2c: I2C编号
  *   @arg  SOFT_I2C1: 软件I2C1
  *   @arg  SOFT_I2C2: 软件I2C2
  * @retval 无
  */
void soft_i2c_init(SOFT_I2C_TypeDef soft_i2c)
{
	if(soft_i2c == SOFT_I2C1){
		GPIO_InitTypeDef GPIO_InitStructure;
		//开启端口时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

		//配置SCL、SDA引脚
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; //开漏输出
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		GPIO_SetBits(GPIOB,GPIO_Pin_6);
		GPIO_SetBits(GPIOB,GPIO_Pin_7); //所有设备空闲，总线拉高电平
	}else if(soft_i2c == SOFT_I2C2){
	}
}

/**
  * @brief  设置SCL引脚电平
  * @param  soft_i2c: I2C编号
  *   @arg  SOFT_I2C1: 软件I2C1
  *   @arg  SOFT_I2C2: 软件I2C2
  * @param  level: 目标电平
  *   @arg  0: 低电平
  *   @arg  1: 高电平
  * @retval 无
  */
void soft_i2c_set_scl_level(SOFT_I2C_TypeDef soft_i2c,uint8_t level)
{
	if(soft_i2c == SOFT_I2C1){
		if(level == 0) GPIO_ResetBits(GPIOB,GPIO_Pin_6);
		else GPIO_SetBits(GPIOB,GPIO_Pin_6);
	}else if(soft_i2c == SOFT_I2C2){}
}

/**
  * @brief  设置SDA引脚电平
  * @param  soft_i2c: I2C编号
  *   @arg  SOFT_I2C1: 软件I2C1
  *   @arg  SOFT_I2C2: 软件I2C2
  * @param  level: 目标电平
  *   @arg  0: 低电平
  *   @arg  1: 高电平
  * @retval 无
  */
void soft_i2c_set_sda_level(SOFT_I2C_TypeDef soft_i2c,uint8_t level)
{
	if(soft_i2c == SOFT_I2C1){
		if(level == 0) GPIO_ResetBits(GPIOB,GPIO_Pin_7);
		else GPIO_SetBits(GPIOB,GPIO_Pin_7);
	}else if(soft_i2c == SOFT_I2C2){}
}

/**
  * @brief  读取SDA引脚电平
  * @param  soft_i2c: I2C编号
  *   @arg  SOFT_I2C1: 软件I2C1
  *   @arg  SOFT_I2C2: 软件I2C2
  * @retval SDA引脚电平值 (0: 低电平, 1: 高电平)
  */
uint8_t soft_i2c_get_sda_level(SOFT_I2C_TypeDef soft_i2c)
{
	uint8_t level;
	if(soft_i2c == SOFT_I2C1){
		level = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7);
	}else if(soft_i2c == SOFT_I2C2){}
	return level;
}
/****************** user port area end   ****************/

/**
  * @brief  I2C产生起始信号（START）
  * @details 起始条件：SCL高时，SDA从高变低
  * @param  soft_i2c: I2C编号
  *   @arg  SOFT_I2C1: 软件I2C1
  *   @arg  SOFT_I2C2: 软件I2C2
  * @retval 无
  */
void soft_i2c_start(SOFT_I2C_TypeDef soft_i2c)
{
	soft_i2c_set_sda_level(soft_i2c,1);
	soft_i2c_set_scl_level(soft_i2c,1);
	soft_i2c_delay_us();
	soft_i2c_set_sda_level(soft_i2c,0);
	soft_i2c_delay_us();
	soft_i2c_set_scl_level(soft_i2c,0);
}

/**
  * @brief  I2C产生停止信号（STOP）
  * @details 停止条件：SCL高时，SDA从低变高
  * @param  soft_i2c: I2C编号
  *   @arg  SOFT_I2C1: 软件I2C1
  *   @arg  SOFT_I2C2: 软件I2C2
  * @retval 无
  */
void soft_i2c_stop(SOFT_I2C_TypeDef soft_i2c)
{	
	soft_i2c_set_scl_level(soft_i2c,0);
	soft_i2c_set_sda_level(soft_i2c,0);
	soft_i2c_delay_us();
	soft_i2c_set_scl_level(soft_i2c,1);
	soft_i2c_set_sda_level(soft_i2c,1);
	soft_i2c_delay_us();
}

/**
  * @brief  等待I2C应答信号到来
  * @details 在第9个时钟周期时，从机拉低SDA表示应答
  * @param  soft_i2c: I2C编号
  *   @arg  SOFT_I2C1: 软件I2C1
  *   @arg  SOFT_I2C2: 软件I2C2
  * @retval 0: 接收应答成功
  * @retval 1: 接收应答失败（超时）
  */
uint8_t soft_i2c_wait_ack(SOFT_I2C_TypeDef soft_i2c)
{
    uint8_t ucErrTime=0;
	soft_i2c_set_sda_level(soft_i2c,1);	 
    soft_i2c_delay_us();
	soft_i2c_set_scl_level(soft_i2c,1);	 
    soft_i2c_delay_us();
    while(soft_i2c_get_sda_level(soft_i2c))
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            soft_i2c_stop(soft_i2c);
            return 1;
        }
    } 
	soft_i2c_set_scl_level(soft_i2c,0);	 
    return 0;
}

/**
  * @brief  主机产生I2C应答信号（ACK）
  * @details 拉低SDA，在SCL高期间保持
  * @param  soft_i2c: I2C编号
  *   @arg  SOFT_I2C1: 软件I2C1
  *   @arg  SOFT_I2C2: 软件I2C2
  * @retval 无
  */
void soft_i2c_ack(SOFT_I2C_TypeDef soft_i2c)
{	
	soft_i2c_set_sda_level(soft_i2c, 0);  // 拉低 SDA（ACK 信号）
    soft_i2c_delay_us();
    soft_i2c_set_scl_level(soft_i2c, 1);  // 拉高 SCL（从机读取 ACK）
    soft_i2c_delay_us();
    soft_i2c_set_scl_level(soft_i2c, 0);  // 拉低 SCL（结束第 9 个时钟周期）
    
    // 【关键】释放 SDA，设为高电平（或输入模式），让从机可以发送下一个字节
    soft_i2c_set_sda_level(soft_i2c, 1);  
    soft_i2c_delay_us();
}

/**
  * @brief  主机产生I2C非应答信号（NACK）
  * @details 保持SDA高电平，在SCL高期间释放
  * @param  soft_i2c: I2C编号
  *   @arg  SOFT_I2C1: 软件I2C1
  *   @arg  SOFT_I2C2: 软件I2C2
  * @retval 无
  */
void soft_i2c_nack(SOFT_I2C_TypeDef soft_i2c)
{
	soft_i2c_set_sda_level(soft_i2c, 1);  // 保持 SDA 高电平（NACK）
    soft_i2c_delay_us();
    soft_i2c_set_scl_level(soft_i2c, 1);  // 拉高 SCL
    soft_i2c_delay_us();
    soft_i2c_set_scl_level(soft_i2c, 0);  // 拉低 SCL
    // NACK 后通常跟着 STOP，所以这里可以不操作 SDA，由 stop 函数处理
}

/**
  * @brief  I2C发送一个字节数据
  * @details 高位先发（MSB）
  * @param  soft_i2c: I2C编号
  *   @arg  SOFT_I2C1: 软件I2C1
  *   @arg  SOFT_I2C2: 软件I2C2
  * @param  txd: 待发送字节数据
  * @retval 无
  */
void soft_i2c_send_byte(SOFT_I2C_TypeDef soft_i2c,uint8_t txd)
{
    uint8_t i;  
	soft_i2c_set_scl_level(soft_i2c,0);	
    for (i = 0; i < 8; i++)
    {
		if((txd&0x80)>>7)
			soft_i2c_set_sda_level(soft_i2c,1);			
		else
			soft_i2c_set_sda_level(soft_i2c,0);	
        txd<<=1;
		soft_i2c_set_scl_level(soft_i2c,1);
        soft_i2c_delay_us();  
		soft_i2c_set_scl_level(soft_i2c,0);
        soft_i2c_delay_us();  
    }
}

/**
  * @brief  I2C读取一个字节数据
  * @details 高位先读（MSB）
  * @param  soft_i2c: I2C编号
  *   @arg  SOFT_I2C1: 软件I2C1
  *   @arg  SOFT_I2C2: 软件I2C2
  * @param  ack: 读取后是否产生应答
  *   @arg  1: 产生应答（继续接收）
  *   @arg  0: 不产生应答（结束接收）
  * @retval 读取的字节数据
  */
uint8_t soft_i2c_read_byte(SOFT_I2C_TypeDef soft_i2c,uint8_t ack)
{
    uint8_t i,receive = 0;  

    for (i = 0; i < 8; i++)
    {
		soft_i2c_set_scl_level(soft_i2c,0);
        soft_i2c_delay_us();  
		soft_i2c_set_scl_level(soft_i2c,1);
        receive <<= 1; 
        if (soft_i2c_get_sda_level(soft_i2c))
        {
            receive++; 
        }
        soft_i2c_delay_us();  
    }
	
	soft_i2c_set_scl_level(soft_i2c, 0);
    soft_i2c_delay_us();
	
    if (!ack)
        soft_i2c_nack(soft_i2c); //非应答
    else
        soft_i2c_ack(soft_i2c); //应答

    return receive;
}

/**
  * @brief  从指定I2C设备的寄存器读取一个字节
  * @details I2C通信流程：START -> 发送从机地址(写) -> 发送寄存器地址 -> 
  *          重复START -> 发送从机地址(读) -> 读取数据 -> NACK -> STOP
  * @param  soft_i2c: I2C编号
  *   @arg  SOFT_I2C1: 软件I2C1
  *   @arg  SOFT_I2C2: 软件I2C2
  * @param  addr: I2C设备地址（7位，不包含读写位）
  * @param  reg: 设备寄存器地址
  * @param  data: 指向数据保存缓冲区的指针
  * @retval 0: 读取成功
  * @retval 1: 读取失败
  */
uint8_t soft_i2c_read_dev_one_byte(SOFT_I2C_TypeDef soft_i2c,uint8_t addr,uint8_t reg,uint8_t *data)
{
    soft_i2c_start(soft_i2c);
    soft_i2c_send_byte(soft_i2c,(addr<<1)|0);     
    if(soft_i2c_wait_ack(soft_i2c))               
    {
        soft_i2c_stop(soft_i2c);
        return 1;
    }
    soft_i2c_send_byte(soft_i2c,reg);             
    if(soft_i2c_wait_ack(soft_i2c))               
    {
        soft_i2c_stop(soft_i2c);
        return 1;
    }
    soft_i2c_start(soft_i2c);                     
    soft_i2c_send_byte(soft_i2c,(addr<<1)|1);     
    if(soft_i2c_wait_ack(soft_i2c))               
    {
        soft_i2c_stop(soft_i2c);
        return 1;
    }
    *data = soft_i2c_read_byte(soft_i2c,0);    
    soft_i2c_stop(soft_i2c);

    return 0;
}

/**
  * @brief  向指定I2C设备的寄存器写入一个字节
  * @details I2C通信流程：START -> 发送从机地址(写) -> 发送寄存器地址 -> 
  *          发送数据字节 -> STOP
  * @param  soft_i2c: I2C编号
  *   @arg  SOFT_I2C1: 软件I2C1
  *   @arg  SOFT_I2C2: 软件I2C2
  * @param  addr: I2C设备地址（7位，不包含读写位）
  * @param  reg: 设备寄存器地址
  * @param  data: 待写入的字节数据
  * @retval 0: 写入成功
  * @retval 1: 写入失败
  */
uint8_t soft_i2c_write_dev_one_byte(SOFT_I2C_TypeDef soft_i2c,uint8_t addr,uint8_t reg,uint8_t data)
{
    soft_i2c_start(soft_i2c);
    soft_i2c_send_byte(soft_i2c,(addr<<1)|0);      
    if(soft_i2c_wait_ack(soft_i2c))               
    {
        soft_i2c_stop(soft_i2c);
        return 1;
    }
    soft_i2c_send_byte(soft_i2c,reg);             
    if(soft_i2c_wait_ack(soft_i2c))                
    {
        soft_i2c_stop(soft_i2c);
        return 1;
    }
    soft_i2c_send_byte(soft_i2c,data);             
    if(soft_i2c_wait_ack(soft_i2c))                
    {
        soft_i2c_stop(soft_i2c);
        return 1;
    }
    soft_i2c_stop(soft_i2c);
    return 0;
}

/**
  * @brief  从指定I2C设备的寄存器读取多个字节
  * @details I2C通信流程：START -> 发送从机地址(写) -> 发送寄存器地址 -> 
  *          重复START -> 发送从机地址(读) -> 连续读取len个字节 -> 
  *          最后一个字节后NACK -> STOP
  * @param  soft_i2c: I2C编号
  *   @arg  SOFT_I2C1: 软件I2C1
  *   @arg  SOFT_I2C2: 软件I2C2
  * @param  addr: I2C设备地址（7位，不包含读写位）
  * @param  reg: 设备寄存器地址
  * @param  len: 读取字节数
  * @param  buf: 指向数据保存缓冲区的指针
  * @retval 0: 读取成功
  * @retval 1: 读取失败
  */
uint8_t soft_i2c_read_dev_len_byte(SOFT_I2C_TypeDef soft_i2c,uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{
    soft_i2c_start(soft_i2c);
    soft_i2c_send_byte(soft_i2c,(addr<<1)|0);  
    if(soft_i2c_wait_ack(soft_i2c))          
    {
        soft_i2c_stop(soft_i2c);
        return 1;
    }
    soft_i2c_send_byte(soft_i2c,reg); 
    if(soft_i2c_wait_ack(soft_i2c))                
    {
        soft_i2c_stop(soft_i2c);
        return 1;
    }
    soft_i2c_start(soft_i2c);
    soft_i2c_send_byte(soft_i2c,(addr<<1)|1); 
    if(soft_i2c_wait_ack(soft_i2c))                
    {
        soft_i2c_stop(soft_i2c);
        return 1;
    }
    while(len)
    {
        if(len==1)*buf=soft_i2c_read_byte(soft_i2c,0);
        else *buf=soft_i2c_read_byte(soft_i2c,1);		
        len--;
        buf++;
    }
    soft_i2c_stop(soft_i2c);                
    return 0;
}

/**
  * @brief  向指定I2C设备的寄存器写入多个字节
  * @details I2C通信流程：START -> 发送从机地址(写) -> 发送寄存器地址 -> 
  *          连续发送len个字节，每个字节后等待应答 -> STOP
  * @param  soft_i2c: I2C编号
  *   @arg  SOFT_I2C1: 软件I2C1
  *   @arg  SOFT_I2C2: 软件I2C2
  * @param  addr: I2C设备地址（7位，不包含读写位）
  * @param  reg: 设备寄存器地址
  * @param  len: 写入字节数
  * @param  buf: 指向待写入数据的缓冲区指针
  * @retval 0: 写入成功
  * @retval 1: 写入失败
  */
uint8_t soft_i2c_write_dev_len_byte(SOFT_I2C_TypeDef soft_i2c,uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{
    uint8_t i;
    soft_i2c_start(soft_i2c);
    soft_i2c_send_byte(soft_i2c,(addr<<1)|0);  
    if(soft_i2c_wait_ack(soft_i2c))         
    {
        soft_i2c_stop(soft_i2c);
        return 1;
    }
    soft_i2c_send_byte(soft_i2c,reg); 
    if(soft_i2c_wait_ack(soft_i2c))               
    {
        soft_i2c_stop(soft_i2c);
        return 1;
    }
    for(i=0; i<len; i++)
    {
        soft_i2c_send_byte(soft_i2c,buf[i]);  
        if(soft_i2c_wait_ack(soft_i2c))          
		{
			soft_i2c_stop(soft_i2c);
			return 1;
		}
    }
    soft_i2c_stop(soft_i2c);
    return 0;
}

/**
  * @brief  向I2C设备写入数据（简化接口）
  * @details I2C通信流程：START -> 发送从机地址(写) -> 连续发送len个字节 ->
  *          每个字节后等待应答 -> STOP
  *          相比 soft_i2c_write_dev_len_byte，此函数不需要指定寄存器地址
  * @param  soft_i2c: I2C编号
  *   @arg  SOFT_I2C1: 软件I2C1
  *   @arg  SOFT_I2C2: 软件I2C2
  * @param  addr: I2C设备地址（7位，不包含读写位）
  * @param  buf: 指向待写入数据的缓冲区指针
  * @param  len: 写入字节数
  * @retval 0: 写入成功
  * @retval 1: 写入失败
  */
uint8_t soft_i2c_write(SOFT_I2C_TypeDef soft_i2c,uint8_t addr,const uint8_t *buf,uint8_t len)
{
    uint8_t i;

    soft_i2c_start(soft_i2c);

    soft_i2c_send_byte(soft_i2c, (addr << 1) | 0);
    if (soft_i2c_wait_ack(soft_i2c))
        goto err;

    for (i = 0; i < len; i++)
    {
        soft_i2c_send_byte(soft_i2c, buf[i]);
        if (soft_i2c_wait_ack(soft_i2c))
            goto err;
    }

    soft_i2c_stop(soft_i2c);
    return 0;

err:
    soft_i2c_stop(soft_i2c);
    return 1;
}

/**
  * @brief  从I2C设备读取数据（简化接口）
  * @details I2C通信流程：START -> 发送从机地址(读) -> 连续读取len个字节 ->
  *          最后一个字节后NACK -> STOP
  *          相比 soft_i2c_read_dev_len_byte，此函数不需要指定寄存器地址
  * @param  soft_i2c: I2C编号
  *   @arg  SOFT_I2C1: 软件I2C1
  *   @arg  SOFT_I2C2: 软件I2C2
  * @param  addr: I2C设备地址（7位，不包含读写位）
  * @param  buf: 指向数据保存缓冲区的指针
  * @param  len: 读取字节数（必须大于0）
  * @retval 0: 读取成功
  * @retval 1: 读取失败或长度为0
  * @note   当len为0时返回失败
  */
uint8_t soft_i2c_read(SOFT_I2C_TypeDef soft_i2c,uint8_t addr,uint8_t *buf,uint8_t len)
{
    uint8_t i;

    if (len == 0)
        return 1;

    soft_i2c_start(soft_i2c);

    soft_i2c_send_byte(soft_i2c, (addr << 1) | 1);
    if (soft_i2c_wait_ack(soft_i2c))
        goto err;

    for (i = 0; i < len; i++)
    {
        if (i == (len - 1))
            buf[i] = soft_i2c_read_byte(soft_i2c, 0); // NACK
        else
            buf[i] = soft_i2c_read_byte(soft_i2c, 1); // ACK
    }

    soft_i2c_stop(soft_i2c);
    return 0;

err:
    soft_i2c_stop(soft_i2c);
    return 1;
}
