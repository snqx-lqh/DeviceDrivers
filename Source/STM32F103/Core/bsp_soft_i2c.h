#ifndef _BSP_SOFT_I2C_H
#define _BSP_SOFT_I2C_H

#include <stdint.h>

typedef enum {
	SOFT_I2C1 = 0,
	SOFT_I2C2 ,
}SOFT_I2C_TypeDef;

void soft_i2c_init(SOFT_I2C_TypeDef soft_i2c);
void soft_i2c_start(SOFT_I2C_TypeDef soft_i2c);
void soft_i2c_stop(SOFT_I2C_TypeDef soft_i2c);
uint8_t soft_i2c_wait_ack(SOFT_I2C_TypeDef soft_i2c);
void soft_i2c_ack(SOFT_I2C_TypeDef soft_i2c);
void soft_i2c_nack(SOFT_I2C_TypeDef soft_i2c);
void soft_i2c_send_byte(SOFT_I2C_TypeDef soft_i2c,uint8_t txd);
uint8_t soft_i2c_read_byte(SOFT_I2C_TypeDef soft_i2c,uint8_t ack);

uint8_t soft_i2c_write_dev_one_byte(SOFT_I2C_TypeDef soft_i2c,uint8_t addr,uint8_t reg,uint8_t data);
uint8_t soft_i2c_read_dev_one_byte(SOFT_I2C_TypeDef soft_i2c,uint8_t addr,uint8_t reg,uint8_t *data);
uint8_t soft_i2c_write_dev_len_byte(SOFT_I2C_TypeDef soft_i2c,uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf);
uint8_t soft_i2c_read_dev_len_byte(SOFT_I2C_TypeDef soft_i2c,uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf);

uint8_t soft_i2c_write(SOFT_I2C_TypeDef soft_i2c,uint8_t addr,const uint8_t *buf,uint8_t len);
uint8_t soft_i2c_read(SOFT_I2C_TypeDef soft_i2c,uint8_t addr,uint8_t *buf,uint8_t len);

#endif

