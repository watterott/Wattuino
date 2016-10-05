# Optiboot

[Optiboot](https://github.com/Optiboot/optiboot) is a small serial bootloader designed for Atmel AVR microcontrollers.

This project is a modified Optiboot with auto baud rate detection specially for ATmega328 microcontrollers.

The Optiboot project is released under GPL license.
Code uploaded via the bootloader is not subject to any license issues.

**Credits**
```
Although it has evolved considerably, Optiboot builds on the original work of Jason P. Kyle (stk500boot.c), Arduino group (bootloader), Spiff (1K bootloader), AVR-Libc group and Ladyada (Adaboot).

Optiboot is the work of Peter Knight (aka Cathedrow). Despite some misattributions, it is not sponsored or supported by any organisation or company including Tinker London, Tinker.it! and Arduino.
Maintenance of Optiboot was taken over by Bill Westfield (aka WestfW) in 2011.
```

## ISP Pins of ATmega328
```
MOSI: PB3 / D11
MISO: PB4 / D12
SCK:  PB5 / D13
RST:  PC6 / Reset
```

## Fuse Settings for ATmega328
```
Extended: 0xFD
High:     0xD6
Low:      0xFF
Lockbits: 0xCF (LPM and SPM prohibited in Boot Section)
```

## AVRdude Parameters for ATmega328
```
avrdude -c avrisp2 -B 10 -p atmega328p -e -U flash:w:optiboot_m328p.hex:i -U lfuse:w:0xFF:m -U hfuse:w:0xD6:m -U efuse:w:0xFD:m -U lock:w:0xCF:m
```
