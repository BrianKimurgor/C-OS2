#include "hal.h"
#include "io.h"       // For inb and outb functions

// Polling-based getchar function to read a character from the keyboard
char getchar() {
    unsigned char scancode;

    // Poll the keyboard's status port (0x64) until it indicates that data is ready
    while (!(inb(0x64) & 0x01));  // Wait until the keyboard is ready to send data

    // Read the scancode from the keyboard data port (0x60)
    scancode = inb(0x60);

    // Check if it's a key press (not a key release)
    if (scancode & 0x80) {
        // Key release, so ignore it by returning 0
        return 0;
    }

    // Return the corresponding ASCII character from the keymap defined in io.c
    return keymap[scancode];
}

// scanf function to read a string from the keyboard input
void scanf(char string[]) {
    char input;
    int i = 0;

    // Loop to collect characters until the Enter key is pressed
    while (1) {
        input = getchar();  // Get the input from the keyboard

        // If Enter key is pressed, terminate the string and return
        if (input == '\n') {
            string[i] = '\0';  // Null-terminate the string
            putchar('\n');     // Move to the next line (optional)
            break;
        }

        // If valid input, store it in the string and print it to the display
        if (input) {
            string[i] = input;
            putchar(input);    // Echo the character on the screen
            i++;

            // Ensure the user doesn't exceed the 100-character limit
            if (i >= 100) {
                string[i] = '\0';  // Null-terminate the string if the limit is reached
                break;
            }
        }
    }
}
