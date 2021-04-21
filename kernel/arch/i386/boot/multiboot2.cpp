/**
 * @file multiboot2.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Stivale2 boot protocol early boot procedue
 * @version 0.1
 * @date 2021-04-20
 * 
 * @copyright Copyright the Panix Contributors (c) 2021
 * 
 */
#include <stdint.h>
#include <arch/stack.hpp>
#include <multiboot/multiboot2.h>

extern "C" void px_boot_multiboot2(void *info, uint32_t magic);
extern "C" void px_kernel_main(void *boot_info, uint32_t magic);

/**
 * @brief Entry point for Stivale2 boot protocol.
 */
extern "C" void px_boot_multiboot2(void *info, uint32_t magic) {
    px_kernel_main(info, magic);
}
