# Wattuino
Arduino/Genuino compatible Boards and Modules.


## Wattuino Uno
[![Wattuino Uno](https://github.com/watterott/wattuino/raw/master/hardware/Wattuino-Uno_v10.jpg)](http://www.watterott.com/en/Wattuino-UNO)
* Shop: [Wattuino Uno](http://www.watterott.com/en/Wattuino-UNO), [Wattuino Uno Kit](http://www.watterott.com/en/Wattuino-Uno-Kit)
* Compatible with Uno R3 (Arduino IDE)
* Atmel AVR ATmega328 @ 16 MHz (external resonator)
* Arduino compatible Bootloader (optiboot) with auto baud rate detection
* LC filter on AVCC of the AVR microcontroller
* FTDI FT231X USB UART Bridge
* USB-B or MicroUSB connector for programming/communication
* 5V and 3.3V LDO voltage regulators
* PCB Size: 68.58mm x 53.34mm


## Wattuino Pro Mini
[![Wattuino Pro Mini](https://github.com/watterott/wattuino/raw/master/hardware/Wattuino-Pro-Mini_v10.jpg)](http://www.watterott.com/en/Wattuino-pro-mini-5V-16MHz)
* Shop [Wattuino Pro Mini 5V 16MHz](http://www.watterott.com/en/Wattuino-pro-mini-5V-16MHz), [Wattuino Pro Mini 3V3 8MHz](http://www.watterott.com/en/Wattuino-pro-mini-3V3-8MHz)
* Compatible with Pro Mini (Arduino IDE)
* Atmel AVR ATmega328 @ 16 MHz or 8 MHz (external resonator)
* Arduino compatible Bootloader (optiboot) with auto baud rate detection
* LC filter on AVCC of the AVR microcontroller
* All pins of the AVR are available (also ADC6, ADC7)
* 5V or 3.3V LDO voltage regulator with over current protection
* FTDI connector for programming/communication
* PCB Size: 33mm x 18mm


## Wattuino Nanite 841
[![Wattuino Nanite841](https://github.com/watterott/wattuino/raw/master/hardware/Wattuino-Nanite841_v10.jpg)](http://www.watterott.com/en/Wattuino-Nanite841)
* Shop: [Wattuino Nanite 841](http://www.watterott.com/en/Wattuino-Nanite841)
* Compatible with [Arduino IDE](https://github.com/watterott/wattuino/tree/master/software/Arduino#watterott-board-support-package) (Bootloader started via Reset-Switch)
* Atmel AVR ATtiny841 @ 8 MHz (internal clock) with USART, SPI, I2C Slave, ADC, EEPROM
* 14-Pin DIP and pin-compatible with 14-Pin SOIC ATtiny841
* Designed by [Tim Böscke](https://github.com/cpldcpu)
* [Micronucleus](https://github.com/micronucleus/micronucleus) USB Bootloader ([Windows Driver](https://github.com/watterott/wattuino/raw/master/software/Micronucleus/driver.zip))
* MicroUSB connector for programming/communication
* PCB Size: 24.13mm x 10.16mm


## Wattuino Nanite 85
[![Wattuino Nanite85](https://github.com/watterott/wattuino/raw/master/hardware/Wattuino-Nanite85_v10.jpg)](http://www.watterott.com/en/Wattuino-Nanite85)
* Shop: [Wattuino Nanite 85](http://www.watterott.com/en/Wattuino-Nanite85)
* Compatible with [Arduino IDE](https://github.com/watterott/wattuino/tree/master/software/Arduino#watterott-board-support-package) (Bootloader started via Reset-Switch)
* Atmel AVR ATtiny85 @ 16.5 MHz (internal clock) with USI, ADC, EEPROM
* Pin-compatible with 8-Pin DIP ATtiny85
* Based on the original [Nanite 85](https://github.com/cpldcpu/Nanite) design by [Tim Böscke](https://github.com/cpldcpu)
* [Micronucleus](https://github.com/micronucleus/micronucleus) USB Bootloader ([Windows Driver](https://github.com/watterott/wattuino/raw/master/software/Micronucleus/driver.zip))
* MicroUSB connector for programming/communication
* PCB Size: 17.46mm x 10.16mm


## Hardware and Software
* [Schematics + Layout](https://github.com/watterott/wattuino/tree/master/hardware)
* [Board Support Package for Arduino IDE](https://github.com/watterott/wattuino/tree/master/software/Arduino#watterott-board-support-package)
* Drivers:
  [FTDI FT23x](http://www.ftdichip.com/Drivers/VCP.htm),
  [Caterina Atmega32u4](https://github.com/watterott/wattuino/raw/master/software/Caterina/driver.zip),
  [Micronucleus Attiny](https://github.com/watterott/wattuino/raw/master/software/Micronucleus/driver.zip)
  (on problems use [Zadig libusb Installer](https://github.com/micronucleus/micronucleus/tree/master/windows_driver_installer))
* CAD Parts:
  [Eagle](https://github.com/watterott/Eagle-Libs),
  [Fritzing](https://github.com/watterott/wattuino/raw/master/hardware/wattuino.fzpz)
