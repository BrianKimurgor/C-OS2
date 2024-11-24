#include "./types.h"
#include "./multitasking.h"
#include "./io.h"

// Array to hold all processes
proc_t processes[MAX_PROCS];
uint8 process_index = 0;  // Tracks the next index for new processes

// Process pointers
proc_t *prev;
proc_t *running;
proc_t *next;
proc_t *kernel;

// Cooperative Round Robin scheduling function
int schedule()
{
    int start = running->pid;  // Start from the current running process
    int next_pid = (start + 1) % process_index;  // Round Robin logic

    // Loop to find the next ready user process
    for (int i = 0; i < process_index; i++) {
        if (processes[next_pid].status == PROC_READY && processes[next_pid].type == PROC_USER) {
            next = &processes[next_pid];
            return 1;  // Found a ready user process
        }
        next_pid = (next_pid + 1) % process_index;
    }

    // No ready user processes, switch back to kernel
    next = kernel;
    return 0;
}

// Creates a new user process
int createproc(void *func, char *stack)
{
    if (process_index >= MAX_PROCS)
        return -1;  // Check max process limit

    proc_t *new_proc = &processes[process_index];
    new_proc->pid = process_index;
    new_proc->type = PROC_USER;
    new_proc->status = PROC_READY;
    new_proc->eip = func;
    new_proc->esp = stack;
    new_proc->ebp = stack;

    process_index++;
    return 0;
}

// Creates and starts a kernel process
int startkernel(void func())
{
    if (process_index >= MAX_PROCS) {
        return -1;
    }

    // Create the kernel process
    proc_t kernproc;
    kernproc.status = PROC_RUNNING;
    kernproc.type = PROC_KERNEL;
    kernproc.pid = process_index;
    processes[process_index] = kernproc;
    kernel = &processes[process_index]; // Keep track of the kernel process
    process_index++;

    // Assign the kernel as the running process and execute
    running = kernel;
    func();

    return 0;
}

// Terminates the currently running process
void exit()
{
    running->status = PROC_TERMINATED;
    running = kernel;  // Switch to kernel after user process terminates
    switchcontext();
}

// Yields the current process
void yield()
{
    if (running->type == PROC_USER) {
        running->status = PROC_READY;  // User process yields, set to ready
        running = kernel;  // Switch back to kernel process
    } else {
        if (schedule()) {
            running = next;  // Switch to scheduled user process
        } else {
            printf("No user processes available!\n");
            while (1);  // Halt if no processes are ready
        }
    }
    switchcontext();
}

// Context switching function
void __attribute__((naked)) switchcontext()
{
    // Save registers for the current running process
    register uint32 eax asm("eax");
    register uint32 ebx asm("ebx");
    register uint32 ecx asm("ecx");
    register uint32 edx asm("edx");
    register uint32 esi asm("esi");
    register uint32 edi asm("edi");
    register void *ebp asm("ebp");
    register void *esp asm("esp");

    asm volatile("pushfl");
    asm volatile("pop %eax");
    register uint32 eflags asm("eax");

    asm volatile("mov %cr3, %eax");
    register uint32 cr3 asm("eax");

    // Store current register values for the running process
    running->eax = eax;
    running->ebx = ebx;
    running->ecx = ecx;
    running->edx = edx;
    running->esi = esi;
    running->edi = edi;
    running->ebp = ebp;
    running->esp = esp;
    running->eflags = eflags;
    running->cr3 = cr3;
    running->eip = &&resume;

    // Switch to the next process
    running = next;
    running->status = PROC_RUNNING;

    // Load register values for the next process
    asm volatile("mov %0, %%eax" : : "r"(running->eflags));
    asm volatile("push %eax");
    asm volatile("popfl");

    asm volatile("mov %0, %%esi" : : "r"(running->esi));
    asm volatile("mov %0, %%edi" : : "r"(running->edi));
    asm volatile("mov %0, %%ebp" : : "r"(running->ebp));
    asm volatile("mov %0, %%esp" : : "r"(running->esp));
    asm volatile("mov %0, %%cr3" : : "r"(running->cr3));

    asm volatile("mov %0, %%eax" : : "r"(running->eax));
    asm volatile("mov %0, %%ebx" : : "r"(running->ebx));
    asm volatile("mov %0, %%ecx" : : "r"(running->ecx));
    asm volatile("mov %0, %%edx" : : "r"(running->edx));

    

    // Jump to the process’s saved instruction
    asm volatile("jmp *%0" : : "r"(running->eip));

resume:
    asm volatile("ret");
}
