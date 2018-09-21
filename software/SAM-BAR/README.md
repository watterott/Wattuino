# SAM-BAR
SAM-BAR (**S**mart **A**tmel **M**icrocontroller **B**oot **A**ssistant **R**eloaded) is a Combo USB CDC+MSD Bootloader for Atmel/Microchip SAM D21 devices (ARM Cortex-M0+).
It is based on the following bootloaders:
* [SAM-BA (AppNote AT07175) by Atmel/Microchip (www.atmel.com)](http://www.atmel.com/images/Atmel-42366-SAM-BA-Bootloader-for-SAM-D21_ApplicationNote_AT07175.zip)
* [Arduino Zero Bootloader by Arduino LLC (www.arduino.cc)](https://github.com/arduino/ArduinoCore-samd/tree/master/bootloaders/zero)
* [SAMD-MSD-Bootloader by Justin Mattair (www.mattairtech.com)](https://github.com/mattairtech/SAMD-MSD-Bootloader)


## Features
* SAM-BA and Arduino Zero compatible Bootloader (USB CDC/VCP)
* Mass-Storage-Device Bootloader (USB MSD/MSC)
* Fits in 16KB (user program start at 0x4000)


## Programming
The bootloader can be flashed with an ARM SWD (Serial Wire Debug) programmer.
To protect the bootloader flash section the fuse ```NVMCTRL_BOOTPROT``` has to be set to ```0x01```.


## Activation
The bootloader will be activated if no user program exists or by doing two reset in 0.5s (double pressing reset switch).
It can also be activated via an Arduino Sketch, when opening the serial port with 1200bps. This will erase the user program and start the bootloader.


## Usage
When the bootloader is active then a new program can be loaded via the serial port (SAM-BA protocol) or via the mass storage device.
For the mass storage device delete the file ```FLASH.BIN``` and then copy a new binary file ```FLASH.BIN``` to the drive.
After the copy process is finished, do a reset to start the new program.

On Linux/Mac the dd command can be used to write the firmware: ```dd if=NEW.BIN of=/media/BOOTLOADER/FLASH.BIN conv=notrunc```

A binary file can be generated and exported in the Arduino IDE with *Sketch->Export compiled Binary* command.


## Known Issues

### Windows
Sometimes the driver for the USB Bootloader is not loaded correctly.
Check the driver state in the Device Manager, when the Bootloader is active (double press reset).

### Linux
The Modem Manager can cause problems, because it detects Serial Ports (e.g. of the USB Bootloader) as a modem.
So we recommend to uninstall/remove the Modem Manager:
```
sudo apt-get uninstall modemmanager
```
As alternative it is also possible to add a backlist rule for the USB device to the file ```77-mm-usb-device-blacklist.rules```.
