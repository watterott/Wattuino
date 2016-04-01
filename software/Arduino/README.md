# Watterott Board Support Package
Board Support Package for Arduino IDE v1.6.4+


## Installation

Add the following URL to the Arduino Boards Manager (*File->Preferences*):
```
https://github.com/watterott/wattuino/raw/master/software/Arduino/package_watterott_index.json
```
and install the *Watterott Boards* via the Boards Manager (*Tools->Boards->Boards Manager*).


## Known Issues

### Windows
Sometimes the driver for the USB Bootloader (Caterina or Micronucleus) is not loaded correctly.
This is the case when *Done uploading.* is not shown after the upload process.
Check the driver state in the [Device Manager](https://github.com/watterott/wattuino/raw/master/software/Caterina/usb-devices.png), when the Bootloader is active (Caterina after the *Upload* is started and Micronucleus after a reset).
If you cannot install the driver on Windows 8 or higher then deactivate the [driver signature enforcement](https://learn.sparkfun.com/tutorials/disabling-driver-signature-on-windows-8/disabling-signed-driver-enforcement-on-windows-8).
* [Caterina Driver](https://github.com/watterott/wattuino/raw/master/software/Caterina/driver.zip)
* [Micronucleus Driver](https://github.com/watterott/wattuino/raw/master/software/Micronucleus/driver.zip) (on problems use [Zadig libusb Installer](https://github.com/micronucleus/micronucleus/tree/master/windows_driver_installer))
* [FTDI Driver](http://www.ftdichip.com/Drivers/VCP.htm)

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

Or you can also remove the Modem Manager from your system with
```
sudo apt-get uninstall modemmanager
```

### Micronucleus
The Micronucleus bootloader is not started automatically. You have to **press the reset switch to activate the bootloader** after *Uploading...* is shown in the Arduino IDE.

The Micronucleus upload tool needs root rights to run and so the Arduino IDE has to be started with sudo or you can also create a rule to allow non-root users access to the Micronucleus USB device.
For this run one of the commands - depending on your system:

```sudo nano /etc/udev/rules.d/49-micronucleus.rules```

```sudo nano /lib/udev/rules.d/49-micronucleus.rules```

and add the following lines to the file:
```
SUBSYSTEMS=="usb", ATTRS{idVendor}=="16d0", ATTRS{idProduct}=="0753", MODE:="0666"
KERNEL=="ttyACM*", ATTRS{idVendor}=="16d0", ATTRS{idProduct}=="0753", MODE:="0666", ENV{ID_MM_DEVICE_IGNORE}="1"
```


## Third party libraries/software
* [Micronucleus ATtiny USB Bootloader by Micronucleus Team](https://github.com/micronucleus/micronucleus)
* [Arduino Core for ATtiny85 by Spence Konde](https://github.com/SpenceKonde/ATTinyCore)
* [Arduino Core for ATtiny841 by Spence Konde](https://github.com/SpenceKonde/arduino-tiny-841)
