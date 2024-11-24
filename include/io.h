#include "./types.h"

// Define our constants that will be widely used
#define VGA_WIDTH 80  ///< Screen width in columns.
#define VGA_HEIGHT 25 ///< Screen height in rows.
#define VIDEO_MEMORY 0xB8000 ///< Start of video memory in VGA text mode.
#define TEXT_COLOR 0x07 ///< Default text color (white on black).
#define VIDEO_MEM 0xB8000 ///< Alias for video memory start address.
#define SCREEN_WIDTH 80 ///< Alias for screen width in columns.
#define SCREEN_HEIGHT 25 ///< Alias for screen height in rows.
#ifndef KEYBOARD_H
#define KEYBOARD_H

// Define our function prototypes


// Keymap array to convert scancodes to ASCII characters
extern unsigned char keymap[128];

// Function to initialize the keymap with PS/2 scancode mappings
void initkeymap();

#endif


/**
 * @brief Writes a byte to a specified I/O port.
 *
 * @param port The I/O port number to write to.
 * @param value The byte value to write.
 */
void outb(uint16 port, uint8 value);

/**
 * @brief Writes a word (2 bytes) to a specified I/O port.
 *
 * @param port The I/O port number to write to.
 * @param value The word value to write.
 */
void outw(uint16 port, uint16 value);

/**
 * @brief Reads a byte from a specified I/O port.
 *
 * @param port The I/O port number to read from.
 * @return The byte value read from the port.
 */
uint8 inb(uint16 port);

/**
 * @brief Reads a word (2 bytes) from a specified I/O port.
 *
 * @param port The I/O port number to read from.
 * @return The word value read from the port.
 */
uint16 inw(uint16 port);

/**
 * @brief Initializes the keyboard mapping.
 */
void initkeymap();

/**
 * @brief Retrieves a character input from the user.
 *
 * @return The character input by the user.
 */
char getchar();

/**
 * @brief Reads a string input from the user and stores it at the specified address.
 *
 * @param address Pointer to the character array where the input will be stored.
 */
void scanf(char address[]);

/**
 * @brief Sets the cursor position on the screen.
 *
 * @param x The x-coordinate (column) of the cursor.
 * @param y The y-coordinate (row) of the cursor.
 * @return 0 on success, or a negative error code on failure.
 */
int setcursor(int x, int y);

/**
 * @brief Outputs a character to the screen.
 *
 * @param character The character to be displayed.
 * @return The character that was displayed.
 */
char putchar(char character);

/**
 * @brief Outputs a formatted string to the screen.
 *
 * @param string The string to display.
 * @return The number of characters printed.
 */
// int printf(char string[]);
int kprintf(const char *format, ...);  // Add this for formatted printing


/**
 * @brief Outputs an integer value to the screen.
 *
 * @param n The integer value to display.
 * @return The number of characters printed.
 */
int printint(uint32 n);

/**
 * @brief Clears the screen by filling it with the default background color.
 */
void clearscreen();
