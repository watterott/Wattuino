# Micronucleus

[Micronucleus](https://github.com/micronucleus/micronucleus) is a small USB bootloader designed for Atmel AVR ATtiny microcontrollers.

The Micronucleus project is released under GPL license.
Code uploaded via the bootloader is not subject to any license issues.

**Credits**
```
Firmware:
  * Micronucleus V2.x             (c) 2015 Tim Boescke
                                  (c) 2014 Shay Green
  * Original Micronucleus         (c) 2012 Jenna Fox
  * Based on USBaspLoader-tiny85  (c) 2012 Louis Beaudoin
  * Based on USBaspLoader         (c) 2007 OBJECTIVE DEVELOPMENT Software GmbH

Commandline tool:
  * Original commandline tool     (c) 2012 ihsan Kehribar
                                  (c) 2012 Jenna Fox
  * Updates for V2.x              (c) 2014 Tim Boescke

Special Thanks:
  * Aaron Stone/@sodabrew for building the OS X command line tool and various fixes.
  * Objective Development's great V-USB bitbanging usb driver
  * Embedded Creations' pioneering and inspiring USBaspLoader-tiny85
  * Digistump for motivation and contributing the VID/PID pair
  * Numerous supporters for smaller bug fixes and improvements.
```

## Bootloader Activation
```
...always    -> run Bootloader on every start
...ext_reset -> run Bootloader only after the reset switch is pressed
```

## ISP Pins of ATtiny85
```
MOSI: PB0 / D0
MISO: PB1 / D1
SCK:  PB2 / D2
RST:  PB5 / D5
```

## Fuse Settings for ATtiny85
```
Extended: 0xFE
High:     0xDD
Low:      0xE1
Lockbits: 0xFF (no memory lock)
```

## AVRdude Parameters for ATtiny85
```
avrdude -c avrisp2 -B 10 -p attiny85 -e -U flash:w:micronucleus-t85_ext_reset.hex:i -U lfuse:w:0xE1:m -U hfuse:w:0xDD:m -U efuse:w:0xFE:m
```

## ISP Pins of ATtiny841
```
MOSI: PA6 / D6
MISO: PA5 / D5
SCK:  PA4 / D4
RST:  PB3 / D11
```

## Fuse Settings for ATtiny841
```
Extended: 0xF4
High:     0xDD
Low:      0xE2
Lockbits: 0xFF (no memory lock)
```

## AVRdude Parameters for ATtiny841
```
avrdude -c avrisp2 -B 10 -p attiny841 -e -U flash:w:micronucleus-t841_ext_reset.hex:i -U lfuse:w:0xE2:m -U hfuse:w:0xDD:m -U efuse:w:0xF4:m
```
