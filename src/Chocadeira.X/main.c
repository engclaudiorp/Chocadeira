#include <xc.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <stdint.h>
#include "main.h"
#include "defines.h"
#include "I2C.h"
//#include "dht11.h"

#define DHT11_PIN      PORTBbits.RB2
#define DHT11_PIN_DIR  TRISBbits.TRISB2

//#pragma config CONFIG1 = 0x2CD4
//#pragma config CONFIG2 = 0x0700

// variables declaration
char Temperature[] = "Temp = 00.0 C  ";
char Humidity[]    = "RH   = 00.0 %  ";
unsigned char T_Byte1, T_Byte2, RH_Byte1, RH_Byte2, CheckSum ;
 
// send start signal to the sensor
void Start_Signal(void) {
  DHT11_PIN_DIR = 0;     // configure DHT11_PIN as output
  DHT11_PIN = 0;         // clear DHT11_PIN output (logic 0)
 
  __delay_ms(25);        // wait 25 ms
  DHT11_PIN = 1;         // set DHT11_PIN output (logic 1)
 
  __delay_us(30);        // wait 30 us
  DHT11_PIN_DIR = 1;     // configure DHT11_PIN as input
}
 
// Check sensor response
__bit Check_Response() {
  TMR1H = 0;                 // reset Timer1
  TMR1L = 0;
  TMR1ON = 1;                // enable Timer1 module
 
  while(!DHT11_PIN && TMR1L < 100);  // wait until DHT11_PIN becomes high (checking of 80µs low time response)
 
  if(TMR1L > 99)                     // if response time > 99µS  ==> Response error
    return 0;                        // return 0 (Device has a problem with response)
 
  else
  {
    TMR1H = 0;               // reset Timer1
    TMR1L = 0;
 
    while(DHT11_PIN && TMR1L < 100); // wait until DHT11_PIN becomes low (checking of 80µs high time response)
 
    if(TMR1L > 99)                   // if response time > 99µS  ==> Response error
      return 0;                      // return 0 (Device has a problem with response)
 
    else
      return 1;                      // return 1 (response OK)
  }
}
 
// Data read function
__bit Read_Data(unsigned char* dht_data)
{
  *dht_data = 0;
 
  for(char i = 0; i < 8; i++)
  {
    TMR1H = 0;             // reset Timer1
    TMR1L = 0;
 
    while(!DHT11_PIN)      // wait until DHT11_PIN becomes high
      if(TMR1L > 100) {    // if low time > 100  ==>  Time out error (Normally it takes 50µs)
        return 1;
      }
 
    TMR1H = 0;             // reset Timer1
    TMR1L = 0;
 
    while(DHT11_PIN)       // wait until DHT11_PIN becomes low
      if(TMR1L > 100) {    // if high time > 100  ==>  Time out error (Normally it takes 26-28µs for 0 and 70µs for 1)
        return 1;          // return 1 (timeout error)
      }
 
     if(TMR1L > 50)                  // if high time > 50  ==>  Sensor sent 1
       *dht_data |= (1 << (7 - i));  // set bit (7 - i)
  }
 
  return 0;                          // return 0 (data read OK)
}

/*
void Teste(int valor){
    char str[20] = {0,};
    sprintf(str, "%d", valor);
    lcd_send_cmd(0xc0);
    lcd_send_string(str);
}
*/

void main () { 
  OSCCON =  0X70;       // set internal oscillator to 8MHz
  ANSELH = 0;           // configure all PORTB pins as digital
  T1CON  = 0x10;        // set Timer1 clock source to internal with 1:2 prescaler (Timer1 clock = 1MHz)
  TMR1H  = 0;           // reset Timer1
  TMR1L  = 0;
  //  uint8_t temperature, erro;
  //  uint8_t temperature_text[5];
  //  dht11_init();
    lcd_init();
    while(1) {
     //  dht11_start_measurement();
       // Teste(erro);
      //  __delay_ms(2000);
     //   if (erro == DHT11_NO_ERROR) {
         //   temperature = dht11_temperature(); 
         //   Teste(temperature);
           
      //  }
         Start_Signal();     // send start signal to the sensor
 
    if(Check_Response())    // check if there is a response from sensor (If OK start reading humidity and temperature data)
    {
      // read (and save) data from the DHT11 sensor and check time out errors
      if(Read_Data(&RH_Byte1) || Read_Data(&RH_Byte2) || Read_Data(&T_Byte1) || Read_Data(&T_Byte2) || Read_Data(&CheckSum))
      {
          lcd_send_cmd(0x01);
        lcd_send_string("Time out!");   // display "Time out!"
      }
 
      else         // if there is no time out error
      {
        if(CheckSum == ((RH_Byte1 + RH_Byte2 + T_Byte1 + T_Byte2) & 0xFF))
        {                                       // if there is no checksum error
          Temperature[7]  = T_Byte1 / 10  + '0';
          Temperature[8]  = T_Byte1 % 10  + '0';
          Temperature[10] = T_Byte2 / 10  + '0';
          Humidity[7]     = RH_Byte1 / 10 + '0';
          Humidity[8]     = RH_Byte1 % 10 + '0';
          Humidity[10]    = RH_Byte2 / 10 + '0';
          Temperature[11] = 223;    // put degree symbol (°)
          lcd_send_cmd(0x80);           // go to column 1, row 2
          lcd_send_string(Temperature);
          lcd_send_cmd(0xc0);           // go to column 1, row 2
          lcd_send_string(Humidity);
        }
 
        // if there is a checksum error
        else
        {
         lcd_send_cmd(0x01);
          lcd_send_string("Checksum Error!");
        }
 
      }
    }
 
    // if there is a response (from the sensor) problem
    else
    {
      lcd_send_cmd(0x01);
      lcd_send_string("No response");
      lcd_send_cmd(0xc0);           // go to column 1, row 2
      lcd_send_string("from the sensor");
    }
 
    TMR1ON = 0;        // disable Timer1 module
    __delay_ms(1000);  // wait 1 second
    }
}
