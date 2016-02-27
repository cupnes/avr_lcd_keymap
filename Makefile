CFLAGS	=	-mmcu=atmega168
CFLAGS	+=	-Wall -Wextra

.c.o:
	avr-gcc $(CFLAGS) -o $@ $<

lcd_keymap.hex: lcd_keymap.o
	avr-objcopy -O ihex $< $@

program: lcd_keymap.hex
	sudo avrdude -p m168 -c avrisp2 -U $<

clean:
	rm -f *~ *.o *.hex

.PHONY: program clean
