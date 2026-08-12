/* delay.c */
/* status : working fine */

#include<lpc21xx.h>
void delay_ms(unsigned int ms){
	T0PR=15000-1;
	T0PC=0;
	T0TC=0;
	T0TCR=2;
	T0TCR=1;
	while(T0TC<ms);
	T0TCR=0;
}

void delay_s(unsigned int s){
	T0PR=15000000-1;
	T0TC=0;
	T0PC=0;
	T0TCR=2;
	T0TCR=1;
	while(T0TC<s);
	T0TCR=0;
}

void delay_us(unsigned int us){
	T0PR=15-1;
	T0TC=0;
	T0PC=0;
	T0TCR=2;
	T0TCR=1;
	while(T0TC<us);
	T0TCR=0;
}

//////////////////////////////////////////////////////////////////////////////////

/* lcd_4bit_driver */
/* status : ----- */
/* pending : float fun */

#include<lpc21xx.h>
#include "header.h"
void lcd_data(unsigned int data){
	unsigned int temp;
	IOCLR1=0XFE<<16;	  
	temp=(data&0xf0)<<16;
	IOSET1=temp;
	IOSET1=1<<17;  // RS=1
	IOCLR1=1<<18;  //RW=0
	IOSET1=1<<19;  //EN=1
	delay_ms(2);
	IOCLR1=1<<19;  //EN=0

	IOCLR1=0XFE<<16;	  
	temp=(data&0x0f)<<20;
	IOSET1=temp;
	IOSET1=1<<17;  // RS=1
	IOCLR1=1<<18;  //RW=0
	IOSET1=1<<19;  //EN=1
	delay_ms(2);
	IOCLR1=1<<19;  //EN=0
}

void lcd_cmd(unsigned int cmd){
	unsigned int temp;
	IOCLR1=0XFE<<16;	  
	temp=(cmd&0xf0)<<16;
	IOSET1=temp;
	IOCLR1=1<<17;  // RS=1
	IOCLR1=1<<18;  //RW=0
	IOSET1=1<<19;  //EN=1
	delay_ms(2);
	IOCLR1=1<<19;  //EN=0

	IOCLR1=0XFE<<16;	  
	temp=(cmd&0x0f)<<20;
	IOSET1=temp;
	IOCLR1=1<<17;  // RS=1
	IOCLR1=1<<18;  //RW=0
	IOSET1=1<<19;  //EN=1
	delay_ms(2);
	IOCLR1=1<<19;  //EN=0
}

void lcd_init(void){
	IODIR1=0XFE<<16;
	PINSEL2  |=0X0;
	lcd_cmd(0x02);	 
	lcd_cmd(0x28);
	lcd_cmd(0x0e);   //enable the row and col
	lcd_cmd(0x01);   // clear the screen
}

void lcd_string(char *p){
	int i;
	for(i=0; p[i]; i++)
		lcd_data(p[i]);
}	


void lcd_integer(int num){
	int a[10],i;
	if(num<0){
		lcd_data('-');
		num=-num;
	}			
	if(num==0){
		lcd_data('0');
		return;
	}
	for(i=0; num; num/=10,i++)
		a[i]=num%10+48;
	for(i-=1; i>=0; i--)
		lcd_data(a[i]);
}

void lcd_float(float f){
	int num;
	if(f<0){
		lcd_data('-');
		f=-f;
	}
	if(f==0){
		lcd_string("0.0");
		return;
	}
	num=f;
	lcd_integer(num);
	lcd_data('.');
	num=(f-num)*1000000;
	lcd_integer(num);
}


void lcd_clear(void){
	lcd_cmd(0x01);
	delay_ms(2);
}


/////////////////////////////////////////////////////////////////////

/*uart0_driver*/
/*status : working all funtion tested */
// check again float:
#include<lpc21xx.h>
#include "header.h"
#define RDR (U0LSR>>0&1)
void uart0_init(unsigned int baud){
	int pclk, result=0;
	if(VPBDIV==0x00)
		pclk=15000000;
	else if(VPBDIV==0x01)
		pclk=60000000;
	else if(VPBDIV==0x02)
		pclk=30000000;

	result=pclk/(16*baud);

	PINSEL0 |= 0x05;
	U0LCR=0x83;
	U0DLL=(result & 0xff);
	U0DLM=(result>>8)&0xff;
	U0LCR=0x03;
}

void uart0_tx(unsigned char data){
	U0THR=data;
	while((U0LSR>>5&1)==0);
}

unsigned char uart0_rx(void){
	while(RDR==0);
	return U0RBR;
}

void uart0_tx_string(char *p){
	int i;
	for(i=0; p[i]; i++)
		uart0_tx(p[i]);
}			

