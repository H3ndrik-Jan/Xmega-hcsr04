/*
* hcsr04.c
*
* Created: 5-4-2020 14:31:09
*  Author: Hendrik-Jan
*
*	Description:
*	This code can be used to measure distance from an object
*	using an HC-SR04 ultrasonic sensor. It works by sending a
*	11us pulse to the trigger pin. This will start a measurement,
*	and when the measurement is done, the echo pin will turn high
*	for a time that can be mapped to a distance in centimeters.
*
*	Connect the trigger-pin of the sensor to pin 0 of port D
*	Connect the echo-pin to pin 2 of port E via a voltage divider,
*	to break down the 5volts to around 3 volts.
*/
#define  F_CPU   32000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include "hcsr04.h"

volatile uint8_t  newCapture = 0;
volatile uint16_t period;
volatile uint16_t pulseWidth;
volatile uint8_t  numberOfOverflows = 0;               // Integer for number of overflows

ISR(TCE0_OVF_vect)                                     // Interrupt routine for overflow
{
	(numberOfOverflows<10) ? (numberOfOverflows++) : (newCapture = true);  // Increment number of overflows and add timeout
}

ISR(TCE0_CCA_vect)
{
	uint16_t captureValue = TCE0.CCA;

	if ( captureValue & 0x8000 ) {
		TCE0.CTRLFSET = TC_CMD_RESTART_gc;
		period = (captureValue & 0x7FFF) + (0x8000UL * numberOfOverflows);  // With correction
		numberOfOverflows = 0;                                              // Reset overflow number
		newCapture = 1;
		}  else {
		pulseWidth = captureValue + (0x8000UL * numberOfOverflows);         // With correction
	}
}

void init_ulsosensor(void)
{
	PORTE.PIN2CTRL = PORT_ISC_BOTHEDGES_gc;	//	Sensor echo pin
	PORTE.DIRCLR   = PIN2_bm;
	EVSYS.CH0MUX   = EVSYS_CHMUX_PORTE_PIN2_gc;
	TCE0.CTRLD     = TC_EVACT_CAPT_gc | TC_EVSEL_CH0_gc;
	TCE0.CTRLB     = TC0_CCAEN_bm | TC_WGMODE_NORMAL_gc;
	TCE0.CTRLA     = TC_CLKSEL_DIV64_gc;
	TCE0.INTCTRLB  = TC_CCAINTLVL_LO_gc;
	TCE0.INTCTRLA  = TC_OVFINTLVL_LO_gc;
	TCE0.PER       = 0x7FFF;
	
	PORTD.DIRSET = PIN0_bm;	//	Sensor trigger pin
	PORTD.OUTCLR = PIN0_bm;
}

uint16_t getDist(void)
{
	uint16_t cm = 0;
	PORTD.OUTSET = PIN0_bm;
	_delay_us(PULSE_US);
	PORTD.OUTCLR = PIN0_bm;
	while (! newCapture);				//	Wait for the response from the sensor
	cm = pulseWidth*(double)0.0344824;	//	Convert to cm
	newCapture = 0;
	return cm;
}
