#include <avr/io.h>

#define	LCD_PORT	PORTD

#define	LCD_E(b) {			\
	if(b) LCD_PORT |= _BV(3);	\
	else LCD_PORT &= ~_BV(3);	\
}

#define	LCD_RS(b) {			\
	if(b) LCD_PORT |= _BV(2);	\
	else LCD_PORT &= ~_BV(2);	\
}

static void waitus(unsigned int _n)
{
	/* volatile unsigned int n = _n * 10; */
	volatile unsigned int n = _n * 20;
	while (n-- > 0);
}

static void waitms(unsigned int _n)
{
	/* volatile unsigned int n = _n * 1500; */
	volatile unsigned int n = _n * 20000;
	while (n-- > 0);
}

static void lcd_write8(unsigned char ch)
{
	LCD_PORT = (LCD_PORT & 0x0f) | (ch & 0xf0);

	LCD_E(1);
	waitus(1);
	LCD_E(0);
	waitus(100);
}

static void lcd_write4(unsigned char ch)
{
	lcd_write8(ch);
	lcd_write8(ch << 4);
}

static void lcd_init(void)
{
	LCD_RS(0);
	LCD_E(0);
	waitms(100);

	lcd_write8(0b00110000);
	waitms(5);

	lcd_write8(0b00110000);
	waitus(100);

	lcd_write8(0b00110000);

	lcd_write8(0b00100000);

	lcd_write4(0b00101000);

	lcd_write4(0b00000110);
	lcd_write4(0b00001100);

	lcd_write4(0b00000001);
	waitms(2);
}

static void lcd_puts(char *s)
{
	LCD_RS(1);
	while (*s)
		lcd_write4(*s++);
}

static void sleep(void)
{
	volatile unsigned short i, j;

	for (i = 0; i < 1000; i++)
		for (j = 0; j < 65; j++);
}

int main(void)
{
	volatile unsigned short i;
	char str[16];

	PORTD = 0;
	DDRD = 0xfc;

	lcd_init();

	str[0] = '0';
	str[1] = '\0';
	while (1) {
		lcd_puts(str);
		str[0]++;

		for (i = 0; i < 5; i++)
			sleep();
	}

	return 0;
}
