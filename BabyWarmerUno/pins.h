/*
 * pins.h
 *
 * Created: 5/31/2019 2:16:45 AM
 *  Author: swain
 */ 


#ifndef PINS_H_
#define PINS_H_

#define DDR(x) (*(&x - 1))      /* address of data direction register of port x */

//For Heater Control /
#define HEATER_PORT PORTD
#define HEATER_PIN PIND3

//Buzzer
#define BUZZER_PORT PORTC
#define BUZZER_PIN PINC5

//LED

#define SENSOR_FAIL_PORT PORTD //
#define SENSOR_FAIL_PIN PIND1     

#define HEATER_FAIL_PORT PORTD
#define HEATER_FAIL_PIN PIND7

#define TS_HIGH_PORT PORTD
#define TS_HIGH_PIN PIND6

#define TA_HIGH_PORT PORTD
#define TA_HIGH_PIN PIND0

#define POWER_PORT PORTD //
#define POWER_PIN PIND0

#define SET_TEMP_PORT PORTD  //
#define SET_TEMP_PIN PIND1


//BUTTONS
#define SET_UP_BUTTON_PORT  PINC
#define SET_UP_BUTTON_PIN PINC3

#define SET_BUTTON_PORT PINB
#define SET_BUTTON_PIN PINB0

#define SET_DOWN_BUTTON_PORT PIND
#define SET_DOWN_BUTTON_PIN PIND2

#define BUZZER_STOP_BUTTON_PORT PINC
#define BUZZER_STOP_BUTTON_PIN PINC4


//SPI SLAVE SELECTS
#define DISPLAY_I_PORT PORTD
#define DISPLAY_I_PIN PIND5

#define DISPLAY_II_PORT PORTD
#define DISPLAY_II_PIN PIND4

#define TEMP_I_PORT PORTB
#define TEMP_I_PIN PINB1

#define TEMP_II_PORT PORTB
#define TEMP_II_PIN PINB2




#endif /* PINS_H_ */
