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
  Modified for attiny841 A.Watterott
*/

#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <avr/pgmspace.h>

// ATMEL ATTINY841 / ARDUINO
//
//                     +-\/-+
//               VCC  1|    |14  GND
//       (D 10)  PB0  2|    |13  PA0  (D  0)  AREF
//  INT0 (D  9)  PB1  3|    |12  PA1  (D  1)
//  RST  (D 11)  PB3  4|    |11  PA2  (D  2)
//  PWM  (D  8)  PB2  5|    |10  PA3  (D  3)
//  PWM  (D  7)  PA7  6|    |9   PA4  (D  4)
//  PWM  (D  6)  PA6  7|    |8   PA5  (D  5)  PWM
//                     +----+

#define PIN_A0         (0)
#define PIN_A1         (1)
#define PIN_A2         (2)
#define PIN_A3         (3)
#define PIN_A4         (4)
#define PIN_A5         (5)
#define PIN_A6         (6)
#define PIN_A7         (7)

static const uint8_t A0 = PIN_A0;
static const uint8_t A1 = PIN_A1;
static const uint8_t A2 = PIN_A2;
static const uint8_t A3 = PIN_A3;
static const uint8_t A4 = PIN_A4;
static const uint8_t A5 = PIN_A5;
static const uint8_t A6 = PIN_A6;
static const uint8_t A7 = PIN_A7;

#if defined(__AVR_ATtinyX41__) || defined(__AVR_ATtiny441__) || defined(__AVR_ATtiny841__)
#define PIN_SPI_SS     (7)
#define PIN_SPI_MOSI   (6)
#define PIN_SPI_MISO   (5)
#define PIN_SPI_SCK    (4)

static const uint8_t SS   = PIN_SPI_SS;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;

#define PIN_WIRE_SDA   (6)
#define PIN_WIRE_SCL   (4)

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;

#define LED_BUILTIN    (8)

#define NUM_ANALOG_INPUTS           11

#else
#define NUM_ANALOG_INPUTS           7
#endif

#define NUM_DIGITAL_PINS            11

#define digitalPinToPCICR(p)        ( ((p) >= 0 && (p) <= 10) ? (&GIMSK) : ((uint8_t *)0) )
#define digitalPinToPCICRbit(p)     ( ((p) <= 7) ? PCIE0 : PCIE1 )
#define digitalPinToPCMSK(p)        ( ((p) <= 7) ? (&PCMSK0) : (((p) <= 10) ? (&PCMSK1) : ((uint8_t *)0)) )
#define digitalPinToPCMSKbit(p)     ( ((p) <= 7) ? (p) : (10 - (p)) )

#define digitalPinToInterrupt(p)    ( ((p) == 9) ? 0 : NOT_AN_INTERRUPT)

#ifdef ARDUINO_MAIN

// these arrays map port names (e.g. port B) to the
// appropriate addresses for various functions (e.g. reading
// and writing)
const uint16_t PROGMEM port_to_mode_PGM[] = 
{
  NOT_A_PORT,
  (uint16_t)&DDRA,
  (uint16_t)&DDRB,
};

const uint16_t PROGMEM port_to_output_PGM[] = 
{
  NOT_A_PORT,
  (uint16_t)&PORTA,
  (uint16_t)&PORTB,
};

const uint16_t PROGMEM port_to_input_PGM[] = 
{
  NOT_A_PORT,
  (uint16_t)&PINA,
  (uint16_t)&PINB,
};

const uint8_t PROGMEM digital_pin_to_port_PGM[] = 
{
  PA, /* 0 */
  PA,
  PA,
  PA,
  PA,
  PA,
  PA,
  PA,
  PB, /* 8 */
  PB,
  PB,
  PB,
};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = 
{
  _BV(0), /* port A */
  _BV(1),
  _BV(2),
  _BV(3),
  _BV(4),
  _BV(5), 
  _BV(6),
  _BV(7),
  _BV(2), /* port B */
  _BV(1),
  _BV(0),
  _BV(3),
};

#if defined(__AVR_ATtinyX41__) || defined(__AVR_ATtiny441__) || defined(__AVR_ATtiny841__)
const uint8_t PROGMEM digital_pin_to_timer_PGM[] = 
{
  NOT_ON_TIMER,
  NOT_ON_TIMER, /* TOCC0 */
  NOT_ON_TIMER, /* TOCC1 */
  NOT_ON_TIMER, /* TOCC2 */
  NOT_ON_TIMER, /* TOCC3 */
  NOT_ON_TIMER, /* TOCC4 */
  NOT_ON_TIMER, /* TOCC5 */
  NOT_ON_TIMER, /* TOCC6 */
  NOT_ON_TIMER, /* TOCC7 */
  NOT_ON_TIMER,
  NOT_ON_TIMER,
  NOT_ON_TIMER,
};

#else
const uint8_t PROGMEM digital_pin_to_timer_PGM[] = 
{
  NOT_ON_TIMER,
  NOT_ON_TIMER,
  NOT_ON_TIMER,
  NOT_ON_TIMER, 
  NOT_ON_TIMER,
  TIMER1B, /* OC1B */
  TIMER1A, /* OC1A */
  TIMER0B, /* OC0B */
  TIMER0A, /* OC0A */
  NOT_ON_TIMER,
  NOT_ON_TIMER,
  NOT_ON_TIMER,
};
#endif

#endif //ARDUINO_MAIN

#endif //Pins_Arduino_h
