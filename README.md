# Wattuino
Arduino compatible boards and modules.

Shop:
[Wattuino Pro Mini 5V 16MHz](http://www.watterott.com/en/Wattuino-pro-mini-5V-16MHz), 
[Wattuino Pro Mini 3V3 8MHz](http://www.watterott.com/en/Wattuino-pro-mini-3V3-8MHz)

CAD Software Parts:
[Eagle](https://raw.github.com/watterott/wattuino/master/pcb/wattuino.lbr), 
[Fritzing](https://raw.github.com/watterott/wattuino/master/pcb/wattuino.fzpz)


## Wattuino Pro Mini Features
![Wattuino Pro Mini](https://raw.github.com/watterott/wattuino/master/pcb/Wattuino-Pro-Mini_v10.jpg)
* Atmel AVR ATmega328 @ 16 MHz or 8 MHz (external resonator)
* Compatible with Arduino Pro Mini
* Arduino Bootloader (optiboot) with auto baud rate detection
* LC filter on AVCC of the AVR microcontroller
* All pins of the AVR are available (also ADC6, ADC7)
* 5V or 3.3V LDO voltage regulator with over current protection (TS5204)


## Wattuino Nanite 85 Features
![Wattuino Nanite85](https://raw.github.com/watterott/wattuino/master/pcb/Wattuino-Nanite85_v10.jpg)
* Atmel AVR ATtiny85 (internal clock)
* Pin-compatible with 8-Pin DIP ATtiny85
* Based on the original [Nanite 85](https://github.com/cpldcpu/Nanite) design by [Tim Böscke](https://github.com/cpldcpu)
* [Micronucleus](https://github.com/micronucleus/micronucleus) USB Bootloader
* MicroUSB connector


## Hardware and Software
* [Schematics + Layout](https://github.com/watterott/wattuino/tree/master/pcb)
* [Arduino Bootloader and Extensions](https://github.com/watterott/wattuino/tree/master/src)
