CC=/usr/bin/avr-gcc
MEGA=168
CFLAGS=-g -Os -Wall -mcall-prologues -mmcu=atmega$(MEGA)
OBJ2HEX=/usr/bin/avr-objcopy 
PROG=/usr/bin/avrdude
PRGMMR=usbasp
TARGET=spi_slave

FUSE=-U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xf9:m 


program : $(TARGET).hex
	$(PROG) -c $(PRGMMR) -p m$(MEGA) -P usb -U flash:w:$(TARGET).hex

program_fuses :
	$(PROG) -c $(PRGMMR) -p m$(MEGA) -P usb -e $(FUSE)

%.obj : %.o
	$(CC) $(CFLAGS) $< -o $@

%.hex : %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

clean :
	rm -f *.hex *.obj *.o
