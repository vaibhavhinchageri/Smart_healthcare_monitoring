/* all_sensors definition */
#include<lpc21xx.h>
#include"header.h"

float read_temperature(void){
	unsigned int t;
	t=DHT11_ReadTemperature();
	return t;
}


unsigned int read_pulse(void){
//	return i2c_read_heart(0xAE,0xFF);
	return i2c_read(0xAE,0xFF);
}


float read_current(void){
	unsigned int adc;
	float voltage;
	adc=adc_read(3);
	voltage=((adc*3.3)/1023);
	return ((voltage-2.5)/0.185);
}


unsigned char read_movement(void){
	if(IR_SENSOR)
		return 1;
	else
		return 0;
}


/* PIR_sensor.c */
void PIR_init(void){
//configure p0.10 as input
	IODIR0&=~(1<<10);
}


unsigned char PIR_read(void){
	if(IOPIN0&(1<<10))
		return 1;
 	else
		return 0;
}






///////////////////////////////////////////////////////////////////////////////////////


#include<lpc21xx.h>
#include "header.h"

unsigned char RH_Int;
unsigned char RH_Dec;
unsigned char Temp_Int;
unsigned char Temp_Dec;
unsigned char CheckSum;
unsigned int timeout;


#define DHT_PIN 11

void DHT11_init(void){
	PINSEL0 &= ~(3<<22);      // P0.11 as GPIO
    IODIR0 |= (1<<DHT_PIN);
    IOSET0 |= (1<<DHT_PIN);
}

unsigned char DHT11_Start(void){
    /* Configure as Output */
    IODIR0 |= (1<<DHT_PIN);

    /* Pull Low for 18ms */
    IOCLR0 = (1<<DHT_PIN);
    delay_ms(18);

    /* Pull High for 30us */
    IOSET0 = (1<<DHT_PIN);
    delay_us(30);

    /* Configure as Input */
    IODIR0 &= ~(1<<DHT_PIN);


    /* Wait for Sensor Response */

//    while(IOPIN0 & (1<<DHT_PIN));

//    while(!(IOPIN0 & (1<<DHT_PIN)));

//    while(IOPIN0 & (1<<DHT_PIN));

//	unsigned int timeout = 10000;

	timeout = 10000;
	while((IOPIN0 & (1<<DHT_PIN)) && timeout--);
	if(timeout == 0) return 0;

	timeout = 10000;
	while((!(IOPIN0 & (1<<DHT_PIN))) && timeout--);
	if(timeout == 0) return 0;

	timeout = 10000;
	while((IOPIN0 & (1<<DHT_PIN)) && timeout--);
	if(timeout == 0) return 0;

	return 1;    
}



unsigned char DHT11_ReadByte(void){
    unsigned char i;
    unsigned char data = 0;
    for(i=0;i<8;i++){
        /* Wait for LOW to complete */
        timeout = 1000;
        while((!(IOPIN0 & (1<<DHT_PIN))) && timeout--);
        if(timeout == 0)
            return 0xFF;
        delay_us(40);

        if(IOPIN0 & (1<<DHT_PIN))
            data = (data<<1) | 1;
        else
            data = (data<<1);

       timeout = 1000;
       while((IOPIN0 & (1<<DHT_PIN)) && timeout--);
       if(timeout == 0)
			return 0xFF;
    }
    return data;
}



unsigned char DHT11_ReadTemperature(void){
    if(DHT11_Start()){
        RH_Int   = DHT11_ReadByte();
        RH_Dec   = DHT11_ReadByte();

        Temp_Int = DHT11_ReadByte();
        Temp_Dec = DHT11_ReadByte();

        CheckSum = DHT11_ReadByte();

        if((RH_Int + RH_Dec + Temp_Int + Temp_Dec) == CheckSum)
            return Temp_Int;
    }
    return 0xFF;          // Error
}



//////////////////////////////////////////////////////////////////////////////////////


#include<lpc21xx.h>
#include"uitra_header.h"
#define TRIG (1<<8)
#define ECH (1<<9)

int trig;
void ultrasonic_init(){
	IODIR0|=TRIG;
	IODIR0&=~(ECH);
	T0TCR=0x02;
	T0PR=59;
	T0TCR=1;
}
void send_pulse(){
	T0TC=0;
	T0PC=0;
	T0PR=59;
	IOSET0=TRIG;
	delay_ms(100);
	IOCLR0=TRIG;
	IODIR0&=~(ECH);
	trig=0;
}
	

unsigned int get_range(){
	unsigned int time;
	unsigned int d;
	send_pulse();
	while(!((IOPIN0&(ECH))));
	T0TCR=1;
	while(((IOPIN0&ECH)));
	T0TCR=0;
	time=T0TC;
	d=(0.0343*time)/2;
	return d;
}









