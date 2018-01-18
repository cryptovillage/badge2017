# badge2017

To add dependencies (ESP32 SDK, u8g2, MicroPython, etc.):

`git submodule update --init --recursive`

If you need to set up the ESP32 development environment,
follow step 1: https://esp-idf.readthedocs.io/en/v2.0/linux-setup.html

`cd components/micropython/micropython-esp32/mpy-cross/ && make clean && make && cd -`

To build and flash firmware:

`cd firmware/esp32`

`make` or download prebuilt firmware from: https://2017.badge.cryptovillage.org/firmware/v0.9/

`make flash` OR

`python esptool/esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 115200 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 bootloader.bin 0x10000 cpv2017.bin 0x8000 partitions.bin`

you may need to replace /dev/ttyUSB0 with /dev/ttyACM0 or whatever serial device the badge is listed as.

edit config with (includes wifi creds):

`make menuconfig`

SD Card contents can be found: https://2017.badge.cryptovillage.org/sdcard/0.3-prerel.zip

If your device is showing "TEST MODE", the EFM8 needs to be flashed. We've found that this only works from a Windows computer or Virtualbox with the Extension Pack for USB devices. If using a VM, we found that the IE11 Win7 testing VM from MicroSoft worked. You will need to install the driver from: https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers

Download the flashing tools here: https://2017.badge.cryptovillage.org/W18779AXC7/TestFlash.zip

The prebuilt firmware can be found here: https://2017.badge.cryptovillage.org/firmware/v0.9/

You need to replace esp32test.bin with cpv2017.bin, and partitions_singleapp.bin with partitions.bin in TestFlash after unzipping. (Keeping the original filenames from TestFlash.zip contents but replacing the file content)

Hold down the knob when plugging the device in to put the device into a mode to flash the EFM8. The LEDs should not turn on and the screen will be blank and dim.

Run efm8load.exe to flash the EFM8. The device should reboot, but may require replugging in the USB cable. It should now appear as a serial device.

Then run NodeMCU-PyFlasher.exe to flash the ESP32.
