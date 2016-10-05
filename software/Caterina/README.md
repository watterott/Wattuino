# Caterina

[Caterina](https://github.com/arduino/Arduino/tree/master/hardware/arduino/avr/bootloaders/caterina) is a small USB bootloader designed for Atmel AVR microcontrollers with USB interface (ATmega32u4).
It is based on the [LUFA library](http://www.lufa-lib.org).

The Caterina project is released under MIT license.
Code uploaded via the bootloader is not subject to any license issues.

**Credits**
```
* Arduino.cc Team
* LUFA (c) Dean Camera (dean [at] fourwalledcubicle [dot] com), www.lufa-lib.org
```

## USB Vendor and Product ID
* VID: 0x1D50 - [Openmoko](http://wiki.openmoko.org/wiki/USB_Product_IDs) + PID: 0x60B0 - [Watterott](http://wiki.openmoko.org/wiki/USB_Product_IDs)
* VID 0x6666 - [Prototype Product Vendor ID](http://www.linux-usb.org/usb.ids)
* VID 0x1209 - [Open-Source Projects](http://pid.codes)

## ISP Pins of ATmega32U4
```
MOSI: PB2
MISO: PB3
SCK:  PB1
RST:  Reset
```

## Fuse Settings for ATmega32U4
```
Extended: 0xCB
High:     0xD8
Low:      0xFF
Lockbits: 0xCF (LPM and SPM prohibited in Boot Section)
```

## AVRdude Parameters for ATmega32U4
```
avrdude -c avrisp2 -B 10 -p atmega32u4 -e -U flash:w:caterina_16mhz.hex:i -U lfuse:w:0xFF:m -U hfuse:w:0xD8:m -U efuse:w:0xCB:m -U lock:w:0xCF:m
```
