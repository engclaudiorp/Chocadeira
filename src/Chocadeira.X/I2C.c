#include<xc.h>
#include "I2C.h"
#include "defines.h"

void I2C_init(uint32_t clock) {
    SSPADD = (_XTAL_FREQ/(4*clock))-1;  // here clock is the BR/clock speed    
    SSPCON = 0b00101000;     //first 4 bits I2c master mode , 6th bit is SSPEN enables the scl and sda line
    SSPSTAT = 0;
    TRISCbits.TRISC3 = 1;
    TRISCbits.TRISC4 = 1;
}
 
void I2C_wait(void) {
    while ((SSPSTAT & 0x04) || (SSPCON2 & 0x1F));    // wait for start bit to clear in SSPSTAT and bits 0 to 4 in SSPCON2
}
 
void I2C_start(void) {
    I2C_wait();
    SSPCON2 |= 0x01; // SEN=1 -> initiate the START condition on SDA and SCL pins
}
 
 
void I2C_repeated_start(void) {
    I2C_wait();
    SSPCON2 |= 0x02; // RSEN=1  -> initiate REPEATED START condition on SDA and SCL pins
}
 
 
void I2C_stop(void) {
    I2C_wait();
    SSPCON2 |= 0x04; // PEN=1 -> initiate the STOP condition on SDA and SCL pins
}
 
 
void I2C_write(uint8_t data) {
    I2C_wait();
    SSPBUF = data;  // load data into SSPBUF register
}
  
void lcd_send_data(unsigned char data) {
	unsigned char data_l, data_u;
	data_l = (data<<4)&0xf0;  //select lower nibble by moving it to the upper nibble position
	data_u = data&0xf0;  //select upper nibble
 
	I2C_start();
	I2C_write(SLAVE_ADDRESS_LCD);
	I2C_write(data_u|0x0D);  //enable=1 and rs =1
	I2C_write(data_u|0x09);  //enable=0 and rs =1
 
	I2C_write(data_l|0x0D);  //enable =1 and rs =1
	I2C_write(data_l|0x09);  //enable=0 and rs =1
 
	I2C_stop();
}
 
void lcd_send_cmd (unsigned char data) {
	unsigned char data_l, data_u;
	data_l = (data<<4)&0xf0;  //select lower nibble by moving it to the upper nibble position
	data_u = data&0xf0;  //select upper nibble
 
	I2C_start();
	I2C_write(SLAVE_ADDRESS_LCD);
	I2C_write(data_u|0x0C);  //enable=1 and rs =0
	I2C_write(data_u|0x08);  //enable=0 and rs =0
 
	I2C_write(data_l|0x0C);  //enable =1 and rs =0
	I2C_write(data_l|0x08);  //enable=0 and rs =0
 
	I2C_stop();
}
 
void lcd_init (void) {
    I2C_init(100000);
    lcd_send_cmd(0x01);
	lcd_send_cmd(0x02);
	lcd_send_cmd(0x28);
	lcd_send_cmd(0x0c);
	lcd_send_cmd(0x80);
}
 
void lcd_send_string (char *str) {
	while (*str) {
        lcd_send_data (*str++);
    }
}
 