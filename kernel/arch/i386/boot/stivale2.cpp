/**
 * @file stivale2.cpp
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
#include <stivale/stivale2.h>

extern "C" void px_boot_stivale2(void *info);
extern "C" void px_kernel_main(void *boot_info, uint32_t magic);
extern uint8_t px_boot_stack[4096];

/**
 * @brief Entry point for Stivale2 boot protocol.
 */
extern "C" void px_boot_stivale2(void *info) {
    px_kernel_main(info, 0x73747632);
}

/*
static struct stivale2_header_tag_framebuffer stivale_tag_framebuffer {
    .tag = {
        // Identification constant defined in stivale2.h and the specification.
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        // If next is 0, then this marks the end of the linked list of tags.
        .next = 0
    },
    .framebuffer_width = 1280,
    .framebuffer_height = 720,
    .framebuffer_bpp = 32
};
*/

__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header stivale_hdr = {
    // Stack located in stack.hpp
    .entry_point = (uint64_t)px_boot_stivale2,
    .stack = (uintptr_t)px_boot_stack + sizeof(px_boot_stack),
    .flags = 0,
    .tags = 0,
};
