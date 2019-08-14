#ifndef I2C_H
#define	I2C_H

#include<xc.h>
#include <stdint.h>

#define SLAVE_ADDRESS_LCD 0x4E // change this according to ur setup

void I2C_init(uint32_t clock);
void I2C_wait(void);
void I2C_start(void);
void I2C_repeated_start(void);
void I2C_stop(void);
void I2C_write(uint8_t data);

void lcd_send_data(unsigned char data);
void lcd_send_cmd(unsigned char data);
void lcd_init(void);
void lcd_send_string(char *str);

#endif
