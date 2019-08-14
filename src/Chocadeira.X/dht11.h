#ifndef DHT11_H
#define DHT11_H

#include <stdint.h>

#define DHT11_NO_ERROR        0u
#define DHT11_NO_RESPONSE     1u
#define DHT11_CHECKSUM_ERROR  2u
#define DHT11_UNKNOWN         3u

// Function Prototype
void dht11_init(void);
uint8_t dht11_start_measurement(void);
uint8_t dht11_temperature(void);
uint8_t dht11_humidity(void);
void dht11_int_handler(void);

#endif