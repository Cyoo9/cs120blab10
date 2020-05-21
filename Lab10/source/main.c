/*      Author: Taeho Yoo
 *  Partner(s) Name: 
 *      Lab Section: 23
*      Assignment: Lab 10  Exercise 3
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
enum S_States {S_Start, S_Off, S_On} S_state;
enum C_states {C_Start, combine} C_state;
enum F_states {F_Start, Neutral, Up, Down, hold1, hold2} F_state;


unsigned char threeLEDs;
unsigned char blinkingLED;
unsigned char emitSound;

unsigned long speakerPeriod = 2;

void frequencySM() {
	switch(F_state) {
		case F_Start:
			speakerPeriod = 2;
			F_state = Neutral;
			break;
		case Neutral:
			if((~PINA & 0x03) == 1) {
				F_state = hold1;
			}
			else if((~PINA & 0x03) == 2) {
				F_state = hold2;
			}
			else { F_state = Neutral; }
			break;
		case hold1:
			if((~PINA & 0x03) == 1) {
				F_state = hold1;
			}
			else { F_state = Up; }
			break;
		case hold2:
			if((~PINA & 0x03) == 2) {
				F_state = hold2;
			}
			else { F_state = Down; }
			break;
		case Up:
			F_state = Neutral;
			break;
		case Down:
			F_state = Neutral;
			break;
		default:
			break;
	}
	switch(F_state) {
		case Up:
			speakerPeriod += 4;
			break;
		case Down:
			if(speakerPeriod > 2) { speakerPeriod -= 4; }
			break;
		default:
			break;
	}
}			

void emitSoundSM() {
        switch(S_state) {
                case S_Start:
                        S_state = S_Off;
                        break;
                case S_Off:
                        if((~PINA & 0x07) == 0x04) {
                                S_state = S_On;
                        }
                        else { S_state = S_Off; }
                        break;
                case S_On:
                        S_state = S_Off;
                        break;
                default:
                        break;
        }
        switch(S_state) {
                case S_Off:
                        emitSound = 0x00;
                        break;
                case S_On:
                        emitSound = 0x10;
                        break;
                default:
                        break;
        }
}


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
			TL_state = TL_On;
			break;
		case TL_On: 
			TL_state = TL_Off;
			break;
		case TL_Off:
			TL_state = TL_On;
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
		case combine: 
			break;
		default:
			break;
	}
	switch(C_state) {
		case combine:
			PORTB = (threeLEDs | blinkingLED) | emitSound;
			break;
		default:
			break;
	}

}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;

	unsigned long BL_time = 300;
	unsigned long TL_time = 1000;
	unsigned long S_time = 2;
	const unsigned long period = 2;

	TimerSet(period);
	TimerOn();
	S_state = S_Start;
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
	    if(S_time >= speakerPeriod) {
		   // frequencySM();
		    emitSoundSM();
		    S_time = 0;
	    }
	    CombineLEDsSM();
	    frequencySM();
	while(!TimerFlag);
	TimerFlag = 0;
	S_time += period;
	TL_time += period;
	BL_time += period;
    }
    return 0;
}
