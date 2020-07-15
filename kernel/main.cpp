/**
 * @file main.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief The entry point into the Panix kernel. Everything is loaded from here.
 * @version 0.3
 * @date 2019-11-14
 *
 * @copyright Copyright the Panix Contributors (c) 2019
 *
 */
// System library functions
#include <sys/types.hpp>
#include <sys/panic.hpp>
// Memory management & paging
#include <mem/heap.hpp>
#include <mem/paging.hpp>
// Intel i386 architecture
#include <arch/arch.hpp>
#include <gnu/multiboot.hpp>
// Generic devices
#include <devices/tty/tty.hpp>
#include <devices/smbios/smbios.hpp>
#include <devices/kbd/kbd.hpp>
#include <devices/rtc/rtc.hpp>
#include <devices/spkr/spkr.hpp>
#include <devices/serial/rs232.hpp>
// memcpy
#include <lib/string.hpp>
#include <lib/stdio.hpp>

#define __YEAR_C__ 7
#define __YEAR__  (\
                    ((__DATE__)[__YEAR_C__ + 0] - '0') * 1000 + \
                    ((__DATE__)[__YEAR_C__ + 1] - '0') * 100  + \
                    ((__DATE__)[__YEAR_C__ + 2] - '0') * 10   + \
                    ((__DATE__)[__YEAR_C__ + 3] - '0') * 1      \
                  )

void px_kernel_print_splash();
void px_kernel_check_multiboot(const multiboot_info_t* mb_struct);
void px_kernel_print_multiboot(const multiboot_info_t* mb_struct);
void px_kernel_boot_tone();
extern uint32_t placement_address;
/**
 * @brief The global constuctor is a necessary step when using
 * global objects which need to be constructed before the main
 * function, px_kernel_main() in our case, is ever called. This
 * is much more necessary in an object-oriented architecture,
 * so it is less of a concern now. Regardless, the OSDev Wiki
 * take a *very* different approach to this, so refactoring
 * this might be on the eventual todo list.
 *
 * According to the OSDev Wiki this is only necessary for C++
 * objects. However, it is useful to know that the
 * global constructors are "stored in a sorted array of
 * function pointers and invoking these is as simple as
 * traversing the array and running each element."
 *
 */
typedef void (*constructor)();
extern "C" constructor _CTORS_START;
extern "C" constructor _CTORS_END;
extern "C" void px_call_constructors() {
    // For each global object with a constructor starting at start_ctors,
    for (constructor* i = &_CTORS_START; i != &_CTORS_END; i++) {
        // Get the object and call the constructor manually.
        (*i)();
    }
}

/**
 * @brief This is the Panix kernel entry point. This function is called directly from the
 * assembly written in boot.S located in arch/x86/boot.S.
 */
extern "C" void px_kernel_main(const multiboot_info_t* mb_struct, uint32_t mb_magic) {
    // Print the splash screen to show we've booted into the kernel properly.
    px_kernel_print_splash();
    // Panix requires a multiboot header, so panic if not provided
    px_kernel_check_multiboot(mb_struct);
    // Install the GDT
    px_interrupts_disable();
    px_gdt_install();
    px_isr_install();           // Interrupt Service Requests
    // px_heap_init((uint32_t)&_EARLY_KMALLOC_START, (uint32_t)&_EARLY_KMALLOC_END);             // Early kernel memory allocation
    px_paging_init();           // Initialize paging service
    px_kbd_init();              // Keyboard
    px_rtc_init();              // Real Time Clock
    px_timer_init(1000);        // Programmable Interrupt Timer (1ms)
    px_rs_232_init(RS_232_COM1);// RS232 Serial
    // Now that we've initialized our core kernel necessities
    // we can initialize paging.
    // Enable interrupts now that we're out of a critical area
    px_interrupts_enable();
    // Print some info to show we did things right
    px_rtc_print();
    // Get the CPU vendor and model data to print
    char *vendor = (char *)px_cpu_get_vendor();
    char *model = (char *)px_cpu_get_model();
    px_print_debug(vendor, Info);
    px_print_debug(model, Info);
    // Start the serial debugger
    px_print_debug("Starting serial debugger...", Info);
    px_rs_232_print(vendor);
    px_rs_232_print(model);
    // Now that we're done make a joyful noise
    px_print_debug("Done.", Success);
    px_kernel_boot_tone();
    while (true) {
        // Keep the kernel alive.
        asm("hlt");
    }
    PANIC("Kernel terminated unexpectedly!");
}

void px_kernel_print_splash() {
    px_clear_tty();
    px_kprintf("\033[93mWelcome to Panix\n"
                "Developed by graduates and undergraduates of Cedarville University.\n"
                "Copyright Keeton Feavel et al (c) %i. All rights reserved.\n\033[0m", __YEAR__);
    px_kprintf("Built on %s at %s.\n\n", __DATE__, __TIME__);
}

void px_kernel_check_multiboot(const multiboot_info_t* mb_struct) {
    if (mb_struct == nullptr) {
        PANIC("Multiboot info missing. Please use a Multiboot compliant bootloader (like GRUB).");
    }
    // Print multiboot information
    px_kernel_print_multiboot(mb_struct);
}

void px_kernel_print_multiboot(const multiboot_info_t* mb_struct) {
    // Print out our memory size information if provided
    if (mb_struct->flags & MULTIBOOT_INFO_MEMORY) {
        px_kprintf(
            "Memory Lower: \033[95m0x%08X\n\033[0mMemory Upper: \033[95m0x%08X\n\033[0mTotal Memory: \033[95m0x%08X\033[0m\n",
            mb_struct->mem_lower,
            mb_struct->mem_upper,
            (mb_struct->mem_lower + mb_struct->mem_upper)
        );
    }
    // Print out our memory map if provided
    if (mb_struct->flags & MULTIBOOT_INFO_MEM_MAP) {
        uint32_t *mem_info_ptr = (uint32_t *)mb_struct->mmap_addr;
        // While there are still entries in the memory map
        while (mem_info_ptr < (uint32_t *)(mb_struct->mmap_addr + mb_struct->mmap_length)) {
            multiboot_memory_map_t *curr = (multiboot_memory_map_t *)mem_info_ptr;
            // If the length of the current map entry is not empty
            if (curr->len > 0) {
                // Print out the memory map information
                px_kprintf("\n[0x%08X-0x%08X] ", curr->addr, (curr->addr + curr->len));
                // Print out if the entry is available or reserved
                curr->type == MULTIBOOT_MEMORY_AVAILABLE ? px_kprintf("Available") : px_kprintf("Reserved");
            } else {
                px_kprintf("\033[91mMissing!\033[0m");
            }
            // Increment the curr pointer to the next entry
            mem_info_ptr += curr->size + sizeof(curr->size);
        }
    }
    px_kprintf("\n\n");
}

void px_kernel_boot_tone() {
    // Beep beep!
    px_spkr_beep(1000, 50);
    sleep(100);
    px_spkr_beep(1000, 50);
}
