# badge2017

To add ESP32 SDK:

`git submodule update --init --recursive`

I don't think this is needed because of --recursive:

`cd firmware/esp-idf && git submodule init && git submodule update && cd -`

follow step 1: https://esp-idf.readthedocs.io/en/v2.0/linux-setup.html

`cd components/micropython/micropython-esp32/mpy-cross/ && make clean && make && cd -`

To build and flash firmware:

`cd firmware/esp32`

`make`

`make flash`

edit config with (includes wifi creds):

`make menuconfig`
