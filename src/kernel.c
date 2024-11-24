#include "./io.h"
#include "./multitasking.h"

// Function declarations for each letter printing process
void proca();
void procb();
void procc();
void procd();
void proce();

void prockernel();

int main()
{
    // Clear the screen and initialize necessary settings
    clearscreen();

    // Start the kernel process
    startkernel(prockernel);

    return 0;
}

// Kernel process function with hardcoded execution order
void prockernel()
{
    printf("Kernel Process Starting...\n");

    //  invoke each process in the required order
    proca();  // Prints "A"
    procb();  // Prints "B"
    procc();  // Prints "C"
    procd();  // Prints "D"
    proce();  // Prints "E"
    procb();  // Prints "B"
    procc();  // Prints "C"
    procd();  // Prints "D"
    proce();  // Prints "E"
    procc();  // Prints "C"
    procd();  // Prints "D"
    procc();  // Prints "C"

    printf("\nKernel Process Exiting...\n");
}

void proca() {
    printf("A");
}

void procb() {
    printf("B");
}

void procc() {
    printf("C");
}

void procd() {
    printf("D");
}

void proce() {
    printf("E");
}
