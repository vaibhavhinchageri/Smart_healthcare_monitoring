#include<lpc21xx.h>
#include"header.h"

//#define RED_LED (1<<16)
//#define GREEN_LED (1<<17)
//#define BUZZER (1<<18)
//#define RELAY (1<<19)
float temp,current,movement;
unsigned int pulse,saline,oxygen;
//unsigned char emergency;


void AI_decision(float temp,
				unsigned int pulse,
				unsigned int saline,
				unsigned int oxygen,
				unsigned char emergency,
				float current,float movement
				)
	
{


unsigned int flag=0;
	/*
		if(emergency){
			lcd_clear();
			lcd_string("EMERGENCY");
			IOSET0=RED_LED|BUZZER|RELAY;
			uart0_tx_string("emergency\r\n");
//			EEPROM_write_event();
			return;
		}
	*/

		if(temp>39){
			lcd_clear();
			lcd_string("HIGH TEMP");
			IOSET0=RED_LED|BUZZER;
			uart0_tx_string("\r\nHIGH TEMP\n");
			flag=1;
		}

		if(pulse<60||pulse>120){
			lcd_clear();
			lcd_string("HEART ALERT");
			IOSET0=RED_LED|BUZZER;
			flag=1;
		}
		
	   if(oxygen<250){
	   		lcd_clear();
			lcd_string("LOW OXYGEN");
			uart0_tx_string("\r\nLOW OXYGEN\n");
			IOSET0=RED_LED|BUZZER|RELAY;
			flag=1;
		}
		if(saline<20){
			lcd_clear();
			lcd_string("LOW SALINE");
			uart0_tx_string("\r\nLOW SALINE\n");
			IOSET0=RED_LED;
			flag=1;
		}
		if(movement){
			lcd_clear();
			lcd_string("PATIENT MOVE");
			uart0_tx_string("\r\nPATIENT MOVE\n");
			IOSET0=RED_LED;
			flag=1;
		}
		if(flag==0){
			lcd_clear();
			lcd_string("PATIENT OK");
			uart0_tx_string("\r\nPATIENT OK\n");
			IOSET0=GREEN_LED;
			IOCLR0=RED_LED|BUZZER|RELAY;
		}
}

///////////////////////////////////////////////////////////////////////////

/*other.c*/
/*emergency_alert()  -- working fine in terminal*/
/* pending rtc_time_getting */
#include<lpc21xx.h>
#include"header.h"
//#include"main_Smart_Healthcare.c"

int h, min, sec;
void emergency_alert(void){
	int led=0;

	lcd_clear();
	lcd_string("Emergency Alert");
	uart0_tx_string("\rEmergency Alert!!\n");
//	rtc_time_getting();	// get real time var - h, m, s


	/*EEPROM update*/
//	void EEPROM_init(void){
//	i2c_send(0xA0,0x0,h);
//	delay_ms(10);
//	i2c_send(0xA0,0x1,min);
//	delay_ms(10);
//	i2c_send(0xA0,0x2,sec);
//	delay_ms(10);


	while(1){
		led^=1;
		if(led)
			IOCLR0=RED_LED ;
		else
			IOSET0=RED_LED ;
	
		if(buzzer==1)
					IOSET0=BUZZER;     //continously making sound
		if(SW==0){
			while(SW==0);
			IOCLR0=BUZZER;     //stop sound
			if(led)
				IOSET0=RED_LED ; 	/* led  i have to stop */
			lcd_cmd(0x1);
			lcd_cmd(0x80);
			lcd_string("~Nurse came");
			uart0_tx_string("\r\n~Nurse came\n");
			break;
		}
	}
}



//void rtc_time_setting(void){
//	/*Set rtc time to 11:59:55PM*/
//	i2c_send(0xD0,0x0,0x55);
//	i2c_send(0xD0,0x1,0x59);
//	i2c_send(0xD0,0x2,0x23);
//}
//
// 
//void rtc_time_getting(void){
//	/*get current rtc time to the var*/
//	h = i2c_read(0xD0,0x2);
//	min=i2c_read(0xD0,0x1);
//	sec=i2c_read(0xD0,0x0);
//}


void intro(void){
	uart0_tx_string("\rAll init work Done\r\n");
	uart0_tx_string("\rPatient Monitoring System\r\n");
//  uart0_tx_string("\rSmart Healthcare\r\n");
	delay_ms(50);

	lcd_cmd(0x80);
	lcd_string("Patient Monitoring");
	lcd_cmd(0xc0);
	lcd_string("System");

	uart0_tx_string("\rDone By : \r\n");
	delay_ms(50);
   	uart0_tx_string("\rSai Suvarna\r\n");
	delay_ms(50);
   	uart0_tx_string("\rDeepak Kumar\r\n");
	delay_ms(50);
   	uart0_tx_string("\rVaibhav Hinchageri\r\n");
	delay_ms(50);
   	uart0_tx_string("\rVenketashwara Surendra\r\n");
	delay_ms(50);
}





