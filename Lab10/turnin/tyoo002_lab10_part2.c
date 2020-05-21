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
enum BL_states {BL_Start, bit0, bit1, bit2} BL_state;
enum TL_states {TL_Start, TL_Off, TL_On} TL_state;
enum C_states {C_Start, combine} C_state;
unsigned char threeLEDs;
unsigned char blinkingLED;

void ThreeLEDsSM() {
	switch(BL_state) {
		case BL_Start:
			BL_state = bit0;
			break;
		case bit0:
			BL_state = bit1;
			break;
		case bit1:
			BL_state = bit2;
			break;
		case bit2:
			BL_state = bit0;
			break;
		default:
			break;
	}
	switch(BL_state) {
		case bit0:
			threeLEDs = 0x01;
			break;
		case bit1:
			threeLEDs = 0x02;
			break;
		case bit2:
			threeLEDs = 0x04;
			break;
		default:
			break;
	}
}

void BlinkingLEDSM() {
	switch(TL_state) {
		case TL_Start:
			TL_state = TL_Off;
			break;
		case TL_Off:
			TL_state = TL_On;
			break;
		case TL_On:
			TL_state = TL_Off;
			break;
		default:
			break;
	}
	switch(TL_state) {
		case TL_On:
			blinkingLED  = 0x08;
			break;
		case TL_Off:
			blinkingLED  = 0x00;
			break;
		default:
			break;
	}
}

void CombineLEDsSM() {
	switch(C_state) {
		case C_Start:
			C_state = combine;
			break;
		case combine: //might not need this
			break;
		default:
			break;
	}
	switch(C_state) {
		case combine:
			PORTB = threeLEDs | blinkingLED;
			break;
		default:
			break;
	}

}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF; PORTB = 0x00;
	unsigned long BL_time = 300;
	unsigned long TL_time = 1000;
	const unsigned long period = 100;
	TimerSet(period);
	TimerOn();
	BL_state = BL_Start;
	TL_state = TL_Start;

    /* Insert your solution below */
    while (1) {
	    if(TL_time >= 1000) {
		    BlinkingLEDSM();
		    TL_time = 0;
	    }
	    if(BL_time >= 300) {
		    ThreeLEDsSM();
		    BL_time = 0;
	    }
	    CombineLEDsSM();
	while(!TimerFlag);
	TimerFlag = 0;
	TL_time += period;
	BL_time += period;
    }
    return 0;
}
