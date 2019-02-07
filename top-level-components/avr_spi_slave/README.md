# AVR SPI Slave
*Copyright (c) 2019 Warren Taylor.*

## Disclaimer
The following documentation in no way guarantees a secure system.
*See [LICENSE](../../LICENSE).*

## Overview
The original plan was to connect peripherals directly to the ESP8266 data pins but I ran out of pins very quickly. Since I had an unused ATmega168 lying around I though "what the heck" and connected the two chips via SPI with the ESP8266 as the master and the ATmega as the slave. In additions to providing a larger number of data pins this scheme also provided 5v tolerance.

This AVR SPI Slave simply receives a byte value from the controlling ESP8266 and then sets the output of Port D to this by value. In other words the ESP8266 is setting the eight Port D output pins via SPI. That's all.

However, a huge advantage for the future is that with a bit more programming in the ESP8266 the AVR can be programmed Over-the-Air, which means it can be made to do anything.

This in not an Arduino Sketch.
This is very simple native AVR firmware.
Why? Because I didn't know how easy native AVR programming was until I tried it.

## Requirements
* ATmega168 (or similar)
* avr-gcc
* AVR ISP (In-System Programming) device

## Wiring up the AVR
I’m going to defer to two other web sites that do an excellent job of describing how to wire-up and program the AVR.
* <http://ediy.com.my/index.php/projects/item/86-minimal-arduino-with-8mhz-internal-clock> 
* <http://www.ladyada.net/learn/avr/index.html> 

## Programming the AVR
Connect you development computer to the ATmega168 as described above.
Then, from this "avr_spi_slave" directory, run:
```bash
make program_fuses
make program
```
Note: "make program_fuses" only needs to be run once, not each time the source file changes.

## AVR References
* <http://ediy.com.my/index.php/projects/item/86-minimal-arduino-with-8mhz-internal-clock>
* <http://www.ladyada.net/learn/avr/index.html>
* <https://en.wikipedia.org/wiki/AVR_microcontrollers>
* <https://www.arduino.cc/en/Tutorial/ArduinoISP>
* <https://www.arduino.cc/en/Main.ArduinoISP>
* <http://maxembedded.com/2015/06/setting-up-avr-gcc-toolchain-on-linux-and-mac-os-x/>
* <https://www.microchip.com/mplab/avr-support/avr-and-arm-toolchains-c-compilers>
* <https://gcc.gnu.org/wiki/avr-gcc>
* <https://www.nongnu.org/avr-libc/>
* <http://www.engbedded.com/fusecalc/>
