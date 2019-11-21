/*
 * MasterBabyWarmer.cpp
 *
 * Created: 11/8/2019 12:45:14 PM
 *  OWNER: NIC
 */ 


/*
 * MasterBabyWarmer.cpp
 *
 * Created: 6/6/2019 8:01:04 AM
 *  Author: swain
 */ 

/*
 * MasterBabyWarmer.cpp
 *
 * Created: 11/10/2019 10:05:04 AM
 *  Co-Author: Abhish Khanal
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "Header.h"
#include "Timerr.h"
#include "Pins.h"
#include "Controls.h"
#include "uart.h"
#include "max31865.h"
#include "max7219.h"
#include "led.h"
#include "uart.h"

#define BAUD 9600
#define BUFF_LEN 700
#define BAUD_PRESCALE (((F_CPU / (BAUD * 16UL))) - 1)

#define HYPOTHERMIALOWTEMPERATURE	36.5
#define HYPERTHERMIAHIGHTEMPERATURE	37.2

void init_devices();
void check();
void sendToDisplay(float air, float skin, float set);
void sendToDisplayBelow(float temp);
void displaySensFail();
void displayPreHeat();
void displaySetTemperature();
void displayON();
void displayOFF();
void displayBABY();
void titititi();
void titi();

//monitoring for 
bool startMonitor = false;
int second = 0;

//TIMER FOR DISPLAY UPDATE
Timerr timerr;
bool displayUpdate = false;

//FAN HEATER BUZZER
Controls controls;
bool start_buzzer = false;
int buzzer_count = 0;
bool buzzer_timer = false;

//PT100 TEMPERATURE
Adafruit_MAX31865 maxAir;
Adafruit_MAX31865 maxSkin;

float skin_temperature = 0.0;
float air_temperature = 0.0;
float set_temperature = 30.5;
float set_temperature_copy = set_temperature;
float pre_heat_max_temp = 30.0;

volatile int display_count = 0;
volatile int display_count_max = 2;
int optimumTemp = 35;

//DISPLAY
max7219 max1;
max7219 max2;

//LED
led Led;

//buttons
bool set_up_pressed = false;
bool set_pressed = false;
bool set_down_pressed = false;
bool buzzer_stop_pressed = false;
bool stop_buzzer = false;
bool button_change = false;
bool emergencyAlert = false;
//preheat heater
bool preHeat = false;

//nonstandard/checking variables
uint32_t countpreheat = 0;


int main(void)
{	
	initUART0();
	init_devices();
	//
// 	while(1){
// 		if(bit_is_clear(BUZZER_STOP_BUTTON_PORT, BUZZER_STOP_BUTTON_PIN)){
// 			countpreheat++;
// 			if(countpreheat > 1000000){
// 				break;
// 			}
// 		}
// 		else{
// 			countpreheat = 0;
// 		}
// 		
// 	}
// 	//
	// ti ti ti ti
	titi();

// 	controls.stopBuzzer();
	////////////////////
	
	//wait while preheat condition is meet.
	bool clear_display = false;
	bool set_temp_display = false;
   	while(!preHeat){
		  
  		 if(displayUpdate){
			clear_display = !clear_display;
			
 			displayUpdate = false;
   			maxSkin.begin(1);
   			skin_temperature = maxSkin.temperature(100.0,430.0);
     		max1.MAX7219_init();
     		max2.MAX7219_init();
 			//these above init should be done to initialize spi 
    			//sendToDisplay(air_temperature,skin_temperature,set_temperature);
 			displayPreHeat();
			 if(clear_display) {
				 max2.MAX7219_clearDisplay();
				 //max1.MAX7219_clearDisplay();
			 } else {
  				sendToDisplayBelow(skin_temperature);
			 }
			 if(skin_temperature > pre_heat_max_temp) {
 				controls.stopHeater();
 				preHeat = true;
 				titititi();
 				}
 			else if(skin_temperature < pre_heat_max_temp) {
 				controls.startHeater();
 				}
  		 }
  		  
  		  else{
 			   asm volatile ("nop");
  		  }
  			 
   	}
	   
	while(1)
	
	{
 		check();
 		if(displayUpdate) {
 			//spi for max7219 for display
 			max1.MAX7219_init(); // these are init
 			max2.MAX7219_init();
			 
			 //buzzer karaing
			 if(start_buzzer) {
				 buzzer_timer = !buzzer_timer;
				 if(buzzer_timer) {
					 controls.startBuzzer();
				 } else {
					 controls.stopBuzzer();
				 }
			 }
			 // sending this
			 if(button_change){
				 set_temp_display = !set_temp_display;
				 if(set_temp_display) {
					sendToDisplayBelow(set_temperature_copy);
				 } else {
					max2.MAX7219_clearDisplay(); 
				 }
				 displaySetTemperature();
			 }
			 
			 else{
				 sendToDisplay(air_temperature, skin_temperature, set_temperature);
			 }
			 	
 			//spi for temperature pt100
 			maxAir.begin(0); //these are init 
 			maxSkin.begin(1);
 			skin_temperature = maxSkin.temperature(100.0, 430.0);
			air_temperature = maxAir.temperature(100.0, 430.0);
			 
				 if(skin_temperature >= (set_temperature-0.1)) {
					 controls.stopHeater();
					 }else if(skin_temperature <= (set_temperature - 0.2)) {
					 controls.startHeater();
				 }
 			                                                                                                                                                                                                                               
 			displayUpdate = false;
 		}
	 }
}



void init_devices() {
	
	//_delay_ms(1000);	
	
	sei();
	Led.led_init();
	
	//heater and buzzer
	controls.initControls();

	//led
	Led.led_init();
	//for time
	timerr.setTimerNum(1);
	timerr.setCompareInterrupt();
	timerr.startCustomTimer(200);
	
	//for pt100
	//maxAir.begin(1);
	//maxSkin.begin(0);
	
	//skin_temperature = maxSkin.temperature(100.0, 430.0);
	//air_temperature = maxAir.temperature(100.0, 430.0);
	
	//for display
	max1.MAX7219_init();
	max1.MAX7219_set(0,4,4);
	
	_delay_ms(10);
	max2.MAX7219_set(1, 4, 4);
	_delay_ms(10);
	max1.MAX7219_init();
	_delay_ms(10);
	max2.MAX7219_init();
	_delay_ms(10);
	max1.MAX7219_clearDisplay();
	_delay_ms(30);
	max2.MAX7219_clearDisplay();
	_delay_ms(30);
	//check for pre heat, wait while preheat is long pressed, otherwise don't run baby warmer. 
	
	
}


void check() {
	
	if(skin_temperature >= (set_temperature+0.2)) {
		if(!startMonitor) {
			//controls.startBuzzer();
			start_buzzer = true;	
		}	
		
		if(stop_buzzer && !startMonitor) {
			startMonitor = true;
			second = 0;
			stop_buzzer = false;
			start_buzzer = false;
			//controls.stopBuzzer();
		}
		
		Led.led_do(TS_HIGH_LED, 1);
		//Led.led_do(TA_HIGH_LED, 1);
	} else {
		controls.stopBuzzer();
		start_buzzer = false;
		Led.led_do(TS_HIGH_LED, 0);
		startMonitor = false;
		second = 0;
	}
	
	if(second >= 600) {
		//why this below line needed when if startMonitor = false, and skin temperature still set_temperature + 0.2 
		//controls.startBuzzer();
		startMonitor = false;
		second = 0;
	}
		
	if(air_temperature > 37) {
		//controls.startBuzzer();
		//Led.led_do(TS_HIGH_LED, 1);
		Led.led_do(TA_HIGH_LED, 1);
	} else{
		//controls.stopBuzzer();
		Led.led_do(TS_HIGH_LED, 0);
	} 
	
	//buzzer stop button
	if(bit_is_clear(BUZZER_STOP_BUTTON_PORT, BUZZER_STOP_BUTTON_PIN) && !buzzer_stop_pressed ) {
		
		controls.stopBuzzer();
		stop_buzzer = true;
		buzzer_stop_pressed = true;
	}
	
	else if(bit_is_set(BUZZER_STOP_BUTTON_PORT, BUZZER_STOP_BUTTON_PIN)) {
		buzzer_stop_pressed = false;
	}
	
	//set up button
	
	if(bit_is_clear(SET_UP_BUTTON_PORT, SET_UP_BUTTON_PIN) && !set_up_pressed) {
		
		if(button_change) {
			set_temperature_copy += 0.1; 
		}
		set_up_pressed = true;
	}
	
	else if(bit_is_set(SET_UP_BUTTON_PORT, SET_UP_BUTTON_PIN)) {
		set_up_pressed = false;
	}
	
	
	//set down button
	if(bit_is_clear(SET_DOWN_BUTTON_PORT, SET_DOWN_BUTTON_PIN) && !set_down_pressed) {
		
		if(button_change) {
			set_temperature_copy -= 0.1;
		}
		
		set_down_pressed= true;
	}
	
	else if(bit_is_set(SET_DOWN_BUTTON_PORT, SET_DOWN_BUTTON_PIN)) {
		set_down_pressed = false;
	}
	
	//set button
	if(bit_is_clear(SET_BUTTON_PORT, SET_BUTTON_PIN) && !set_pressed) {
			button_change = !button_change;
			if(!button_change) {
				set_temperature = set_temperature_copy;
				titi();
			}
			
			set_pressed = true;
		}
	
	else if(bit_is_set(SET_BUTTON_PORT, SET_BUTTON_PIN)) {
		set_pressed = false;
	}
	
}


//// ALL ISR Below
ISR(TIMER1_COMPA_vect) {
	TCNT1 = 0;
	if(startMonitor) {
		second++;
	}
	display_count++;
	if(display_count >= display_count_max) {
		displayUpdate = true;
		display_count = 0;
	}
}

//////////////ALL DISPLAY CODE BELOW/////////////////
void sendToDisplay(float air, float skin, float set)  {
	max1.MAX7219_writeData(MAX7219_MODE_DECODE,0xFF);
	max2.MAX7219_writeData(MAX7219_MODE_DECODE,0xFF);
	
	max1.MAX7219_writeData(4,0);
	max1.MAX7219_writeData(8, air / 10);
	max1.MAX7219_writeData(6, ((int)air % 10) | 0b10000000);
	
	max1.MAX7219_writeData(2, (int)(air * 10) % 10);
	
	max1.MAX7219_writeData(3,0);
	max1.MAX7219_writeData(7, skin / 10);
	max1.MAX7219_writeData(5, ((int)skin % 10) | 0b10000000);
	
	max1.MAX7219_writeData(1, (int)(skin * 10) % 10);
	
	//max2.MAX7219_writeData(1, set / 10);
	
	//max2.MAX7219_writeData(5, (int)set % 10);
	//max2.MAX7219_writeData(4, (int)(set*10) % 10);
	max2.MAX7219_writeData(6, (set / 10));
	
	max2.MAX7219_writeData(8, ((int)set % 10)| 0b10000000);
	max2.MAX7219_writeData(2, (int)(set*10) % 10);
}

void sendToDisplayBelow(float temp){
	max2.MAX7219_writeData(MAX7219_MODE_DECODE,0xFF);
	max2.MAX7219_writeData(6, (temp / 10));
	max2.MAX7219_writeData(8, ((int)temp % 10)| 0b10000000);
	max2.MAX7219_writeData(2, (int)(temp*10) % 10);
}

void displaySensFail()
{
	
	max1.MAX7219_writeData(MAX7219_MODE_DECODE,0x00);
	//first 4-7segment display from left to right
	max1.MAX7219_writeData(3,S);
	max1.MAX7219_writeData(7,E);
	max1.MAX7219_writeData(5,N);
	max1.MAX7219_writeData(1,S);
	
	//second 4-7 segment display from left to right
	max1.MAX7219_writeData(4,F);	
	max1.MAX7219_writeData(8,A);
	max1.MAX7219_writeData(6,I);
	max1.MAX7219_writeData(2,L);
}

void displaySetTemperature(){
	max1.MAX7219_writeData(MAX7219_MODE_DECODE,0x00);
	//first 4-7segment display from left to right
	max1.MAX7219_writeData(3,das);
	max1.MAX7219_writeData(7,S);
	max1.MAX7219_writeData(5,E);
	max1.MAX7219_writeData(1,t);
	
	//second 4-7 segment display from left to right
	max1.MAX7219_writeData(4,t);
	max1.MAX7219_writeData(8,P);
	max1.MAX7219_writeData(6,das);
	max1.MAX7219_writeData(2,das);
}

void displayPreHeat(){
	max1.MAX7219_writeData(MAX7219_MODE_DECODE,0x00);
	//first 4-7segment display from left to right
	max1.MAX7219_writeData(3,das);
	max1.MAX7219_writeData(7,P);
	max1.MAX7219_writeData(5,R);
	max1.MAX7219_writeData(1,E);
	
	//second 4-7 segment display from left to right
	max1.MAX7219_writeData(4,H);
	max1.MAX7219_writeData(8,E);
	max1.MAX7219_writeData(6,A);
	max1.MAX7219_writeData(2,t);
}

void displayON(){
	max1.MAX7219_writeData(MAX7219_MODE_DECODE,0x00);
	//first 4-7segment display from left to right
	max1.MAX7219_writeData(3,das);
	max1.MAX7219_writeData(7,O);
	max1.MAX7219_writeData(5,N);
	max1.MAX7219_writeData(1,das);
	
	//second 4-7 segment display from left to right
	max1.MAX7219_writeData(4,das);
	max1.MAX7219_writeData(8,O);
	max1.MAX7219_writeData(6,F);
	max1.MAX7219_writeData(2,F);
	
}

void displayOFF(){
	max1.MAX7219_writeData(MAX7219_MODE_DECODE,0x00);
	//first 4-7segment display from left to right
	max1.MAX7219_writeData(3,das);
	max1.MAX7219_writeData(7,O);
	max1.MAX7219_writeData(5,N);
	max1.MAX7219_writeData(1,das);
	
	//second 4-7 segment display from left to right
	max1.MAX7219_writeData(4,das);
	max1.MAX7219_writeData(8,O);
	max1.MAX7219_writeData(6,F);
	max1.MAX7219_writeData(2,F);
}

void displayBABY(){
	max1.MAX7219_writeData(MAX7219_MODE_DECODE,0x00);
	//first 4-7segment display from left to right
	max1.MAX7219_writeData(3,B);
	max1.MAX7219_writeData(7,A);
	max1.MAX7219_writeData(5,B);
	max1.MAX7219_writeData(1,Y);
	
	//second 4-7 segment display from left to right
	max1.MAX7219_writeData(4,B);
	max1.MAX7219_writeData(8,A);
	max1.MAX7219_writeData(6,B);
	max1.MAX7219_writeData(2,Y);
}
// Order of right and left 4-7 segment matrix registers.
// 3 7 5 1      4 8 6 2


void titititi(){
	controls.startBuzzer();
	_delay_ms(100);
	controls.stopBuzzer();
	_delay_ms(100);
	controls.startBuzzer();
	_delay_ms(100);
	controls.stopBuzzer();
	_delay_ms(300);
	controls.startBuzzer();
	_delay_ms(100);
	controls.stopBuzzer();
	_delay_ms(100);
	controls.startBuzzer();
	_delay_ms(100);
}

void titi(){
	controls.startBuzzer();
	_delay_ms(100);
	controls.stopBuzzer();
	_delay_ms(100);
	controls.startBuzzer();
	_delay_ms(100);
	controls.stopBuzzer();
	_delay_ms(100);
}
