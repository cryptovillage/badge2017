# badge2017

To add dependencies (ESP32 SDK, u8g2, MicroPython, etc.):

`git submodule update --init --recursive`

If you need to set up the ESP32 development environment,
follow step 1: https://esp-idf.readthedocs.io/en/v2.0/linux-setup.html

`cd components/micropython/micropython-esp32/mpy-cross/ && make clean && make && cd -`

To build and flash firmware:

`cd firmware/esp32`

`make`

`make flash`

edit config with (includes wifi creds):

`make menuconfig`
