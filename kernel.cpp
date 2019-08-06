#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__linux__)
#error "must use a cross compiler"
#endif
 
#if !defined(__i386__)
#error "must be compiled with ix86 architecture"
#endif
 
enum VgaColour {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};
 
static inline uint8_t CreateVgaColour(enum VgaColour fg, enum VgaColour bg) 
{
	return fg | bg << 4;
}
 
static inline uint16_t VgaEntry(unsigned char uc, uint8_t colour) 
{
	return (uint16_t) uc | (uint16_t) colour << 8;
}
 
size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
 
void InitTerminal(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = CreateVgaColour(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLUE);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = VgaEntry(' ', terminal_color);
		}
	}
}

void PutEntryAt(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = VgaEntry(c, color);
}
 
void PutChar(char c) 
{
	if(c == '\n')
	{
		terminal_row++;
		terminal_column = 0;
		return;
	}

	PutEntryAt(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}
}
 
void Write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		PutChar(data[i]);
}
 
void WriteString(const char* data) 
{
	Write(data, strlen(data));
}
 
extern "C" void kernel_main(void) 
{
	InitTerminal();
 
	WriteString("Hello, kernel World!\nThis should be at the start of the second line");
}
