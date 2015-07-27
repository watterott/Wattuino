# Watterott Board Support Package
Board Support Package for Arduino IDE >=1.6


## Automatic Installation

Add the following URL to the Arduino Boards Manager:
```
https://github.com/watterott/wattuino/raw/master/src/Arduino/package_watterott_index.json
```
and install the Watterott Boards via the Boards Manager.


## Manual Installation

Download https://github.com/watterott/wattuino/raw/master/src/Arduino/Arduino.zip and copy the directory "hardware" to your Arduino program directory.


## Known Issues

### Windows
Sometimes the driver for the USB Bootloader (Caterina or Micronucleus) is not loaded correctly.
This is the case when *Done uploading.* is not shown after the upload process.
Check the driver state in the Device Manager, when the Bootloader is active (Caterina after the *Upload* is started and Micronucleus after a reset).
On Windows 8 or higher the [driver signature enforcement](https://learn.sparkfun.com/tutorials/disabling-driver-signature-on-windows-8/disabling-signed-driver-enforcement-on-windows-8) has to be disabled for the driver installation.
* [Caterina Driver](https://github.com/watterott/wattuino/raw/master/src/Caterina/Caterina.inf)
* [Micronucleus Driver](https://github.com/watterott/wattuino/raw/master/src/Micronucleus/driver/driver.zip)
* [FTDI Drivers](http://www.ftdichip.com/Drivers/VCP.htm)

### Linux
The Modem Manager detects the Serial Ports (e.g. Caterina Bootloader) as a modem and therefore a blacklist rule is needed.
Run one of the commands - depending on your system:

```sudo nano /etc/udev/rules.d/77-mm-usb-device-blacklist.rules```
    
```sudo nano /lib/udev/rules.d/77-mm-usb-device-blacklist.rules```
    
and add the following lines to the file:

```
ATTRS{idVendor}=="6666", ENV{ID_MM_DEVICE_IGNORE}="1"
ATTRS{idVendor}=="1D50", ATTRS{idProduct}=="60B0", ENV{ID_MM_DEVICE_IGNORE}="1"
```
