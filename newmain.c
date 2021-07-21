/*
 * File:   newmain.c
 * Author: jcaf
 * MPLAB X IDE v4.05 + PICKIT2 + Arch Linux
 * AN0 -> NTC10K
 * AN1 -> FSR400-FORCE
 * AN2 -> Resistencia variable de 0 - 200 ohms
 * 
 * Freq = crystal 16 MHz - > XTAL-110592-UCRISTAL DE CUARZO DE 11.0592 MHz 49U
 * 
 * Created on May 7, 2021, 5:43 PM
 * 
 * pk2cmd -PPIC16F88 -B./ -R -X -J -M -F/run/media/jcaf/mydata/pleasure/PROYECTOS/ATMEL/C/Wilfredo/MedicionTemperaturaFuerza/MedicionTemperaturaFuerza/firmware/pic16f88_medTempFuerzaResis.X/dist/default/production/pic16f88_medTempFuerzaResis.X.production.hex

 * OK SABADO 26 JUN 2021
 * 
 * Cambiando a cristal 11.0592 MHz 
 */


#include <xc.h>
#include <stdint.h>
#include "system.h"
#include "types.h"
#include "temperature.h"
#include "adc/adc.h"
#define myitoa(_integer_, _buffer_, _base_) itoa(_buffer_, _integer_, _base_) 

#pragma config "BOREN = OFF", "DEBUG = OFF", "FOSC = HS", "MCLRE = ON", "WDTE = OFF", "CP = ON", "LVP = OFF", "PWRTE = ON"

void tx_char(char c)
{
    while (TXIF == 0)
    {;}
    
    TXREG = c;
}
void usart_print_string(const char *p)
{
    while (*p)
    {
        tx_char(*p);
        p++;
    }
}
//uint8_t checksum(char *str);
uint8_t checksum(char *str, uint8_t length);
char bin_to_asciihex(char c)//nibbleBin_to_asciiHex
{
    if (c < 10)
        return c+'0';
    else
        return (c-10) + 'A';
}
/*
 *@N512F1023R256C21
@N512F1023R257C22
 */
void main(void) 
{
    char str[30];
    char buff[30];
    uint16_t ADRESHL;
    
    uint8_t checks = 0;
    //
    OSCCON = 0x64;

    PORTB = 0x00;//PB5 = TX

    PORTA = 0x00;
    TRISA = 0B00000111;
    ADC_init();
    
    //UART @1200 baud
    //(Asynchronous) Baud Rate = FOSC/(64(X + 1))
#define BAUDRATE 1200    
#define SPRBG_VALUE ((F_CPU/(64.0 * BAUDRATE)) -1)
    //SPBRG = 207;// @16 Mhz 1200BAUD
    SPBRG = SPRBG_VALUE;//143 @ 11.0592E6
    
    TXSTA=0B00100000;
    RCSTA=0B10000000;
    //
    uint16_t ADRESHL_NTC10K = 0;
    uint16_t ADRESHL_FSR400 = 0;
    uint16_t ADRESHL_RES = 0;
    
    while (1)
    {
        //Get NTC10K temperature
        ADC_go(0);
        ADRESHL_NTC10K = ((ADRESH & 0x03) << 8) | ADRESL;
//ADRESHL_NTC10K = 512;        
        //NTC10K_Temp = ntc10k_st(ADRESHL, ADC_MAX_VAL_1023);
        
        //Get FSR400-FORCE
        ADC_go(1);
        ADRESHL_FSR400 = ((ADRESH & 0x03) << 8) | ADRESL;
//ADRESHL_FSR400 = 128;        
        
        //Get RESISTANCE
        ADC_go(2);
        ADRESHL_RES = ((ADRESH & 0x03) << 8) | ADRESL;
//ADRESHL_RES = 64;        
        
        //Construir payload data + checksum
        //@NxxxxFxxxxRxxxxCcc'\r\n'
        strcpy(str,"@N");
        //
        myitoa(ADRESHL_NTC10K, buff, 10);
        strcat(str,buff);
        //
        strcat(str,"F");
        myitoa(ADRESHL_FSR400, buff, 10);
        strcat(str,buff);
        //
        strcat(str,"R");
        myitoa(ADRESHL_RES, buff, 10);
        strcat(str,buff);
        
        //En este punto, antes de adicionar los demas chararteres
        //calculo el checksum
        checks = checksum(str, strlen(str));
        
        strcat(str,"C");//Checksum
        
        buff[1] = '\0';
        buff[0] = bin_to_asciihex(checks>>4);//primero la parte alta
        strcat(str,buff);
        buff[0] = bin_to_asciihex(checks & 0x0f);//primero la parte baja
        strcat(str,buff);
        
        //myitoa(checks, buff, 10);
        //El checksum siempre sera de 2 caracteres, si es <10, entonces
        //se rellena con "0"
      
        //
        //strcat(str,"\r\n");
        strcat(str,"\r");//0x0D
        
        usart_print_string(str);
        __delay_ms(20);
    }
                
    return;
}
uint8_t checksum(char *str, uint8_t length)
{
    uint16_t acc = 0;
    for (int i=0; i< length; i++)
    {
        acc += str[i];
    }
    return (uint8_t)(acc);
}

/*
uint8_t checksum(char *str)
{
    uint8_t len = strlen(str);
    uint16_t acc = 0;
    for (int i=0; i< len; i++)
    {
        acc += str[i];
    }
    return (uint8_t)(acc);
}
*/