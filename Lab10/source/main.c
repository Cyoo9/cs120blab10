/*      Author: Taeho Yoo
 *  Partner(s) Name: 
 *      Lab Section: 23
*      Assignment: Lab 10  Exercise 2
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() { TCCR1B = 0x00; }
void TimerISR() { TimerFlag = 1; }

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
enum State {Start, bit0, bit1, bit2, bit3, Off, On, combine} state;
unsigned char threeLEDs;
unsigned char blinkingLED;

void ThreeLEDsSM() {
	switch(state) {
		case Start:
			state = bit0;
			break;
		case bit0:
			state = bit1;
			break;
		case bit1:
			state = bit2;
			break;
		case bit2:
			state = bit3;
			break;
		default:
			break;
	}
	switch(state) {
		case bit0:
			threeLEDs = 0x01;
			break;
		case bit1:
			threeLEDs = 0x02;
			break;
		case bit2:
			threeLEDs = 0x04;
			break;
		case bit3:
			threeLEDs = 0x08;
			break;
		default:
			break;
	}
}

void BlinkingLEDSM() {
	switch(state) {
		case Start:
			state = On;
			break;
		case On: 
			state = Off;
			break;
		case Off:
			state = On;
			break;
		default:
			break;
	}
	switch(state) {
		case On:
			blinkingLED  = 0x08;
			break;
		case Off:
			blinkingLED  = 0x00;
			break;
		default:
			break;
	}
}

void CombineLEDsSM() {
	switch(state) {
		case Start:
			state = combine;
			break;
		case combine:
			state = combine; //might not need this
			break;
	}
	switch(state) {
		case combine:
			PORTB = threeLEDs + blinkingLED;
	}

}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0x00; PORTB = 0xFF;
	TimerSet(1000);
	TimerOn();

    /* Insert your solution below */
    while (1) {
	ThreeLEDsSM();
	BlinkingLEDSM();
	CombineLEDsSM();

	while(!TimerFlag);
	TimerFlag = 0;
    }
    return 0;
}
