#include "./io.h"

// Track the current cursor's row and column
volatile int cursorCol = 0;
volatile int cursorRow = 0;

unsigned char keymap[128];

// C version of assembly I/O port instructions
// Allows for reading and writing with I/O
void outb(uint16 port, uint8 value)
{
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

uint8 inb(uint16 port)
{
   uint8 ret;
   asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
}

// Set the cursor position
int setcursor(int x, int y)
{
    // Check bounds and wrap if necessary
    if (x >= VGA_WIDTH) {
        y += x / VGA_WIDTH;
        x %= VGA_WIDTH;
    }
    if (y >= VGA_HEIGHT) {
        y %= VGA_HEIGHT;
    }

    cursorCol = x;
    cursorRow = y;

    // Update the cursor position in the VGA hardware
    uint16 position = cursorRow * VGA_WIDTH + cursorCol;
    outb(0x3D4, 0x0F);  // Send low byte of cursor position
    outb(0x3D5, (uint8)(position & 0xFF));
    outb(0x3D4, 0x0E);  // Send high byte of cursor position
    outb(0x3D5, (uint8)((position >> 8) & 0xFF));

    return 0;
}

// Print a character to the screen at the current cursor position
char putchar(char character) {
    uint16 *video = (uint16*) VIDEO_MEMORY;

    if (character == '\n') {
        // Move cursor to the next row and set column to 0
        cursorCol = 0;
        cursorRow++;
    } else {
        // Write the character to video memory
        video[cursorRow * VGA_WIDTH + cursorCol] = (TEXT_COLOR << 8) | character;
        cursorCol++;

        // Move to the next row if the cursor reaches the screen width
        if (cursorCol >= VGA_WIDTH) {
            cursorCol = 0;
            cursorRow++;
        }
    }

    // Handle screen wraparound or scrolling when the cursor reaches the bottom of the screen
    if (cursorRow >= VGA_HEIGHT) {
        cursorRow = VGA_HEIGHT - 1;  // Move cursor to the last row if screen wraps or scrolls
    }

    // Move the hardware cursor to the current position
    setcursor(cursorCol, cursorRow);

    return character;
}


// Print a string to the display
int printf(char string[])
{
    int count = 0;
    for (int i = 0; string[i] != 0; i++) {
        putchar(string[i]);
        count++;
    }
    return count;
}

// Clear the screen by filling it with spaces and reset the cursor
void clearscreen()
{
    uint16 *video = (uint16*) VIDEO_MEMORY;
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        video[i] = (TEXT_COLOR << 8) | ' ';
    }

    // Reset cursor position to top-left
    cursorCol = 0;
    cursorRow = 0;
    setcursor(0, 0);
}

void initkeymap() {
    // Map letters 'a' through 'z'
    keymap[0x1E] = 'a';
    keymap[0x30] = 'b';
    keymap[0x2E] = 'c';
    keymap[0x20] = 'd';
    keymap[0x12] = 'e';
    keymap[0x21] = 'f';
    keymap[0x22] = 'g';
    keymap[0x23] = 'h';
    keymap[0x17] = 'i';
    keymap[0x24] = 'j';
    keymap[0x25] = 'k';
    keymap[0x26] = 'l';
    keymap[0x32] = 'm';
    keymap[0x31] = 'n';
    keymap[0x18] = 'o';
    keymap[0x19] = 'p';
    keymap[0x10] = 'q';
    keymap[0x13] = 'r';
    keymap[0x1F] = 's';
    keymap[0x14] = 't';
    keymap[0x16] = 'u';
    keymap[0x2F] = 'v';
    keymap[0x11] = 'w';
    keymap[0x2D] = 'x';
    keymap[0x15] = 'y';
    keymap[0x2C] = 'z';

    // Map numbers '0' through '9'
    keymap[0x0B] = '0';
    keymap[0x02] = '1';
    keymap[0x03] = '2';
    keymap[0x04] = '3';
    keymap[0x05] = '4';
    keymap[0x06] = '5';
    keymap[0x07] = '6';
    keymap[0x08] = '7';
    keymap[0x09] = '8';
    keymap[0x0A] = '9';

    // Map special characters
    keymap[0x39] = ' ';  // Space
    keymap[0x1C] = '\n'; // Enter key (newline)
}
