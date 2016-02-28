#include <avr/io.h>

#define	LCD_PORT	PORTD
#define KEY_COLS	3
#define KEY_ROWS	4
#define KEY_RECOG_THLD	180

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
	lcd_write4(0b00001111);

	lcd_write4(0b00000001);
	waitms(2);
}

static void line_feed(void)
{
	static unsigned char current_line = 0;

	LCD_RS(0);
	if (current_line == 0) {
		lcd_write4(0xc0);
		current_line = 1;
	} else {
		lcd_write4(0x80);
		current_line = 0;
	}
}

static void lcd_puts(char *s)
{
	static unsigned char row = 0;

	LCD_RS(1);
	while (*s) {
		lcd_write4(*s++);
		if (++row >= 16) {
			line_feed();
			row = 0;
		}
	}
}

static void sleep(void)
{
	volatile unsigned short i, j;

	for (i = 0; i < 1000; i++)
		for (j = 0; j < 65; j++);
}

static char check_key(char c)
{
	static unsigned char key_counter = 0;
	char i, recog_row = -1;

	PORTB = _BV(c);

	waitus(1);

	for (i = 0; i < KEY_ROWS; i++) {
		if (PINC & _BV(i)) {
			recog_row = i;
			key_counter++;
			break;
		}
	}
	if (recog_row < 0)
		key_counter = 0;
	else if (key_counter < KEY_RECOG_THLD)
		recog_row = -1;

	return recog_row;
}

int main(void)
{
	char recog_row;

	PORTD = 0;
	DDRD = 0xfc;

	DDRB |= _BV(0);

	lcd_init();

	while (1) {
		if ((recog_row = check_key(0)) >= 0) {
			switch (recog_row) {
			case 0:
				lcd_puts("3");
				break;
			case 1:
				lcd_puts("6");
				break;
			case 2:
				lcd_puts("9");
				break;
			case 3:
				lcd_puts("#");
				break;
			}
		}
	}

	return 0;
}
