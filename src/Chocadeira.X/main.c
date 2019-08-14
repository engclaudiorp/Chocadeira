#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#include "main.h"
#include "defines.h"
#include "I2C.h"
#include "dht11.h"


void Teste(int valor){
    char str[20] = {0,};
    sprintf(str, "%d", valor);
    lcd_send_cmd(0xc0);
    lcd_send_string(str);
}


void main (void) { 
    uint8_t temperature, erro;
    dht11_init();
    lcd_init();
    while(1) {
        erro = dht11_start_measurement();
        Teste(erro);
        __delay_ms(2000);
        if (erro == DHT11_NO_ERROR) {
            temperature = dht11_temperature(); 
            Teste(temperature);
        }
    }
}