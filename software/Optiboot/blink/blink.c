/*
 * blink.c - simple LED blinking for ATmega328 on PB5 (Arduino Pin 13)
 */

#include <inttypes.h>
#include <avr/io.h>
#if !defined(_AVR_IOXXX_H_)
# if defined(__AVR_ATmega328P__)
#  include <avr/iom328p.h>
# elif defined(__AVR_ATmega328PB__)
#  include <avr/iom328pb.h>
# endif
#endif
#include <util/delay.h>

void delay(int ms)
{
  for(; ms!=0; ms--)
  {
    _delay_ms(1);
  }
}

int main(void)
{
  DDRB = (1<<5); //bit 5 output, all others inputs
  DDRC = 0;      //all inputs
  DDRD = 0;      //all inputs

  while(1)
  {
    PORTB |= (1<<5);  //high
    delay(400);       //delay
    PORTB &= ~(1<<5); //low
    delay(400);       //delay
  }
}
