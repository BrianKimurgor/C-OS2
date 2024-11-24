#ifndef TYPES_H
#define TYPES_H



// Signed integers (8 bit, 16 bit, and 32 bit)
typedef signed char int8;
typedef signed short int16;
typedef signed int int32;

// Unsigned integers (8 bit, 16 bit, and 32 bit)
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

// Define directory_entry_t
typedef struct {
    char filename[8];       // File name (8 characters)
    char extension[3];      // File extension (3 characters)
    uint16 startingCluster; // Starting cluster number
    uint32 fileSize;        // File size in bytes
} directory_entry_t;

#endif // TYPES_H
