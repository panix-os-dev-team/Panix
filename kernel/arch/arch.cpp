/**
 * @file arch.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.3
 * @date 2020-06-01
 * 
 * @copyright Copyright the Panix Contributors (c) 2020
 * 
 */
#include <arch/arch.hpp>

const char* px_exception_descriptions[] = {
    "Divide-By-Zero", "Debugging", "Non-Maskable", "Breakpoint",
    "Overflow", "Out Bound Range", "Invalid Opcode", "Device Not Avbl",
    "Double Fault", "Co-CPU Overrun", "Invalid TSS", "Sgmnt !Present",
    "Seg Fault", "Protection Flt", "Page Fault", "RESERVED",
    "Floating Pnt", "Alignment Check", "Machine Check", "SIMD Flt Pnt",
    "Virtualization", "RESERVED", "RESERVED", "RESERVED",
    "RESERVED", "RESERVED", "RESERVED", "RESERVED",
    "RESERVED", "Security Excptn", "RESERVED", "Triple Fault", "FPU Error"
};

/**
 * @brief 
 * 
 * @param code 
 * @param str 
 * @return int 
 */
static inline void px_arch_cpuid(int flag, unsigned long eax, unsigned long ebx, unsigned long ecx, unsigned long edx)
{
    __asm__ volatile ("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(flag));
}

static inline int px_arch_cpuid(int flag, int regs[4]) {
    // ECX and EDX are swapped in order to make the strings readable
    __asm__ volatile ("cpuid" : "=a"(*regs), "=b"(*(regs+1)), "=c"(*(regs+2)), "=d"(*(regs+3)) : "a"(flag));
    return (int)regs[0];
}

static inline int px_arch_cpuid_vendor(int flag, int regs[4]) {
    // ECX and EDX are swapped in order to make the strings readable
    __asm__ volatile ("cpuid" : "=a"(*regs), "=b"(*(regs+0)), "=d"(*(regs+1)), "=c"(*(regs+2)) : "a"(flag));
    return (int)regs[0];
}

const char* const px_cpu_get_vendor() {
	static char vendor[16];
	px_arch_cpuid_vendor(0, (int *)(vendor));
	return vendor;
}

const char* const px_cpu_get_model() {
    // The CPU model is broken up across 3 different calls, each using
    // EAX, EBX, ECX, and EDX to store the string, so we basically
    // are appending all 4 register values to this char array each time.
    static char model[48];
	px_arch_cpuid(0x80000002, (int *)(model));
    px_arch_cpuid(0x80000003, (int *)(model+16));
    px_arch_cpuid(0x80000004, (int *)(model+32));
	return model;
}