void uart0_rx_string(char *p){
	int i;
	unsigned char t;
	for(i=0; ;i++){
		t=uart0_rx();
		uart0_tx(t);
		if(t=='\r')
			break;
		p[i]=t;
	}
	p[i]='\0';
}

void uart0_rfid(char *p, int len){
	int i;
	for(i=0;i<len;i++){
		while(RDR==0);
		p[i]=U0RBR;
		if(p[i]=='\r')
			break;
	}
	p[i]='\0';
}


void uart0_integer(int num){
	int a[10], i;
	if(num==0){
		uart0_tx('0');
	 	return;
	}else if(num<0){
		num=-num;
		uart0_tx('-');
	}
	i=0;
	while(num){
		a[i]=num%10+48;
		num/=10;
        i++;
	}
	for(i=i-1; i>=0; i--)
		uart0_tx(a[i]);	
}

void uart0_float(float f){
	int num;
	if(f==0){
		uart0_tx_string("0.0");
		return;
	}
	if(f<0){
		f=-f;
		uart0_tx('-');
	}
	num=f;
	uart0_integer(num); //123.5
	uart0_tx('.');
	f=(f-num)*1000000;
	uart0_integer(f);
}

///////////////////////////////////////////////////////////////////////////

/*adc_driver.c*/
#include<lpc21xx.h>
#include"header.h"
void adc_init(void){
    PINSEL1 |= 0x15400000;
    ADCR = 0x200400;
}

u32 adc_read(int ch_num){
    u32 result;
    ADCR |= (1<<ch_num);
    ADCR |= 1<<24;
    while(((ADDR>>31)&1)==0);
    ADCR ^= 1<<24;
    ADCR ^= 1<<ch_num;
    result = ((ADDR>>6)&0x3FF);
    return result;
}

//////////////////////////////////////////////////////////////////////////////

/*i2c_driver.c*/

#include<lpc21xx.h>
#include"header.h"
void i2c_init(void){
	PINSEL0 |= 0x50;
	I2SCLH=I2SCLL=75;
	/*select master mode*/
	I2CONCLR=(1<<2);  // AA=0
	I2CONSET=(1<<6);  // I2EN=1
}

#define SI ((I2CONSET>>3)&1) 
void i2c_send(u8 sa, u8 mr, u8 data){
	/*start condition*/
	I2CONSET=1<<5;  // STA=1
	I2CONCLR=1<<3;  // SI=0
	while(SI==0);
	I2CONCLR=1<<5;  // STA=0

	I2DAT=sa;
	I2CONCLR=1<<3;  // SI=0
	while(SI==0);
	if(I2STAT==0x20){
		uart0_tx_string("err : SA+W \r\n");
		goto exit;
	}

	I2DAT=mr;
	I2CONCLR=1<<3;	// SI=0
	while(SI==0);
	if(I2STAT==0x30){
		uart0_tx_string("err : Mr \r\n");
		goto exit;
	}

	I2DAT=data;
	I2CONCLR=1<<3;  // SI=0
	while(SI==0);
	if(I2STAT==0x30){
		uart0_tx_string("err : DATA \r\n");
		goto exit;
	}
	/*stop condition*/
	exit:
	I2CONSET=1<<4;  // STO=1
	I2CONCLR=1<<3;  // SI=0
}

u8 i2c_read(u8 sa, u8 mr){
	u8 res=0;
	I2CONSET=1<<5; //STA=1
	I2CONCLR=1<<3; // SI=0
	while(SI==0);
	I2CONCLR=1<<5; //STA=0
	
	I2DAT=sa^1;
	I2CONCLR=1<<3; // SI=0
	if(I2STAT==0x20){
		uart0_tx_string("err : SA+W \r\n");
		goto exit;
	}

	I2DAT=mr;
	I2CONCLR=1<<3;	// SI=0
	while(SI==0);
	if(I2STAT==0x30){
		uart0_tx_string("err : Mr \r\n");
		goto exit;
	}

	/*Generate Re-Start Condition*/
	I2CONSET=1<<5; //STA=1
	I2CONCLR=1<<3; // SI=0
	while(SI==0);
	I2CONCLR=1<<5; //STA=0

	I2DAT=sa;
	I2CONCLR=1<<3; // SI=0
	if(I2STAT==0x48){
		uart0_tx_string("err : SA+R \r\n");
		goto exit;
	}
	/*Read Data*/
	I2CONCLR=1<<3;   //  SI=0
	while(SI==0);
	res=I2DAT;
	uart0_tx_string("\r\nin i2c : ");
	uart0_integer(res);
		
	exit:
	I2CONSET=1<<4; 	 // STO=1
	I2CONCLR=1<<3;   // SI=0
	return res;
}

////////////////////////////////////////////////////////////////////////////////////////


