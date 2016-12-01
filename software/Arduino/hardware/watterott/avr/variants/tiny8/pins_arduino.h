/*
  pins_arduino.h - Pin definition functions for Arduino
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  Modified 28-08-2009 for attiny84 R.Wiersma
  Modified 09-10-2009 for attiny45 A.Saporetti
*/

#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <avr/pgmspace.h>

// ATMEL ATTINY85 / ARDUINO
//
//                  +-\/-+
// AIN0 (D 5) PB5  1|    |8  VCC
// AIN3 (D 3) PB3  2|    |7  PB2 (D 2) AIN1
// AIN2 (D 4) PB4  3|    |6  PB1 (D 1) PWM1
//            GND  4|    |5  PB0 (D 0) PWM0
//                  +----+

#define ATTINYX5 1       //backwards compatibility
#define __AVR_ATtinyX5__ //this is recommended way
#define USE_SOFTWARE_SPI 1
#define USE_SOFTWARE_SERIAL 1
#define INITIALIZE_ANALOG_TO_DIGITAL_CONVERTER 1
#define INITIALIZE_SECONDARY_TIMERS 1
#define TIMER_TO_USE_FOR_MILLIS 0

#ifndef TCCR1A
#define TCCR1A GTCCR
#endif
//Analog reference bit masks
// X 0 0 VCC used as Voltage Reference, disconnected from PB0 (AREF).
#ifndef DEFAULT
#define DEFAULT (0)
#endif
// X 0 1 External Voltage Reference at PB0 (AREF) pin, Internal Voltage Reference turned off.
#ifndef EXTERNAL
#define EXTERNAL (1)
#endif
// 0 1 0 Internal 1.1V Voltage Reference.
#ifndef INTERNAL
#define INTERNAL (2)
#endif
#define INTERNAL1V1 INTERNAL
// 1 1 1 Internal 2.56V Voltage Reference with external bypass capacitor at PB0 (AREF) pin(1).
#define INTERNAL2V56 (7)
// An alternative for INTERNAL2V56 is (6):
// 1 1 0 Internal 2.56V Voltage Reference without external bypass capacitor, disconnected from PB0 (AREF)(1).

#define ANALOG_COMP_DDR       DDRB
#define ANALOG_COMP_PORT      PORTB
#define ANALOG_COMP_PIN       PINB
#define ANALOG_COMP_AIN0_BIT  0
#define ANALOG_COMP_AIN1_BIT  1

#define PIN_A0         (6)
#define PIN_A1         (7)
#define PIN_A2         (8)
#define PIN_A3         (9)

static const uint8_t A0 = PIN_A0;
static const uint8_t A1 = PIN_A1;
static const uint8_t A2 = PIN_A2;
static const uint8_t A3 = PIN_A3;

#define NUM_DIGITAL_PINS            6
#define NUM_ANALOG_INPUTS           4

#define analogInputToDigitalPin(p)  ( ((p) == 0) ? 5 : (((p) == 1) ? 2 : (((p) == 2) ? 4 :(((p) == 3) ? 3 : -1))) )
#define analogPinToChannel(p)       ( (p) < 6 ? (p) : (p) - 6 )

#define digitalPinHasPWM(p)         ( (p) == 0 || (p) == 1 || (p) == 4 )

#define digitalPinToPCICR(p)        ( ((p) >= 0 && (p) <= 5) ? (&GIMSK) : ((uint8_t *)NULL) )
#define digitalPinToPCICRbit(p)     ( PCIE )
#define digitalPinToPCMSK(p)        ( ((p) >= 0 && (p) <= 5) ? (&PCMSK) : ((uint8_t *)NULL) )
#define digitalPinToPCMSKbit(p)     ( p )

#define digitalPinToInterrupt(p)    ( ((p) == 2) ? 0 : NOT_AN_INTERRUPT )

#ifdef ARDUINO_MAIN

// these arrays map port names (e.g. port B) to the
// appropriate addresses for various functions (e.g. reading
// and writing) tiny45 only port B 
const uint16_t PROGMEM port_to_mode_PGM[] = {
	NOT_A_PORT,
	NOT_A_PORT,
	(uint16_t) &DDRB,
};

const uint16_t PROGMEM port_to_output_PGM[] = {
	NOT_A_PORT,
	NOT_A_PORT,
	(uint16_t) &PORTB,
};

const uint16_t PROGMEM port_to_input_PGM[] = {
	NOT_A_PIN,
	NOT_A_PIN,
	(uint16_t) &PINB,
};

const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
	PB, /* 0 */
	PB,
	PB,
	PB,
	PB, 
	PB, // 5
	PB, // A0
	PB,
	PB,
	PB, // A4

};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
	_BV(0), /* 0, port B */
	_BV(1),
	_BV(2),
	_BV(3), /* 3 port B */
	_BV(4),
	_BV(5),
	_BV(5),
	_BV(2),
	_BV(4),
	_BV(3),
};

const uint8_t PROGMEM digital_pin_to_timer_PGM[] = {
	TIMER0A, /* OC0A */
	TIMER0B,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	TIMER1B,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
	NOT_ON_TIMER,
};

#endif //ARDUINO_MAIN

#endif //Pins_Arduino_h
