#include <stdint.h>  // For uint32_t, uint16_t, uint8_t types
#include <stddef.h>  // For NULL
#include <string.h>  // For string manipulation and memset
#include "types.h"
#include "fdc.h"


// Assuming structure definitions (adjust as necessary):
typedef struct {
    uint16_t startingCluster; // First cluster of the file
    uint32_t fileSize;        // File size in bytes
    char filename[8];         // File name (8 characters)
    char extension[3];        // File extension (3 characters)
} file_entry_t;

typedef struct {
    file_entry_t *entry;      // Pointer to the file entry in the FAT
    uint32_t startingAddress; // Memory address where file data is loaded
    uint8_t isOpened;         // 1 if file is open, 0 otherwise
} file_t;

typedef struct {
    char filename[8];          // File name (8 characters max)
    char extension[3];         // Extension (3 characters max)
    file_entry_t *entry;       // Pointer to the file entry
    uint32_t startingAddress;  // Address where directory is loaded
} directory_t;

typedef struct {
    uint16_t entries[2304]; // File Allocation Table with enough entries for FAT16
} fat_t;

// Global FAT table pointers
fat_t *fat0, *fat1;

// Helper function to find the next free cluster
uint16_t findFreeCluster() {
    for (uint16_t i = 2; i < 2304; i++) { // Clusters start at 2
        if (fat0->entries[i] == 0) {     // Free cluster is marked with 0
            return i;
        }
    }
    return 0xFFFF; // No free cluster found
}

// Helper function to load a file from the disk into memory
void loadFile(file_t *file, uint16_t startingCluster) {
    uint16_t currentCluster = startingCluster;
    uint32_t index = 0;

    while (currentCluster != 0xFFFF) { // 0xFFFF indicates end of the cluster chain
        // Load the cluster's data into the file's starting address
        floppy_read(0, 33 + currentCluster - 2, (void *)(file->startingAddress + index), 1);
        index += 512; // Move to the next sector
        currentCluster = fat0->entries[currentCluster]; // Get the next cluster
    }
}

// 1. Open file function
int openFile(file_t *file) {
    if (file == NULL || file->entry == NULL) {
        return -1; // Error: Invalid file or file entry
    }

    loadFile(file, file->entry->startingCluster); // Load file data
    file->isOpened = 1; // Mark as opened
    return 0;           // Success
}

// 2. Read a byte from the file
uint8_t readByte(file_t *file, uint32_t index) {
    if (file == NULL || file->isOpened == 0) {
        return 0xFF; // Error: File not open
    }
    return *((uint8_t *)(file->startingAddress + index)); // Return the byte at the index
}

// 3. Write a byte to the file
int writeByte(file_t *file, uint8_t byte, uint32_t index) {
    if (file == NULL || file->isOpened == 0) {
        return -1; // Error: File not open
    }
    *((uint8_t *)(file->startingAddress + index)) = byte; // Write the byte
    file->entry->fileSize++;                              // Increment the file size
    return 0;                                             // Success
}

// 4. Close the file
void closeFile(file_t *file) {
    if (file == NULL || file->isOpened == 0) {
        return; // Error: File not open
    }

    uint16_t currentCluster = file->entry->startingCluster;
    uint32_t remainingSize = file->entry->fileSize;
    uint32_t index = 0;

    while (remainingSize > 0) {
        uint32_t sectorSize = remainingSize > 512 ? 512 : remainingSize;
        floppy_write(0, 33 + currentCluster - 2, (void *)(file->startingAddress + index), 1);
        index += sectorSize;
        remainingSize -= sectorSize;

        if (remainingSize > 0 && fat0->entries[currentCluster] == 0xFFFF) {
            uint16_t newCluster = findFreeCluster();
            if (newCluster == 0xFFFF) {
                return; // Error: No free cluster available
            }
            fat0->entries[currentCluster] = newCluster;
            fat0->entries[newCluster] = 0xFFFF; // Mark as end of chain
        }
        currentCluster = fat0->entries[currentCluster];
    }
    file->isOpened = 0; // Mark as closed
}

// 5. Create a new file
int createFile(file_t *file, directory_t *parent) {
    if (file == NULL || parent == NULL || parent->entry == NULL) {
        return -1; // Error: Invalid file or parent
    }

    uint16_t freeCluster = findFreeCluster();
    if (freeCluster == 0xFFFF) {
        return -1; // Error: No free cluster
    }

    file_entry_t *entry = parent->entry;
    memset(entry, 0, sizeof(file_entry_t)); // Clear the entry
    memcpy(entry->filename, file->entry->filename, 8);
    memcpy(entry->extension, file->entry->extension, 3);
    entry->startingCluster = freeCluster;
    entry->fileSize = 0;
    fat0->entries[freeCluster] = 0xFFFF; // Mark end of chain

    closeFile(file); // Save the file to disk
    return 0;        // Success
}

// 6. Delete a file
void deleteFile(file_t *file, directory_t *parent) {
    if (file == NULL || parent == NULL || file->entry == NULL) {
        return; // Error: Invalid file or parent
    }

    uint16_t currentCluster = file->entry->startingCluster;

    while (currentCluster != 0xFFFF) {
        uint16_t nextCluster = fat0->entries[currentCluster];
        fat0->entries[currentCluster] = 0; // Mark cluster as free
        currentCluster = nextCluster;
    }

    memset(file->entry, 0, sizeof(file_entry_t)); // Clear the file entry
}
