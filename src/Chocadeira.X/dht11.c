#include <xc.h>
#include "dht11.h"
#include "defines.h"

#define Data        PORTBbits.RB2
#define DataDir     TRISBbits.TRISB2 

static uint8_t TOUT = 0;
static uint8_t temperature;
static uint8_t humidity;
static uint8_t temp[5];

// Private Function Prototypes
void start_signal( void );
uint8_t check_response( void );
uint8_t read_bytes( void );

// Function Definition
void dht11_init( void )
{
  TOUT = 0;
  T1CON = 0x00;   // Turn-On the Timer when required
  TMR1IF = 0;
  TMR1H = 0xFF;   // Load Values for 10us
  TMR1L = 0x88;
  TMR1IE = 1; // Enable Timer Interrupt
  INTCON = 0xC0;
}

void dht11_int_handler( void )
{
  TMR1IF = 0; // Clear Flag
  TMR1H = 0xFF;   // Reload Values
  TMR1L = 0x88;
  TOUT++;
}

uint8_t dht11_start_measurement( void )
{
  uint8_t error = DHT11_NO_ERROR;
  uint8_t i, check, checksum;
  start_signal();
  check = check_response();
  if( check != DHT11_NO_ERROR )
  {
    error = DHT11_NO_RESPONSE;
   // UART1_Write_Text("Response Error\r\n");
  }
  else
  {
    checksum = 0;
    error = read_bytes();
    for( i=0; i<4; i++ )
    {
      checksum = checksum + temp[i];
    }
    if( checksum == temp[4] )
    {
      // Data is Valid
      humidity = temp[0];
      temperature = temp[2];
    }
    else
    {
      error = DHT11_CHECKSUM_ERROR;
    }
  }
  return error;
}

uint8_t dht11_temperature( void )
{
  return temperature;
}

uint8_t dht11_humidity( void )
{
  return humidity;
}

void start_signal( void )
{
  DataDir = 0;     // Data port is output
  Data    = 0;
  __delay_ms(25);
  Data    = 1;
  __delay_ms(30);
  DataDir = 1;     // Data port is input
}

uint8_t check_response( void )
{
  uint8_t error = DHT11_NO_ERROR;
  uint8_t counter = 0u;
  // Wait for Falling Edge
  while( Data )
  {
    counter++;
    __delay_us(1);
    if( counter == 80u )
    {
      error = DHT11_NO_RESPONSE;
    }
  }
  if( error == DHT11_NO_ERROR )
  {
    counter = 0u;
    // Wait For Rising Edge
    while( !Data )
    {
      counter++;
      __delay_us(1);
      if( counter > 100u )
      {
        error = DHT11_NO_RESPONSE;
        return error;
      }
    }
    counter = 0;
    // Rising Pulse is of 80us, wait here
    while( Data );
    {
      counter++;
      __delay_us(1);
      if( counter > 100u )
      {
        error = DHT11_NO_RESPONSE;
        return error;
      }
    }
  }
  return error;
}

uint8_t read_bytes( void )
{
  uint8_t error = DHT11_NO_ERROR;
  uint8_t num = 0;
  uint8_t i, j;
  uint8_t counter = 0;
  DataDir = 1;
  for( j=0; j<5u; j++ )
  {
    for( i=0; i<8u; i++ )
    {
      /*
      Signal will be like this
      50usec Low and after that if high duration is 70us, then 1 else 0
             _________
            |
            |
      ______|
      */
      // Wait for pin to go High
      while( !Data )
      {
        counter++;
        __delay_us(1);
        if( counter > 80u )
        {
          error = DHT11_UNKNOWN;
        }
      }
      counter = 0;
      // Measure the dursation of signal using timer
      T1CON = 0x01;       // Start the Timer
      // Wait for High to Low Pulse
      while( Data )
      {
        counter++;
        __delay_us(1);
        if( counter > 80u )
        {
          error = DHT11_UNKNOWN;
        }
      }
      counter = 0;
      T1CON = 0x00;       // Stop Timer and Check Counts
      if( TOUT > 5u )     // Greater than 50us means 1
      {
        num |= 0x01<<(7-i);
      }
      else                // Less than 50us means 0
      {
      }
      TOUT = 0x00;
      TMR1H = 0xFF;   // Load Values for 10us
      TMR1L        = 0x88;
    }
    temp[j] = num;
    num = 0u;
  }
  return error;
}