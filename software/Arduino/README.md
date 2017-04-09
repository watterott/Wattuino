# Watterott Board Support Package
Board Support Package for [Arduino IDE v1.6+](https://www.arduino.cc/en/Main/Software)


## Installation

Add the following URL to the Arduino Boards Manager (*File->Preferences*):
```
https://github.com/watterott/wattuino/raw/master/software/Arduino/package_watterott_index.json
```
and install the **Watterott AVR Boards**, **Watterott SAMD Boards** and **Arduino SAMD Boards** via the Boards Manager (*Tools->Boards->Boards Manager*).

If you want to use the **ATTinyCore** from [Spence Konde](https://github.com/SpenceKonde/ATTinyCore) for the Nanite boards, then also install it via the Arduino Boards Manager.
Here is the URL:
```
http://drazzy.com/package_drazzy.com_index.json
```


## Known Issues

### Windows
Sometimes the driver for the USB Bootloader (Caterina or Micronucleus) is not loaded correctly.
This is the case when *Done uploading.* is not shown after the upload process.
Check the driver state in the [Device Manager](https://github.com/watterott/wattuino/raw/master/software/Caterina/usb-devices.png), when the Bootloader is active (Caterina after the *Upload* is started and Micronucleus after a reset).
If you cannot install the driver on Windows 8 or higher then deactivate the [driver signature enforcement](https://learn.sparkfun.com/tutorials/disabling-driver-signature-on-windows-8/disabling-signed-driver-enforcement-on-windows-8).
* [Caterina Driver](https://github.com/watterott/wattuino/raw/master/software/Caterina/driver.zip)
* [Micronucleus Driver](https://github.com/watterott/wattuino/raw/master/software/Micronucleus/driver.zip) (on problems use [Zadig](https://github.com/micronucleus/micronucleus/tree/master/windows_driver_installer))
* [FTDI Driver](http://www.ftdichip.com/Drivers/VCP.htm)
* [SAM-BAR Driver](https://github.com/watterott/SAM-BAR/raw/master/software/arduino/driver.zip)

### Linux
The Modem Manager detects the Serial Ports (e.g. Caterina Bootloader) as a modem and therefore a blacklist rule is needed.
Run one of the commands - depending on your system:

```sudo nano /etc/udev/rules.d/77-mm-usb-device-blacklist.rules```

```sudo nano /lib/udev/rules.d/77-mm-usb-device-blacklist.rules```

and add the following lines to the file:
```
ATTRS{idVendor}=="6666", ENV{ID_MM_DEVICE_IGNORE}="1"
ATTRS{idVendor}=="1D50", ATTRS{idProduct}=="60B0", ENV{ID_MM_DEVICE_IGNORE}="1"
ATTRS{idVendor}=="1D50", ATTRS{idProduct}=="6112", ENV{ID_MM_DEVICE_IGNORE}="1"
ATTRS{idVendor}=="1D50", ATTRS{idProduct}=="6113", ENV{ID_MM_DEVICE_IGNORE}="1"
```

Reload udev rules:
```
sudo udevadm control --reload-rules
```

Or you can also remove the Modem Manager from your system with
```
sudo apt-get uninstall modemmanager
```

### Micronucleus Bootloader
The Micronucleus bootloader is not started automatically. You have to **press the reset switch to activate the bootloader** after *Uploading...* is shown in the Arduino IDE.

The Micronucleus upload tool needs root rights to run and so the Arduino IDE has to be started with sudo or you can also create a rule to allow non-root users access to the Micronucleus USB device.
For this run one of the commands - depending on your system:

```sudo nano /etc/udev/rules.d/49-micronucleus.rules```

```sudo nano /lib/udev/rules.d/49-micronucleus.rules```

and add the following lines to the file:
```
SUBSYSTEMS=="usb", ATTRS{idVendor}=="16d0", ATTRS{idProduct}=="0753", MODE:="0666", ENV{MTP_NO_PROBE}="1"
KERNEL=="ttyACM*", ATTRS{idVendor}=="16d0", ATTRS{idProduct}=="0753", MODE:="0666", ENV{ID_MM_DEVICE_IGNORE}="1"
```

Reload udev rules:
```
sudo udevadm control --reload-rules
```

## SAM-BAR Bootloader

The bootloader can be activated via the Arduino IDE or by doing two reset in 0.5s (double pressing reset switch).

When the bootloader is active then a new program can be loaded via the Arduino IDE and the serial port or via the mass storage device.
For the mass storage device delete the file ```FLASH.BIN``` and then copy a new binary file ```FLASH.BIN``` to the drive.
After the copy process is finished, do a reset to start the new program.

A binary file can be generated and exported in the Arduino IDE with *Sketch->Export compiled Binary* command.
